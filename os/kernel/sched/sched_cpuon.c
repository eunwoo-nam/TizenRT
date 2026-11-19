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
 * kernel/sched/sched_cpuon.c
 *
 * Description:
 *   This file implements the CPU control's power on function which starts
 *   the secondary CPUs at boot (nx_smp_start()) and turns on again a CPU
 *   that was turned off, e.g. from the PM idle logic.
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
#include <tinyara/spinlock.h>
#include <tinyara/irq.h>
#include <tinyara/queue.h>
#include <tinyara/sched.h>

#include "sched/sched.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* Spinlock for synchronizing access to global hotplug state */

volatile spinlock_t g_state_transition_lock = SP_UNLOCKED;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

#ifdef CONFIG_CPU_HOTPLUG
/****************************************************************************
 * Name: __reset_idle_task
 *
 * Description:
 *   Prepare the IDLE task of an offline CPU to restart from its entry point.
 *   A CPU leaves the scheduler from its IDLE loop and never resumes that
 *   context: up_cpu_start() boots it again through arm_cpu_boot() into
 *   nx_idle_trampoline(), exactly as at cold boot.  At cold boot the IDLE
 *   task is already in that state.
 *
 * Input Parameters:
 *   cpu - The index of the offline CPU.
 *
 * Returned Value:
 *   None
 *
 * Assumptions:
 *   Called inside the critical section while the CPU is offline.
 *
 ****************************************************************************/

static void __reset_idle_task(int cpu)
{
	FAR struct tcb_s *tcb = current_task(cpu);

	DEBUGASSERT(is_idle_task(tcb));

	/* nx_idle_trampoline() starts with the scheduler locked once */

	tcb->lockcount = 1;
	up_initial_state(tcb);

	g_delivertasks[cpu] = SWITCH_NONE;
	nxsched_cpuoff_reset(cpu);
}
#endif /* CONFIG_CPU_HOTPLUG */

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: sched_cpuon
 *
 * Description:
 *   Request to bring a CPU to power on state.
 *
 * Input Parameters:
 *   cpu - The CPU index to turn on
 *
 * Returned Value:
 *   OK (0) on success, negative value on failure.
 *
 * Preconditions(Checks):
 *   1. CPU cannot turn itself ON.
 *   2. CPU number must be valid (1 ... CONFIG_SMP_NCPUS).
 *   3. CPU 0 cannot be turned ON.
 *
 ****************************************************************************/

int sched_cpuon(int cpu)
{
	irqstate_t flags;
	int ret;

	if (cpu == this_cpu()) {
		smplldbg("Cannot turn on this CPU from itself: %d\n", cpu);
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

	/* Lock the scheduler and disable interrupts to ensure atomicity
	 * throughout this critical operation.
	 */

	flags = enter_critical_section();

	if (CPU_ISSET(cpu, &g_active_cpus_mask)) {
		smplldbg("Already desired state (ON), operation not supported\n");
		ret = -ENOTSUP;
		goto errout_with_lock;
	}

#ifdef CONFIG_CPU_HOTPLUG
	__reset_idle_task(cpu);
#endif

	/* Start the CPU.  The architecture powers the core on as needed and
	 * the CPU enters its IDLE task on its own.
	 */

	ret = up_cpu_start(cpu);
	if (ret == OK) {
		smpllvdbg("CPU%d is turned on successfully\n", cpu);

		/* Update the active CPUs mask to indicate that the offline CPU is
		 * active again
		 */

		CPU_SET(cpu, &g_active_cpus_mask);
	} else {
		smplldbg("Failed to turn on CPU%d: %d\n", cpu, ret);
	}

errout_with_lock:
	leave_critical_section(flags);
	spin_unlock(&g_state_transition_lock);
	return ret;
}
