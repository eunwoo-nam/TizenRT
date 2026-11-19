/*
 * Copyright (c) 2020 Realtek Semiconductor Corp.	All rights reserved.
 *
 * Author: PSP Software Group
 */

#include <tinyara/config.h>

#include <assert.h>
#include <debug.h>

#include <tinyara/arch.h>

#include "sheipa.h"
#include "ameba_soc.h"
#include "psci.h"
#include "smp.h"
#include "amebasmart_smp.h"
#include <arch/barriers.h>
#include <errno.h>

#define SECONDARY_CORE_ID              1
#define CPU_PSCI_POLL_COUNT            200
#define CPU_PSCI_POLL_INTERVAL_USEC    50

#ifndef CONFIG_PLATFORM_TIZENRT_OS
#include "task.h"
#endif

#ifdef CONFIG_PLATFORM_TIZENRT_OS
extern void __cpu1_start(void);
#else
extern void _boot(void);
extern void vPortRestoreTaskContext(void);
#endif


/* from psci.h aff_info_state_t */
typedef enum {
	AFF_STATE_ON = 0,
	AFF_STATE_OFF = 1,
	AFF_STATE_ON_PENDING = 2
} aff_info_state_t;


/*-----------------------------------------------------------*/

void rtk_core1_power_on(void)
{
	u32 val;
	CA32_TypeDef *ca32 = CA32_BASE;

	ca32->CA32_C0_RST_CTRL &= (~(CA32_NCOREPORESET(0x2) | CA32_NCORERESET(0x2)));

	val =  HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO);
	val |= (HSYS_ISO_HP_AP_CORE(0x2));
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO, val);
	DelayUs(50);

	val =  HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC);
	val |= (HSYS_PSW_HP_AP_CORE(0x3));
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC, val);
	DelayUs(50);

	val =  HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC);
	val |= (HSYS_PSW_HP_AP_CORE_2ND(0x3));
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC, val);
	DelayUs(500);

	val =  HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO);
	val &= ~(HSYS_ISO_HP_AP_CORE(0x3));
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO, val);
	DelayUs(50);

	ca32->CA32_C0_RST_CTRL |= (CA32_NCOREPORESET(0x2) | CA32_NCORERESET(0x2) | CA32_BIT_NRESETSOCDBG | CA32_BIT_NL2RESET | CA32_BIT_NGICRESET);

}


void rtk_core1_power_off(void)
{
	u32 val;

	val =  HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO);
	val |= (HSYS_ISO_HP_AP_CORE(0x2));
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO, val);

	val =  HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC);
	val &= ~(HSYS_PSW_HP_AP_CORE_2ND(0x2));
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC, val);

	val =  HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC);
	val &= ~(HSYS_PSW_HP_AP_CORE(0x2));
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC, val);
}

/****************************************************************************
 * Name: cpu_core_poweron
 *
 * Description:
 *   Powers on the CPU domain and initializes hardware for the secondary CPU.
 *   This function handles the low-level power management and hardware
 *   initialization of the cpu.
 *
 * Input Parameters:
 *   cpu - The CPU ID to power on
 *
 * Returned Value:
 *   OK (0) on success, negated error no on failure.  -EBUSY means that PSCI
 *   does not report the CPU off; its power is then left untouched.
 *
 * Assumptions:
 *   - Called from CPU0
 *   - Target CPU is currently powered off
 *
 ****************************************************************************/

int cpu_core_poweron(int cpu)
{
	int state;

	/* Currently only supports CPU1 */
	if (cpu != 1) {
		smplldbg("%s only supports CPU1, got CPU%d\n", __func__, cpu);
		return -ENOTSUP;
	}

	/* If psci_cpu_on is called more than once, the CPU may not function
	 * properly.  Start only a CPU that PSCI reports off; the target enters the
	 * scheduler on its own once it is booted, so a CPU that cannot be booted
	 * must be reported as a failure.
	 */
	state = psci_affinity_info(cpu, 0);
	if (state != AFF_STATE_OFF) {
		smplldbg("CPU%d is not off, affinfo: %d\n", cpu, state);
		return -EBUSY;
	}

	/* turn on CPU1's power domain, require time to stabilize */
	rtk_core1_power_on();
	DelayUs(50);

	/* The Cortex-A32 has no SCU to re-enable.  The old arm_enable_smp(0)
	 * call here only invalidated the L1 data cache of the running CPU0,
	 * which can drop dirty lines; CPU0 takes part in the coherency since
	 * amebasmart_enable_smp(0) at boot.
	 */

	return OK;
}

/****************************************************************************
 * Name: vPortSecondaryBoot
 *
 * Description:
 *   Starts the secondary CPU execution via PSCI and waits for it to come
 *   online. This function handles the complete boot sequence from powered-off
 *   state to CPU online, including PSCI boot and affinity validation.
 *
 * Input Parameters:
 *   cpu - The CPU ID to start
 *
 * Returned Value:
 *   OK (0) on success, negated error no on failure.  On failure the power
 *   of the core is turned off again.
 *
 * Assumptions:
 *   - Called from CPU0
 *   - Target CPU core has been powered on by cpu_core_poweron()
 *
 ****************************************************************************/

int vPortSecondaryBoot(int cpu)
{
	BaseType_t err;
	int state;
	int count;
	int ret;

	err = psci_cpu_on(cpu, (unsigned long)__cpu1_start);
	if (err < 0) {
		smplldbg("PSCI CPU_ON failed for CPU%d: %d\n", cpu, (int)err);
		ret = -EIO;
		goto errout_poweroff;
	}

	/* await for PSCI to report cpu state */
	for (count = 0; count < CPU_PSCI_POLL_COUNT; count++) {
		state = psci_affinity_info(cpu, 0);
		if (state == AFF_STATE_ON) {
			return OK;
		}

		DelayUs(CPU_PSCI_POLL_INTERVAL_USEC);
	}

	smplldbg("Secondary core boot timeout affinfo: %d\n", state);
	ret = -ETIMEDOUT;

errout_poweroff:
	/* The target is not handed to the scheduler.  Do not leave a partially
	 * booted core powered while the kernel records it as offline.
	 */
	rtk_core1_power_off();
	return ret;
}

/* CPU hotplug operations.  The target calls up_cpu_die() from its IDLE
 * loop after it has accepted the OFF request (see sched_cpuoff()), and
 * up_cpu_stop() runs on the requester to wake the target, wait for its
 * PSCI CPU_OFF and cut its power.
 */

#ifdef CONFIG_CPU_HOTPLUG
/****************************************************************************
 * Name: cpu_core_powerdown
 *
 * Description:
 *   Powers down the specified CPU core after ensuring it's in a safe state.
 *   This function handles the core power down operations including PSCI
 *   coordination and RTK power off.
 *
 * Input Parameters:
 *   cpu - The CPU ID to power down
 *
 * Returned Value:
 *   OK (0) on success, negated error no on failure.  -ETIMEDOUT means that
 *   PSCI did not report the CPU off in time; its power is left on.
 *
 * Assumptions:
 *   - CPU has already been hot-plugged
 *   - Called from CPU0
 *
 ****************************************************************************/

int cpu_core_powerdown(int cpu)
{
	int count;
	int state;

	/* Currently only supports CPU1 */
	if (cpu != 1) {
		smplldbg("%s only supports CPU1, got CPU%d\n", __func__, cpu);
		return -ENOTSUP;
	}

	/* wait for PSCI to report that CPU1 is now safe to power off */
	for (count = 0; count < CPU_PSCI_POLL_COUNT; count++) {
		state = psci_affinity_info(cpu, 0);
		if (state == AFF_STATE_OFF) {
			/* turn off CPU1's power domain */
			rtk_core1_power_off();
			return OK;
		}

		DelayUs(CPU_PSCI_POLL_INTERVAL_USEC);
	}

	/* Not an error by itself: a target that has not taken the OFF request
	 * yet makes sched_cpuoff() withdraw it.  sched_cpuoff() reports the
	 * cases that are errors.
	 */

	smpllvdbg("Core powerdown timeout for CPU%d, affinfo: %d\n", cpu, state);
	return -ETIMEDOUT;
}

/****************************************************************************
 * Name: up_cpu_die
 *
 * Description:
 *   Shut down the current CPU core using PSCI. This function is called by a
 *   CPU that needs to power itself down as part of the hot-plug sequence.
 *   It invokes the PSCI CPU_OFF service which puts the calling CPU into
 *   a low-power state and prevents it from being scheduled for execution.
 *
 *   This is a terminal function that does not return. The CPU will remain
 *   in the powered-down state until it is explicitly powered back on by
 *   another CPU via PSCI_CPU_ON.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   Does not return on success; a negated errno value on failure.
 *
 * Assumptions:
 *   - Called from the IDLE loop of the target CPU with interrupts disabled
 *   - The CPU holds no kernel lock and has been removed from the scheduler
 *   - The CPU restarts from its IDLE entry point when it is powered on
 *
 ****************************************************************************/

int up_cpu_die(void)
{
	/* PSCI CPU_OFF does not return on success.  Propagate firmware errors so
	 * that the scheduler can keep this CPU online.
	 */

	return psci_cpu_off();
}

/****************************************************************************
 * Name: up_cpu_stop
 *
 * Description:
 *   Stop a CPU which has a pending OFF request and cut its power.  SEV
 *   wakes the target from the WFE of its up_idle() (or makes its next WFE
 *   return at once), so that it reaches its IDLE loop, accepts the request
 *   and calls up_cpu_die().  A SEV lost on the CA32 cluster is covered by
 *   the generic timer event stream (amebasmart_enable_evtstrm()).  This
 *   function then waits until PSCI reports the target off and turns its
 *   power domain off.
 *
 *   It takes no kernel lock, so the target makes progress while the caller
 *   holds the critical section.
 *
 * Input Parameters:
 *   cpu - The index of the CPU to stop (1 .. CONFIG_SMP_NCPUS-1)
 *
 * Returned Value:
 *   Zero (OK) on success; -ETIMEDOUT if the target was not off in time, or
 *   another negated errno value on failure.
 *
 * Assumptions:
 *   - Called from another CPU, after sched_cpuoff() published the request
 *
 ****************************************************************************/

int up_cpu_stop(int cpu)
{
	if (cpu <= 0 || cpu >= CONFIG_SMP_NCPUS || cpu == up_cpu_index()) {
		smplldbg("Invalid CPU index: %d\n", cpu);
		return -EINVAL;
	}

	/* Make the published request visible before the target wakes up */

	UP_DSB();
	UP_SEV();

	/* Wait for PSCI CPU_OFF of the target and cut its power */

	return cpu_core_powerdown(cpu);
}

#endif /* CONFIG_CPU_HOTPLUG */
