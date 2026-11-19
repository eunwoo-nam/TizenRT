/****************************************************************************
 *
 * Copyright 2025 Samsung Electronics All Rights Reserved.
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
 * kernel/sched/sched_cpuoff.c
 *
 * Description:
 *   This file implements the CPU control's power off function  which handles
 *   CPU off operations both from user applications (via CPU driver) and
 *   from the PM idle logic.
 *
 *   A CPU is never stopped from an interrupt handler.  The requester only
 *   publishes an OFF request and calls up_cpu_stop(), which wakes the
 *   target without taking any kernel lock, waits until the firmware reports
 *   it off and cuts its power.  The target accepts the request from its own
 *   IDLE loop, where it holds no kernel lock, and enters the firmware
 *   CPU_OFF service through up_cpu_die().  The requester may therefore keep
 *   the critical section while it waits: the wait is bounded, and a request
 *   that the target has not accepted in time is withdrawn atomically.
 *
 ****************************************************************************/
/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <tinyara/config.h>

#include <sched.h>
#include <debug.h>
#include <errno.h>
#include <assert.h>

#include <tinyara/arch.h>
#include <tinyara/atomic.h>
#include <tinyara/spinlock.h>
#include <tinyara/irq.h>
#include <tinyara/sched.h>

#include "sched/sched.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* State of the OFF request of a CPU */

#define CPUOFF_NONE        0	/* No request */
#define CPUOFF_REQUESTED   1	/* Published by the requester */
#define CPUOFF_ACCEPTED    2	/* Taken by the target, entering CPU_OFF */
#define CPUOFF_FAILED      3	/* The target returned from CPU_OFF */

/****************************************************************************
 * Private Data
 ****************************************************************************/

static atomic_t g_cpuoff_req[CONFIG_SMP_NCPUS];

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: __check_cpuoff_ready
 *
 * Description:
 *   Check, inside the critical section, that the target CPU is running its
 *   IDLE task and has no scheduler request in flight.  A pending task
 *   delivery or SMP call would make the target busy-wait for the critical
 *   section from its interrupt handler, so it could not accept the request.
 *
 * Input Parameters:
 *   cpu - The target CPU index.
 *
 * Returned Value:
 *   true if the target can be stopped now.
 *
 ****************************************************************************/

static bool __check_cpuoff_ready(int cpu)
{
	if (!is_idle_task(current_task(cpu))) {
		return false;
	}

	if (g_delivertasks[cpu] != SWITCH_NONE) {
		return false;
	}

	return !nxsched_smp_call_pending(cpu);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: nxsched_process_cpuoff
 *
 * Description:
 *   Accept a pending OFF request for this CPU.  This function is called from
 *   the IDLE loop of a secondary CPU, i.e. outside of any interrupt handler
 *   and without holding the critical section.  When a request is accepted,
 *   the CPU does not return from here: it is restarted from its IDLE entry
 *   point by sched_cpuon().
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void nxsched_process_cpuoff(void)
{
	int cpu = this_cpu();
	irqstate_t flags;
	int32_t expected;
	int ret;

	if (atomic_read_acquire(&g_cpuoff_req[cpu]) != CPUOFF_REQUESTED) {
		return;
	}

	flags = up_irq_save();

	expected = CPUOFF_REQUESTED;
	if (atomic_cmpxchg(&g_cpuoff_req[cpu], &expected, CPUOFF_ACCEPTED)) {
		/* Does not return on success */

		ret = up_cpu_die();

		smplldbg("CPU%d failed to enter CPU_OFF: %d\n", cpu, ret);
		UNUSED(ret);
		atomic_set_release(&g_cpuoff_req[cpu], CPUOFF_FAILED);
	}

	up_irq_restore(flags);
}

/****************************************************************************
 * Name: nxsched_cpuoff_reset
 *
 * Description:
 *   Clear the OFF request state of a CPU which is being brought online.
 *
 * Input Parameters:
 *   cpu - The CPU index.
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void nxsched_cpuoff_reset(int cpu)
{
	atomic_set_release(&g_cpuoff_req[cpu], CPUOFF_NONE);
}

/****************************************************************************
 * Name: sched_cpuoff
 *
 * Description:
 *   Request to bring a CPU to power off state.
 *
 * Input Parameters:
 *   cpu - The CPU index to turn off.
 *
 * Returned Value:
 *   OK (0) on success, Negative value on failure.
 *   -EBUSY means that the target CPU is not idle now; the caller may retry.
 *
 * Preconditions(Checks):
 *   1. CPU cannot turn itself OFF.
 *   2. CPU number must be valid (1 ... CONFIG_SMP_NCPUS).
 *   3. CPU 0 cannot be turned OFF.
 *   4. The target CPU must be running its IDLE task.
 *
 * Assumptions:
 *   May be called inside a critical section.  The target CPU never needs
 *   the critical section to complete the transition.
 *
 ****************************************************************************/

int sched_cpuoff(int cpu)
{
	irqstate_t flags;
	int32_t expected;
	int ret;

	if (cpu == this_cpu()) {
		smplldbg("Cannot turn off CPU from itself: %d\n", cpu);
		return -EINVAL;
	}

	/* Validate CPU index */

	if (cpu <= 0 || cpu >= CONFIG_SMP_NCPUS) {
		smplldbg("Invalid CPU number: %d\n", cpu);
		return -EINVAL;
	}

	if (!spin_trylock(&g_state_transition_lock)) {
		return -EBUSY;
	}

	flags = enter_critical_section();

	if (!CPU_ISSET(cpu, &g_active_cpus_mask)) {
		smplldbg("Already desired state (OFF), operation not supported\n");
		ret = -ENOTSUP;
		goto errout_with_lock;
	}

	if (!__check_cpuoff_ready(cpu)) {
		ret = -EBUSY;
		goto errout_with_lock;
	}

	/* No task may be assigned to the CPU from now on.  The critical section
	 * keeps the scheduler from observing the intermediate state.
	 */

	CPU_CLR(cpu, &g_active_cpus_mask);
	atomic_set_release(&g_cpuoff_req[cpu], CPUOFF_REQUESTED);

	/* Wake the target so that it reaches its IDLE loop and leaves through
	 * up_cpu_die(), then wait until it is off and cut its power.
	 */

	ret = up_cpu_stop(cpu);
	if (ret < 0) {
		/* The target may be busy-waiting for the critical section that this
		 * CPU holds, so it could not reach its IDLE loop.  Withdraw the
		 * request unless the target has taken it in the meantime.
		 */

		expected = CPUOFF_REQUESTED;
		if (atomic_cmpxchg(&g_cpuoff_req[cpu], &expected, CPUOFF_NONE)) {
			ret = -EBUSY;
			goto errout_with_mask;
		}

		if (atomic_read_acquire(&g_cpuoff_req[cpu]) == CPUOFF_FAILED) {
			/* The target returned to its IDLE loop and is still running */

			atomic_set_release(&g_cpuoff_req[cpu], CPUOFF_NONE);
			goto errout_with_mask;
		}

		/* The target has left the scheduler but its power state is unknown.
		 * Keep it offline; sched_cpuon() boots it again from scratch.
		 */

		smplldbg("CPU%d power state unknown after CPU_OFF: %d\n", cpu, ret);
		goto errout_with_lock;
	}

	smpllvdbg("CPU%d is turned off successfully\n", cpu);
	leave_critical_section(flags);
	spin_unlock(&g_state_transition_lock);
	return OK;

errout_with_mask:
	CPU_SET(cpu, &g_active_cpus_mask);

errout_with_lock:
	leave_critical_section(flags);
	spin_unlock(&g_state_transition_lock);
	return ret;
}
