/*
  Part of: CCEvents
  Contents: tests for timer events source
  Date: Mon Feb  6, 2017

  Abstract



  Copyright (C) 2017 Marco Maggi <marco.maggi-ipsu@poste.it>

  This is free software; you  can redistribute it and/or modify it under
  the terms of the GNU Lesser General Public License as published by the
  Free Software  Foundation; either version  3.0 of the License,  or (at
  your option) any later version.

  This library  is distributed in the  hope that it will  be useful, but
  WITHOUT   ANY  WARRANTY;   without  even   the  implied   warranty  of
  MERCHANTABILITY  or FITNESS  FOR A  PARTICULAR PURPOSE.   See  the GNU
  Lesser General Public License for more details.

  You  should have  received a  copy of  the GNU  Lesser  General Public
  License along  with this library; if  not, write to  the Free Software
  Foundation, Inc.,  59 Temple Place,  Suite 330, Boston,  MA 02111-1307
  USA.

*/

#include "ccevents.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

static void
test_single_timer_expiration (void)
{
  volatile bool		error_flag      = false;
  volatile bool		expiration_flag = false;

  void timeout_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src)
  {
    //fprintf(stderr, "%s: here\n", __func__);
    expiration_flag = true;
    ccevents_source_dequeue_itself(src);
  }

  {
    cce_location_t		L[1];
    ccevents_group_t		grp[1];
    ccevents_timer_source_t	timsrc[1];
    ccevents_timeout_t		expiration_to;
    ccevents_timeval_t		expiration_tv;
    ccevents_timeout_t		loop_to;
    ccevents_timeval_t		loop_tv;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      ccevents_timer_source_init(timsrc);

      expiration_to = ccevents_timeout_init(L, 0, 1, 0);
      expiration_tv = ccevents_timeout_start(L, expiration_to);
      ccevents_source_set_timeout(timsrc, expiration_tv, timeout_handler);

      ccevents_group_init(grp, 100);
      ccevents_group_enqueue_source(grp, timsrc);

      loop_to = ccevents_timeout_init(L, 0, 10, 0);
      loop_tv = ccevents_timeout_start(L, loop_to);
      do {
	ccevents_group_enter(grp);
      } while (! ccevents_timeval_is_expired_timeout(loop_tv));
      //fprintf(stderr, "loop expired? %d\n", ccevents_timeout_expired(&loop_time));

      cce_run_cleanup_handlers(L);
    }
  }
  assert(false == error_flag);
  assert(true  == expiration_flag);
}

int
main (int argc CCEVENTS_UNUSED, const char *const argv[] CCEVENTS_UNUSED)
{
  ccevents_init();
  if (1) test_single_timer_expiration();
  exit(EXIT_SUCCESS);
}

/* end of file */
