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

#include <tinyara/clock.h>
#include <tinyara/irq.h>
#include <tinyara/list.h>
#include <tinyara/wdog.h>

#include "wdog/wdog.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: wd_getwakeupdelay
 *
 * Description:
 *   Return the delay until the earliest active watchdog that expires into
 *   the given function.  The power management uses a dedicated expiry
 *   function for its wake-up timers, so the active list identifies them
 *   without any extra state: a watchdog which is cancelled, expires or is
 *   restarted with another function is no longer a wake-up source.
 *
 * Input Parameters:
 *   wakeup - The expiry function that marks a wake-up watchdog.
 *
 * Returned Value:
 *   The number of ticks until the wake-up watchdog expires; zero if no
 *   such watchdog is active.  A watchdog which is already due but not yet
 *   processed reports one tick, so that it is never mistaken for "no
 *   wake-up timer".
 *
 ****************************************************************************/

clock_t wd_getwakeupdelay(wdentry_t wakeup)
{
	FAR struct wdog_s *curr;
	irqstate_t flags;
	sclock_t delay;
	clock_t ret = 0;

	if (wakeup == NULL) {
		return 0;
	}

	flags = enter_critical_section();

	/* g_wdactivelist is sorted by expiry, so the first match is the earliest */

	list_for_every_entry(&g_wdactivelist, curr, struct wdog_s, node) {
		if (curr->func == wakeup) {
			delay = (sclock_t)(curr->expired - clock_systime_ticks());
			ret = delay > 0 ? (clock_t)delay : 1;
			break;
		}
	}

	leave_critical_section(flags);
	return ret;
}
