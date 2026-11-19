/****************************************************************************
 * arch/arm/src/amebasmart/amebasmart_cpuboot.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <tinyara/config.h>

#include <stdint.h>
#include <assert.h>
#include <debug.h>

#include <tinyara/arch.h>
#include <tinyara/sched.h>
#ifdef CONFIG_SCHED_INSTRUMENTATION
#  include <tinyara/sched_note.h>
#endif
#include <arch/irq.h>

#include "init/init.h"
#include "sched/sched.h"

#include "arm_internal.h"
#include "sctlr.h"
#include "smp.h"
#include "scu.h"
#include "gic.h"
#include "amebasmart_enablesmp.h"
#include "amebasmart_smp.h"
#include "mmu.h"

#ifdef CONFIG_SMP

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: arm_cpu_boot
 *
 * Description:
 *   Continues the C-level initialization started by the assembly language
 *   __cpu[n]_start function (NuttX armv7-a arm_cpuhead.S).  CPU0 brings
 *   CPUn up with up_cpu_start() (power-on and PSCI CPU_ON at __cpu1_start),
 *   reached through sched_cpuon() both at boot and when a CPU is turned on
 *   again.  Like the NuttX qemu chip, this function enables the cache
 *   coherency, the FPU and the GIC CPU interface of CPUn, then enters the
 *   IDLE task of CPUn with nx_idle_trampoline().  The system tick stays on
 *   CPU0.
 *
 * Input Parameters:
 *   cpu - The CPU index.  This is the same value that would be obtained by
 *      calling up_cpu_index();
 *
 * Returned Value:
 *   Does not return.
 *
 ****************************************************************************/

void arm_cpu_boot(int cpu)
{
  /* Enable SMP cache coherency for the CPU (no SCU on the Cortex-A32) */

  amebasmart_enable_smp(cpu);

  /* Initialize the FPU */

  arm_fpuconfig();

  /* Initialize the Generic Interrupt Controller (GIC) for CPUn (n != 0),
   * set VBAR and enable the interrupts.
   */

  up_irqinitialize();

  /* Then transfer control to the IDLE task */

  nx_idle_trampoline();
}

/****************************************************************************
 * Name: up_cpu_start
 *
 * Description:
 *   Power on CPUn and boot it through PSCI CPU_ON at __cpu1_start.  The
 *   PSCI request carries the entry point and arm_cpu_boot() enters the IDLE
 *   task with nx_idle_trampoline(), so no CPUSTART SGI follows.
 *   sched_cpuon() calls this function both at boot and when a CPU that
 *   sched_cpuoff() turned off is turned on again.
 *
 *   This board implementation replaces the weak one of armv7-a
 *   (arm_cpustart.c).  It is kept in this file, which every SMP link pulls
 *   in through the arm_cpu_boot() reference of arm_cpuhead.S: in an archive
 *   member that nothing else references, the weak default would satisfy
 *   the reference and this function would be silently left out.
 *
 * Input Parameters:
 *   cpu - The index of the CPU being started (1 .. CONFIG_SMP_NCPUS-1)
 *
 * Returned Value:
 *   Zero on success; a negated errno value on failure.
 *
 * Assumptions:
 *   - Called from another CPU, inside the critical section of sched_cpuon()
 *   - The target CPU is powered off
 *
 ****************************************************************************/

int up_cpu_start(int cpu)
{
	int ret;

	DEBUGASSERT(cpu > 0 && cpu < CONFIG_SMP_NCPUS && cpu != this_cpu());

#ifdef CONFIG_SCHED_INSTRUMENTATION
	/* Notify of the start event */

	sched_note_cpu_start(this_task(), cpu);
#endif

	/* Power the core on, then boot it at __cpu1_start through PSCI */

	ret = cpu_core_poweron(cpu);
	if (ret != OK) {
		smplldbg("Failed to power on CPU%d: %d\n", cpu, ret);
		return ret;
	}

	ret = vPortSecondaryBoot(cpu);
	if (ret != OK) {
		smplldbg("Failed to boot CPU%d: %d\n", cpu, ret);
	}

	return ret;
}

#endif /* CONFIG_SMP */
