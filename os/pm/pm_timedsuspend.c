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
 * pm/pm_timedsuspend.c
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
#include <errno.h>
#include <debug.h>

#include "pm.h"

/************************************************************************
 * Private Functions
 ************************************************************************/

/************************************************************************
 * Name: __pm_timedsuspend_timeout
 *
 * Description:
 *   Expiry function of the timed suspend watchdog of a domain.  It runs
 *   from the timer interrupt inside the critical section, so the domain
 *   cannot be unregistered concurrently: pm_domain_unregister() cancels
 *   the watchdog inside the critical section before it frees the domain.
 *
 ************************************************************************/

static void __pm_timedsuspend_timeout(wdparm_t arg)
{
	FAR struct pm_domain_s *domain = (FAR struct pm_domain_s *)arg;

	if (pm_resume(domain) != OK) {
		pmlldbg("Unable to resume domain: %s\n", domain->name);
	}
}

/************************************************************************
 * Public Functions
 ************************************************************************/

/************************************************************************
 * Name: pm_timedsuspend
 *
 * Description:
 *   This function locks PM state transition for a specific duration.
 *   This function can be called from IRQ context.
 *
 *   The domain holds one suspend reference while its timed suspend
 *   watchdog is active.  A new request only extends the remaining time,
 *   and a zero duration cancels the timed suspend immediately.
 *
 * Parameters:
 *   domain      - Pointer to the domain structure
 *   milliseconds - expected lock duration in millisecond
 *
 * Return Value:
 *   OK (0) - success
 *   ERROR (-1) - error
 *
 ************************************************************************/

int pm_timedsuspend(FAR struct pm_domain_s *domain, unsigned int milliseconds)
{
	irqstate_t flags;
	sclock_t delay;
	int ret = OK;

	if (domain == NULL) {
		set_errno(EINVAL);
		return ERROR;
	}

	flags = enter_critical_section();

	if (milliseconds == 0) {
		/* Cancel the timed suspend and drop its suspend reference */

		if (wd_cancel(&domain->wdog) == OK) {
			__pm_timedsuspend_timeout((wdparm_t)domain);
		}

		goto exit;
	}

	/* Round up to one tick so that the request is not taken as a cancel */

	delay = MSEC2TICK(milliseconds);
	if (delay <= 0) {
		delay = 1;
	}

	if (!WDOG_ISACTIVE(&domain->wdog)) {
		/* Take the suspend reference of the timed suspend */

		if (pm_suspend(domain) != OK) {
			ret = ERROR;
			goto exit;
		}
	} else if (delay <= wd_gettime(&domain->wdog)) {
		/* The running timed suspend already lasts longer */

		goto exit;
	}

	ret = wd_start(&domain->wdog, delay, __pm_timedsuspend_timeout, (wdparm_t)domain);
	if (ret < 0) {
		if (!WDOG_ISACTIVE(&domain->wdog)) {
			(void)pm_resume(domain);
		}

		set_errno(-ret);
		ret = ERROR;
	}

exit:
	leave_critical_section(flags);
	return ret;
}
