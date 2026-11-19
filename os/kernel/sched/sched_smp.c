/****************************************************************************
 *
 * Copyright 2026 Samsung Electronics All Rights Reserved.
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
 * sched/sched/sched_smp.c
 *
 * SPDX-License-Identifier: Apache-2.0
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

#include <assert.h>
#include <tinyara/arch.h>
#include <tinyara/nuttx.h>
#include <tinyara/queue.h>
#include <tinyara/semaphore.h>
#include <tinyara/sched.h>
#include <tinyara/spinlock.h>

#include "sched/sched.h"

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct smp_call_cookie_s {
	sem_t       sem;
	int         error;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

static sq_queue_t g_smp_call_queue[CONFIG_SMP_NCPUS];
static spinlock_t g_smp_call_lock;

/****************************************************************************
 * Public Data
 ****************************************************************************/

/* CPUs on which the scheduler may run tasks.  nx_smp_start() marks CPU0,
 * sched_cpuon() adds a CPU as it brings it up and sched_cpuoff() removes it
 * before the CPU leaves the scheduler.  sched_getactivecpu() returns the
 * mask to the PM code.
 */

cpu_set_t g_active_cpus_mask = 1;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: nxsched_smp_call_add
 *
 * Description:
 *   Add call data to other processors
 *
 * Input Parameters:
 *   cpu        - Target cpu id
 *   data  - Call data
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

static void nxsched_smp_call_add(int cpu,
                                 FAR struct smp_call_data_s *data)
{
	irqstate_t flags;

	flags = spin_lock_irqsave(&g_smp_call_lock);
	if (!sq_inqueue(&data->node[cpu], &g_smp_call_queue[cpu])) {
		sq_addlast(&data->node[cpu], &g_smp_call_queue[cpu]);
	}

	spin_unlock_irqrestore(&g_smp_call_lock, flags);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: nxsched_smp_call_handler
 *
 * Description:
 *   SMP function call handler
 *
 * Input Parameters:
 *   irq     - Interrupt id
 *   context - Regs context before irq
 *   arg     - Interrupt arg
 *
 * Returned Value:
 *   Result
 *
 ****************************************************************************/

int nxsched_smp_call_handler(int irq, FAR void *context,
                             FAR void *arg)
{
	FAR sq_queue_t *call_queue;
	FAR sq_entry_t *curr;
	FAR sq_entry_t *next;
	int cpu = this_cpu();

	irqstate_t flags = spin_lock_irqsave(&g_smp_call_lock);

	call_queue = &g_smp_call_queue[cpu];

	sq_for_every_safe(call_queue, curr, next) {
		FAR struct smp_call_data_s *data =
		  container_of(curr, struct smp_call_data_s, node[cpu]);
		int ret;

		sq_rem(&data->node[cpu], call_queue);

		spin_unlock_irqrestore(&g_smp_call_lock, flags);

		ret = data->func(data->arg);

		flags = spin_lock_irqsave(&g_smp_call_lock);

		if (data->cookie != NULL) {
			if (ret < 0) {
				data->cookie->error = ret;
			}

			nxsem_post(&data->cookie->sem);
		}
	}

	spin_unlock_irqrestore(&g_smp_call_lock, flags);
	return OK;
}

/****************************************************************************
 * Name: nxsched_smp_call_pending
 *
 * Description:
 *   Check whether SMP calls are queued for a CPU.
 *
 * Input Parameters:
 *   cpu - Target cpu id
 *
 * Returned Value:
 *   true if the CPU has a call to process.
 *
 ****************************************************************************/

bool nxsched_smp_call_pending(int cpu)
{
	irqstate_t flags;
	bool pending;

	flags = spin_lock_irqsave(&g_smp_call_lock);
	pending = !sq_empty(&g_smp_call_queue[cpu]);
	spin_unlock_irqrestore(&g_smp_call_lock, flags);

	return pending;
}

/****************************************************************************
 * Name: nxsched_smp_call_init
 *
 * Description:
 *   Init call_data
 *
 * Input Parameters:
 *   data - Call data
 *   func - Function
 *   arg  - Function args
 *
 * Returned Value:
 *   Result
 *
 ****************************************************************************/

void nxsched_smp_call_init(FAR struct smp_call_data_s *data,
                           nxsched_smp_call_t func, FAR void *arg)
{
	DEBUGASSERT(data != NULL && func != NULL);

	memset(data, 0, sizeof(struct smp_call_data_s));
	data->func = func;
	data->arg = arg;
}

/****************************************************************************
 * Name: nxsched_smp_call_single
 *
 * Description:
 *   Call function on single processor, wait function callback
 *
 * Input Parameters:
 *   cpuid - Target cpu id
 *   func  - Function
 *   arg   - Function args
 *
 * Returned Value:
 *   Result
 *
 ****************************************************************************/

int nxsched_smp_call_single(int cpuid, nxsched_smp_call_t func,
                            FAR void *arg)
{
	cpu_set_t cpuset;

	CPU_ZERO(&cpuset);
	CPU_SET(cpuid, &cpuset);
	return nxsched_smp_call(cpuset, func, arg);
}

/****************************************************************************
 * Name: nxsched_smp_call
 *
 * Description:
 *   Call function on multi processors, wait function callback
 *
 * Input Parameters:
 *   cpuset - Target cpuset
 *   func   - Function
 *   arg    - Function args
 *
 * Returned Value:
 *   Result
 *
 ****************************************************************************/

int nxsched_smp_call(cpu_set_t cpuset, nxsched_smp_call_t func,
                     FAR void *arg)
{
	struct smp_call_data_s data;
	struct smp_call_cookie_s cookie;
	int cpucnt;
	int ret = OK;
	int i;

	/* Cannot wait in interrupt context. */

	DEBUGASSERT(!up_interrupt_context());

#ifdef CONFIG_CPU_HOTPLUG
	/* An offline CPU never answers; do not wait for it */

	cpuset &= g_active_cpus_mask;
#endif

	nxsched_smp_call_init(&data, func, arg);
	cookie.error = 0;
	nxsem_init(&cookie.sem, 0, 0);

	data.cookie = &cookie;
	ret = nxsched_smp_call_async(cpuset, &data);

	if (ret < 0) {
		nxsem_destroy(&cookie.sem);
		return ret;
	}

	cpucnt = CPU_COUNT(&cpuset);
	for (i = 0; i < cpucnt; i++) {
		int rc = nxsem_wait_uninterruptible(&cookie.sem);
		if (rc < 0) {
			ret = rc;
		}
	}

	if (cookie.error < 0) {
		ret = cookie.error;
	}

	nxsem_destroy(&cookie.sem);

	return ret;
}

/****************************************************************************
 * Name: nxsched_smp_call_single_async
 *
 * Description:
 *   Call function on single processor async
 *
 * Input Parameters:
 *   cpuset - Target cpuset
 *   data   - Call data
 *
 * Returned Value:
 *   Result
 *
 ****************************************************************************/

int nxsched_smp_call_single_async(int cpuid,
                                  FAR struct smp_call_data_s *data)
{
	cpu_set_t cpuset;

	CPU_ZERO(&cpuset);
	CPU_SET(cpuid, &cpuset);
	return nxsched_smp_call_async(cpuset, data);
}

/****************************************************************************
 * Name: nxsched_smp_call_async
 *
 * Description:
 *   Call function on multi processors async
 *
 * Input Parameters:
 *   cpuset - Target cpuset
 *   data   - Call data
 *
 * Returned Value:
 *   Result
 *
 ****************************************************************************/

int nxsched_smp_call_async(cpu_set_t cpuset,
                           FAR struct smp_call_data_s *data)
{
	int cpucnt;
	int ret = OK;
	int i;

#ifdef CONFIG_CPU_HOTPLUG
	/* An offline CPU cannot run the call.  It is not queued, so that the
	 * CPU does not find a stale request when it is restarted.
	 */

	cpuset &= g_active_cpus_mask;
#endif

	/* Prevent reschedule on another processor */

	if (!up_interrupt_context()) {
		sched_lock();
	}

	if (CPU_ISSET(this_cpu(), &cpuset)) {
		ret = data->func(data->arg);
		if (data->cookie != NULL) {
			data->cookie->error = ret;
			nxsem_post(&data->cookie->sem);
			if (ret < 0) {
				goto out;
			}
		}

		CPU_CLR(this_cpu(), &cpuset);
	}

	cpucnt = CPU_COUNT(&cpuset);
	if (cpucnt == 0) {
		goto out;
	}

	for (i = 0; i < CONFIG_SMP_NCPUS; i++) {
		if (CPU_ISSET(i, &cpuset)) {
			nxsched_smp_call_add(i, data);
			if (--cpucnt == 0) {
				break;
			}
		}
	}

	up_send_smp_call(cpuset);

out:
	if (!up_interrupt_context()) {
		sched_unlock();
	}

	return ret;
}
