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
test_loop_single_group_single_source__dequeued_both (void)
/* A single group  with a single source.  Both the  group and the source
   dequeue themselves triggering the loop exit. */
{
  ccevents_loop_t		loop[1];
  ccevents_group_t		grp[1];
  ccevents_task_source_t	src[1];
  cce_location_t		L[1];
  volatile bool			error_flag = false;
  volatile int			count = 0;

  bool event_inquirer (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
  {
    return true;
  }
  void event_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src)
  {
    ++count;
    if (count == 4) {
      ccevents_group_dequeue_itself(ccevents_source_get_group(src));
      ccevents_source_dequeue_itself(src);
    }
  }

  if (cce_location(L)) {
    error_flag = true;
    cce_run_error_handlers(L);
    cce_condition_free(cce_condition(L));
  } else {
    ccevents_loop_init(loop);
    ccevents_group_init(grp, 10);
    ccevents_task_source_init(src);
    ccevents_task_source_set(src, event_inquirer, event_handler);
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
test_loop_single_group_single_source__dequeued_group (void)
/* A single group  with a single source.  The group  dequeues itself and
   posts a request to exit. */
{
  ccevents_loop_t		loop[1];
  ccevents_group_t		grp[1];
  ccevents_task_source_t	src[1];
  cce_location_t		L[1];
  volatile bool			error_flag = false;
  volatile int			count = 0;

  bool event_inquirer (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
  {
    return true;
  }
  void event_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src)
  {
    ++count;
    if (count == 4) {
      ccevents_group_t *	grp = ccevents_source_get_group(src);
      ccevents_group_dequeue_itself(grp);
      ccevents_group_post_request_to_leave(grp);
    }
  }

  if (cce_location(L)) {
    error_flag = true;
    cce_run_error_handlers(L);
    cce_condition_free(cce_condition(L));
  } else {
    ccevents_loop_init(loop);
    ccevents_group_init(grp, 10);
    ccevents_task_source_init(src);
    ccevents_task_source_set(src, event_inquirer, event_handler);
    ccevents_group_enqueue_source(grp, src);
    ccevents_loop_enqueue_group(loop, grp);
    ccevents_loop_enter(loop);
    cce_run_cleanup_handlers(L);
  }

  if (0) { fprintf(stderr, "%s: count=%d\n", __func__, count); }
  assert(false == error_flag);
  assert(4 == count);

  while (ccevents_loop_queue_is_not_empty(loop)) {
    ccevents_group_t *    grp CCEVENTS_UNUSED;
    grp = ccevents_loop_dequeue_group(loop);
  }
}


static void
test_loop_post_exit_in_loop (void)
/* A single group with  a single source.  A request to  exit the loop is
   posted. */
{
  ccevents_loop_t		loop[1];
  ccevents_group_t		grp[1];
  ccevents_task_source_t	src[1];
  cce_location_t		L[1];
  volatile bool			error_flag = false;
  volatile int			count = 0;

  bool event_inquirer (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
  {
    return true;
  }
  void event_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
  {
    ++count;
    if (4 == count) {
      ccevents_loop_post_request_to_leave(loop);
    }
    if (0) { fprintf(stderr, "%s: leaving count=%d\n", __func__, count); }
  }

  if (cce_location(L)) {
    error_flag = true;
    cce_run_error_handlers(L);
    cce_condition_free(cce_condition(L));
  } else {
    ccevents_loop_init(loop);
    ccevents_group_init(grp, 10);
    ccevents_task_source_init(src);
    ccevents_task_source_set(src, event_inquirer, event_handler);
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
test_loop_multi_groups (void)
{
  ccevents_loop_t		loop[1];
  ccevents_group_t		grpA[1], grpB[1], grpC[1];
  ccevents_task_source_t	srcA[1], srcB[1], srcC[1];
  cce_location_t		L[1];
  volatile bool			error_flag = false;
  volatile int			countA = 0, countB = 0, countC = 0;

  bool event_inquirer (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
  {
    return true;
  }
  void event_handler_A (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
  {
    ++countA;
    if (0) { fprintf(stderr, "%s: countA=%d\n", __func__, countA); }
  }
  void event_handler_B (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
  {
    ++countB;
    if (0) { fprintf(stderr, "%s: countB=%d\n", __func__, countB); }
  }
  void event_handler_C (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
  {
    ++countC;
    if (0) { fprintf(stderr, "%s: countC=%d\n", __func__, countC); }
    if (4 == countC) {
      ccevents_loop_post_request_to_leave(loop);
    }
  }

  if (cce_location(L)) {
    error_flag = true;
    cce_run_error_handlers(L);
    cce_condition_free(cce_condition(L));
  } else {
    ccevents_loop_init(loop);

    ccevents_group_init(grpA, 3);
    ccevents_group_init(grpB, 2);
    ccevents_group_init(grpC, 1);

    ccevents_task_source_init(srcA);
    ccevents_task_source_init(srcB);
    ccevents_task_source_init(srcC);

    ccevents_task_source_set(srcA, event_inquirer, event_handler_A);
    ccevents_task_source_set(srcB, event_inquirer, event_handler_B);
    ccevents_task_source_set(srcC, event_inquirer, event_handler_C);

    ccevents_group_enqueue_source(grpA, srcA);
    ccevents_group_enqueue_source(grpB, srcB);
    ccevents_group_enqueue_source(grpC, srcC);

    ccevents_loop_enqueue_group(loop, grpC);
    ccevents_loop_enqueue_group(loop, grpB);
    ccevents_loop_enqueue_group(loop, grpA);

    ccevents_loop_enter(loop);
    cce_run_cleanup_handlers(L);
  }

  fprintf(stderr, "countA=%d, countB=%d, countC=%d\n", countA, countB, countC);

  assert(false == error_flag);
  assert(12 == countA);
  assert(8 == countB);
  assert(4 == countC);
}


int
main (int argc CCEVENTS_UNUSED, const char *const argv[] CCEVENTS_UNUSED)
{
  ccevents_init();
  //
  if (1) test_loop_single_group_single_source__dequeued_both();
  if (1) test_loop_single_group_single_source__dequeued_group();
  if (1) test_loop_post_exit_in_loop();
  if (1) test_loop_multi_groups();
  //
  exit(EXIT_SUCCESS);
}

/* end of file */
