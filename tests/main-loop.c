/*
  Part of: CCEvents
  Contents: tests for the main loop
  Date: Fri Feb 10, 2017

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

#include <ccevents.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void
test_loop_single_group (void)
{
  ccevents_loop_t		loop[1];
  ccevents_group_t		grp[1];
  ccevents_task_source_t	src[1];
  cce_location_t		L[1];
  volatile bool			error_flag = false;
  volatile int			count = 0;

  bool event_inquirer (cce_location_t * there CCEVENTS_UNUSED,
		       ccevents_group_t * grp CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
  {
    return true;
  }
  void event_handler (cce_location_t * there CCEVENTS_UNUSED,
		      ccevents_group_t * grp, ccevents_source_t * src)
  {
    ++count;
    if (count < 4) {
      ccevents_group_enqueue_source(grp, src);
    } else {
      ccevents_loop_remove_group(loop, grp);
    }
  }

  if (cce_location(L)) {
    error_flag = true;
    cce_run_error_handlers(L);
    cce_condition_free(cce_location_condition(L));
  } else {
    ccevents_loop_init(loop);
    ccevents_group_init(grp, 10);
    ccevents_task_source_init(src);
    ccevents_task_source_set(L, src, event_inquirer, event_handler);
    ccevents_group_enqueue_source(grp, src);
    ccevents_loop_enqueue_group(loop, grp);
    ccevents_loop_enter(loop);
    cce_run_cleanup_handlers(L);
  }

  assert(false == error_flag);
  assert(4 == count);

  while (ccevents_loop_queue_is_not_empty(loop)) {
    ccevents_group_t *    grp CCEVENTS_UNUSED;
    grp = ccevents_loop_dequeue_group(loop);
  }
}

static void
test_loop_post_exit (void)
{
  ccevents_loop_t		loop[1];
  ccevents_group_t		grp[1];
  ccevents_task_source_t	src[1];
  cce_location_t		L[1];
  volatile bool			error_flag = false;
  volatile int			count = 0;

  bool event_inquirer (cce_location_t * there CCEVENTS_UNUSED,
		       ccevents_group_t * grp CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
  {
    return true;
  }
  void event_handler (cce_location_t * there CCEVENTS_UNUSED,
		      ccevents_group_t * grp, ccevents_source_t * src)
  {
    ++count;
    ccevents_group_enqueue_source(grp, src);
    if (4 == count) {
      ccevents_group_post_request_to_leave_asap(grp);
      ccevents_loop_post_request_to_leave_asap(loop);
    }
    fprintf(stderr, "%s: leaving count=%d\n", __func__, count);
  }

  if (cce_location(L)) {
    error_flag = true;
    cce_run_error_handlers(L);
    cce_condition_free(cce_location_condition(L));
  } else {
    ccevents_loop_init(loop);
    ccevents_group_init(grp, 10);
    ccevents_task_source_init(src);
    ccevents_task_source_set(L, src, event_inquirer, event_handler);
    ccevents_group_enqueue_source(grp, src);
    ccevents_loop_enqueue_group(loop, grp);
    ccevents_loop_enter(loop);
    cce_run_cleanup_handlers(L);
  }

  assert(false == error_flag);
  assert(4 == count);

  while (ccevents_loop_queue_is_not_empty(loop)) {
    ccevents_group_t *    grp CCEVENTS_UNUSED;
    grp = ccevents_loop_dequeue_group(loop);
  }
}

int
main (int argc CCEVENTS_UNUSED, const char *const argv[] CCEVENTS_UNUSED)
{
  ccevents_init();
  //
  if (1) test_loop_single_group();
  if (0) test_loop_post_exit();
  //
  exit(EXIT_SUCCESS);
}

/* end of file */
