/****************************************************************************
 *
 * Copyright 2024 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <tinyara/config.h>
#include <assert.h>
#include <debug.h>
#include <errno.h>
#include <tinyara/pm/pm.h>
#include <tinyara/clock.h>
#include <tinyara/irq.h>
#include <tinyara/arch.h>
#include <tinyara/wdog.h>
#include "../kernel/clock/clock.h"
#include "../kernel/sched/sched.h"

#include "pm.h"

#ifdef CONFIG_PM

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define PM_DEBUG_STR "."

/* enter_sleep() prints PM_DEBUG_STR after each stage and the wakeup reason
 * in between, so one sleep/wakeup cycle takes one console line and a hang
 * shows the last stage that was reached.  The NuttX syslog prepends the CPU
 * and the function name to every message and ends it with a newline, so
 * this trace writes to the low-level console instead.
 */

#ifdef CONFIG_DEBUG_ERROR
#  define pm_trace lowsyslog
#else
#  define pm_trace _none
#endif

/****************************************************************************
 * Private Variables
 ****************************************************************************/

static clock_t stime;

#if defined(CONFIG_SMP) && defined(CONFIG_CPU_HOTPLUG)
/* Secondary CPUs which were online when the current sleep cycle started.
 * Only these are turned off for the sleep and turned on again afterwards.
 */

static cpu_set_t g_active_cpu_snapshot;
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: suspend_devices
 *
 * Description:
 *   This function suspend devices. It is called before entering sleep mode.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   OK on success, ERROR on failure if state change fails.
 *
 ****************************************************************************/
static int suspend_devices(void)
{
	/* Then force the global state change */
	if (pm_changestate(PM_SLEEP) < 0) {
		/* The new state change failed */
		pmlldbg("State change failed! newstate = %d\n", PM_SLEEP);
		return ERROR;
	}
	return OK;
}

/****************************************************************************
 * Name: resume_devices
 *
 * Description:
 *   This function resume devices. It is called after waking up from sleep mode.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/
static void resume_devices(void)
{
	/* After wakeup, restore the PM state to NORMAL. */
	(void)pm_changestate(PM_NORMAL);
}

#if defined(CONFIG_SMP) && defined(CONFIG_CPU_HOTPLUG)
/****************************************************************************
 * Name: disable_secondary_cpus
 *
 * Description:
 *   This function turns off the secondary CPUs of the snapshot before
 *   entering sleep mode.  sched_cpuoff() makes the final idle check of each
 *   CPU inside the critical section and returns -EBUSY when the CPU cannot
 *   leave the scheduler now; the sleep is then skipped for this cycle.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   OK on success; a negated errno value if a CPU could not be turned off.
 *   The CPUs turned off before the failure stay off until
 *   enable_secondary_cpus() is called.
 *
 ****************************************************************************/
static int disable_secondary_cpus(void)
{
	int cpu;
	int ret;

	for (cpu = 1; cpu < CONFIG_SMP_NCPUS; cpu++) {
		if (!CPU_ISSET(cpu, &g_active_cpu_snapshot)) {
			continue;
		}

		ret = sched_cpuoff(cpu);
		if (ret == -EBUSY) {
			pmllvdbg("Sleep abort! CPU%d is busy\n", cpu);
			return ret;
		} else if (ret < 0) {
			pmlldbg("CPU%d shutdown failed: %d\n", cpu, ret);
			return ret;
		}
	}

	return OK;
}

/****************************************************************************
 * Name: enable_secondary_cpus
 *
 * Description:
 *   This function turns on again the secondary CPUs of the snapshot which
 *   are offline now, i.e. the CPUs that disable_secondary_cpus() turned off
 *   in this cycle, including after a partial failure.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/
static void enable_secondary_cpus(void)
{
	cpu_set_t online = sched_getactivecpu();
	int cpu;
	int ret;

	for (cpu = 1; cpu < CONFIG_SMP_NCPUS; cpu++) {
		if (!CPU_ISSET(cpu, &g_active_cpu_snapshot) || CPU_ISSET(cpu, &online)) {
			continue;
		}

		ret = sched_cpuon(cpu);
		if (ret < 0) {
			pmlldbg("CPU%d power on failed: %d\n", cpu, ret);
		}
	}
}

/****************************************************************************
 * Name: check_secondary_cpus_idle
 *
 * Description:
 *   This function checks if all secondary CPUs are idle and ready for sleep.
 *   If any CPU is not idle, it aborts the sleep before the devices are
 *   suspended.  It also saves a snapshot of the online CPUs to
 *   g_active_cpu_snapshot, which is later used by disable/enable_secondary
 *   _cpus functions.  The result is stable because the caller holds the
 *   critical section.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   OK if all secondary CPUs are idle, ERROR if sleep should be aborted.
 *
 ****************************************************************************/
static int check_secondary_cpus_idle(void)
{
	FAR struct tcb_s *tcb;
	int cpu;

	g_active_cpu_snapshot = sched_getactivecpu();

	for (cpu = 1; cpu < CONFIG_SMP_NCPUS; cpu++) {
		if (!CPU_ISSET(cpu, &g_active_cpu_snapshot)) {
			continue;
		}

		tcb = current_task(cpu);
		if (!is_idle_task(tcb)) {
			pmllvdbg("Sleep abort! CPU%d task: %s!\n", cpu, tcb->name);
			return ERROR;
		}
	}

	return OK;
}
#else
#define disable_secondary_cpus()    (OK)
#define enable_secondary_cpus()
#define check_secondary_cpus_idle() (OK)
#endif

/****************************************************************************
 * Name: disable_systick
 *
 * Description:
 *   This function disables the system tick timer before entering sleep.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   OK on success.
 *
 ****************************************************************************/
static int disable_systick(void)
{
	return up_timer_disable();
}

/****************************************************************************
 * Name: enable_and_compensate_systick
 *
 * Description:
 *   This function enables the system tick timer after waking up and
 *   compensates for any ticks missed during sleep. It updates the
 *   system time and timer accordingly.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/
static void enable_and_compensate_systick(void)
{
#ifdef CONFIG_PM_TICKSUPPRESS
	struct pm_sleep_ops *sleep_ops = g_pmglobals.sleep_ops;
	clock_t missing_tick;

	if (!sleep_ops || !sleep_ops->get_missingtick) {
		goto enable_timer;
	}

	missing_tick = sleep_ops->get_missingtick();

	pmllvdbg("missing_tick: %llu\n", (unsigned long long)missing_tick);
	pm_metrics_update_missing_tick(missing_tick);

	if (missing_tick > 0) {
		/* Correct the system time for the ticks missed in sleep.  Watchdogs
		 * keep absolute expiry ticks, so no watchdog is adjusted here: the
		 * next tick interrupt expires every watchdog that became due during
		 * the sleep, after the wake-up interrupt has been serviced.
		 *
		 *     WAKE UP -> HW IRQ ISR -> THREAD -> TICK ISR
		 *           |              |          |           |
		 *           +--------------+----------+-----------+
		 *     (corrects tick)                       (expire timer)
		 */
		clock_increase_sched_ticks(missing_tick);
	}

enable_timer:
#endif
	(void)up_timer_enable();
}

#ifdef CONFIG_PM_TIMEDWAKEUP
/****************************************************************************
 * Name: get_next_wakeup_time
 *
 * Description:
 *   This function gets the next wakeup time from the watchdog timer.
 *   If the delay is too short (less than SLEEP_ENTRY_WAIT), it returns ERROR
 *   to abort sleep.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   The next wakeup time in ticks, or ERROR if sleep should be aborted.
 *
 ****************************************************************************/
static int get_next_wakeup_time(void)
{
	clock_t delay;

	/* Get the delay of the earliest pm_sleep() wake-up timer */
	delay = wd_getwakeupdelay(pm_sleep_timeout);
	if ((delay > 0) && (delay < MSEC2TICK(CONFIG_PM_SLEEP_ENTRY_WAIT_MS))) {
		pmllvdbg("Wdog Timer Delay: %ldms is less than SLEEP_ENTRY_WAIT: %dms\n", (long)TICK2MSEC(delay), CONFIG_PM_SLEEP_ENTRY_WAIT_MS);
		return ERROR;
	}

	return (int)delay;
}

/****************************************************************************
 * Name: set_pm_wakeup_timer
 *
 * Description:
 *   This function sets the wakeup timer to wake up the system after
 *   the specified delay. It uses the sleep_ops to set the timer.
 *
 * Input Parameters:
 *   delay - The wakeup delay in ticks.
 *
 * Returned Value:
 *   OK on success, or the error code from sleep_ops->set_timer on failure.
 *
 ****************************************************************************/
static int set_pm_wakeup_timer(int delay) 
{
	if (delay > 0) {
		/* set wakeup timer */
		pmllvdbg("Setting timer and board will wake up after %ld millisecond\n", (long)TICK2MSEC(delay));
		return g_pmglobals.sleep_ops->set_timer(TICK2USEC(delay));
	}

	return OK;
}
#else
#define get_next_wakeup_time()       (0)
#define set_pm_wakeup_timer(delay)  (OK)
#endif

/****************************************************************************
 * Name: update_wakeup_reason
 *
 * Description:
 *   This function updates the wakeup reason by querying the sleep_ops.
 *   It logs the wakeup source and updates PM metrics.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/
static void update_wakeup_reason(void)
{
	pm_wakeup_reason_code_t wakeup_src;
	struct pm_sleep_ops *sleep_ops = g_pmglobals.sleep_ops;

	if (sleep_ops && sleep_ops->get_wakeupreason) {
		wakeup_src = sleep_ops->get_wakeupreason();
		if (wakeup_src >= PM_WAKEUP_SRC_COUNT) {
			wakeup_src = PM_WAKEUP_UNKNOWN;
		}

		pm_trace(": %s", wakeup_src_name[wakeup_src]);
		pm_metrics_update_wakeup_reason(wakeup_src);
	}
}

/****************************************************************************
 * Name: check_pm_state
 *
 * Description:
 *   This function checks if the system can enter sleep mode.
 *   It verifies the PM state and checks if all secondary CPUs are idle.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   OK if the system can enter sleep, ERROR if sleep should be aborted.
 *
 ****************************************************************************/
static int check_pm_state(void) 
{
	/* Decide, which power saving level can be obtained */
	if (pm_checkstate() != PM_SLEEP) {
		return ERROR;
	}

	if (check_secondary_cpus_idle() != 0) {
		return ERROR;
	}

	return OK;
}

/****************************************************************************
 * Name: enter_sleep
 *
 * Description:
 *   This function handles the entire sleep sequence. It gets the wakeup time,
 *   suspends devices, disables secondary CPUs and systick, sets the wakeup
 *   timer, enters sleep, and then resumes everything on wakeup.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/
static void enter_sleep(void)
{
	int next_wakeup_time;
	struct pm_sleep_ops *sleep_ops = g_pmglobals.sleep_ops;

	DEBUGASSERT(sleep_ops);

	next_wakeup_time = get_next_wakeup_time();
	if (next_wakeup_time < 0) {
		return;
	}

	pm_trace(PM_DEBUG_STR);
	if (suspend_devices() != OK) {
		goto DEVICES_RESUME;
	}

	pm_trace(PM_DEBUG_STR);
	if (disable_secondary_cpus() != OK) {
		goto CPUS_ENABLE;
	}

	if (disable_systick() != OK) {
		goto SYSTICK_ENABLE;
	}

	if (set_pm_wakeup_timer(next_wakeup_time) != OK) {
		goto SYSTICK_ENABLE;
	}

	pm_trace(PM_DEBUG_STR);
	if (sleep_ops->sleep && sleep_ops->sleep() != 0) {
		goto SYSTICK_ENABLE;
	}

	update_wakeup_reason();

SYSTICK_ENABLE:
	enable_and_compensate_systick();
	pm_trace(PM_DEBUG_STR);

CPUS_ENABLE:
	enable_secondary_cpus();
	pm_trace(PM_DEBUG_STR);

DEVICES_RESUME:
	resume_devices();
	pm_trace(PM_DEBUG_STR"\n");
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: pm_idle
 *
 * Description:
 *   This function is called by IDLE thread to make board sleep. This function
 *   also allow to set wake up timer & handler and do all the PM pre processing
 *   required before going to sleep.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void pm_idle(void)
{
	irqstate_t flags;
	clock_t now;
	/* State change only if PM is ready to state change */
	if (!pm_is_running()) {
		return;
	}

	now = clock_systime_ticks();
	/* We need to check and change PM state transition only if one tick time has been passed,
	 * because state transition only happens when CPU receive TICK INTERRUPT. So checking pm state
	 * multiple times within one tick is waste of CPU clocks and we should avoid it.
	 */
	if (now <= stime) {
		return;
	}	
	
	flags = enter_critical_section();
	sched_lock();

	pm_metrics_update_idle();

	if (check_pm_state() == OK) {
		enter_sleep();
	}

	stime = clock_systime_ticks();

	sched_unlock();
	leave_critical_section(flags);
}

#endif /* CONFIG_PM */
