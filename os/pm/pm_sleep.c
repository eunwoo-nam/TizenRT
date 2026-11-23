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
/************************************************************************
 * pm/pm_sleep.c
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ************************************************************************/

/************************************************************************
 * Included Files
 ************************************************************************/

#include <tinyara/config.h>
#include <tinyara/pm/pm.h>
#include <tinyara/irq.h>
#include <tinyara/wdog.h>
#include <tinyara/clock.h>
#include <tinyara/sched.h>
#include <tinyara/semaphore.h>
#include <errno.h>
#include <debug.h>

#include "pm.h"

/************************************************************************
 * Public Functions
 ************************************************************************/

/************************************************************************
 * Name: pm_sleep_timeout
 *
 * Description:
 *   Expiry function of the pm_sleep() watchdog.  It also identifies the
 *   watchdog as a wake-up source: pm_idle() looks up the earliest active
 *   watchdog with this expiry function to program the wake-up timer.
 *
 ************************************************************************/

void pm_sleep_timeout(wdparm_t arg)
{
	(void)nxsem_post((FAR sem_t *)arg);
}

/************************************************************************
 * Name: pm_sleep
 *
 * Description:
 *   This function allows the board to sleep for given time interval.
 *   When this function is called, it is expected that board will sleep for
 *   given duration of time. But for some cases board might not go
 *   to sleep instantly if :
 *      1. system is in pm lock (pm state transition is locked)
 *      2. Other threads(other than idle) are running
 *      3. NORMAL to SLEEP state threshold time is large
 *
 * Parameters:
 *   milliseconds - expected board sleep duration
 *
 * Return Value:
 *   OK - success
 *   ERROR - error
 *
 ************************************************************************/

int pm_sleep(int milliseconds)
{
	FAR struct tcb_s *rtcb;
	irqstate_t flags;
	sclock_t delay;
	sem_t pm_sem;
	int ret;

	if (milliseconds < 0) {
		set_errno(EINVAL);
		return ERROR;
	}

	if (milliseconds == 0) {
		return OK;
	}

	delay = MSEC2TICK(milliseconds);
	if (delay <= 0) {
		delay = 1;
	}

	/* The semaphore only signals the expiry of the wake-up timer */

	nxsem_init(&pm_sem, 0, 0);

	/* Use the timed-wait watchdog of the calling thread, as the kernel timed
	 * waits do.  The thread is blocked on pm_sem while the watchdog runs, so
	 * no other timed wait can use it, and nxtask_recover() cancels it if the
	 * thread is deleted while it sleeps.
	 *
	 * TODO - Since PM & Kernel are separate, we should not use tcb inside pm.
	 */

	rtcb = nxsched_self();

	flags = enter_critical_section();

	ret = wd_start(&rtcb->waitdog, delay, pm_sleep_timeout, (wdparm_t)&pm_sem);
	if (ret < 0) {
		pmdbg("pm_sleep: wd_start failed: %d\n", ret);
		goto errout;
	}

	/* Wait until the timer expires */

	ret = nxsem_wait_uninterruptible(&pm_sem);

	/* Stop the timer if the wait ended for another reason */

	wd_cancel(&rtcb->waitdog);

errout:
	leave_critical_section(flags);
	nxsem_destroy(&pm_sem);

	if (ret < 0) {
		set_errno(-ret);
		return ERROR;
	}

	return OK;
}
