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


static volatile int test_1_0_count = 0;

static bool
test_1_0_event_inquirer (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
{
  return true;
}
static void
test_1_0_event_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src)
{
  ++test_1_0_count;
  if (test_1_0_count == 4) {
    ccevents_group_dequeue_itself(ccevents_source_get_group(src));
    ccevents_source_dequeue_itself(src);
  }
}

static void
test_1_0_loop_single_group_single_source__dequeued_both (void)
/* A single group  with a single source.  Both the  group and the source
   dequeue themselves triggering the loop exit. */
{
  ccevents_loop_t		loop[1];
  ccevents_group_t		grp[1];
  ccevents_task_source_t	src[1];
  cce_location_t		L[1];
  volatile bool			error_flag = false;

  if (cce_location(L)) {
    error_flag = true;
    cce_run_error_handlers_final(L);
  } else {
    ccevents_loop_init(loop);
    ccevents_group_init(grp, 10);
    ccevents_task_source_init(src);
    ccevents_task_source_set(src, test_1_0_event_inquirer, test_1_0_event_handler);
    ccevents_group_enqueue_source(grp, ccevents_source(src));
    ccevents_loop_enqueue_group(loop, grp);
    ccevents_loop_enter(loop);
    cce_run_cleanup_handlers(L);
  }

  assert(false == error_flag);
  assert(4 == test_1_0_count);

  while (ccevents_loop_queue_is_not_empty(loop)) {
    ccevents_group_t *    grp CCEVENTS_UNUSED;
    grp = ccevents_loop_dequeue_group(loop);
  }
}


static volatile int test_1_1_count = 0;

static bool
test_1_1_event_inquirer (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
{
  return true;
}

static void
test_1_1_event_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src)
{
  ++test_1_1_count;
  if (test_1_1_count == 4) {
    ccevents_group_t *	grp = ccevents_source_get_group(src);
    ccevents_group_dequeue_itself(grp);
    ccevents_group_post_request_to_leave(grp);
  }
}

static void
test_1_1_loop_single_group_single_source__dequeued_group (void)
/* A single group  with a single source.  The group  dequeues itself and
   posts a request to exit. */
{
  ccevents_loop_t		loop[1];
  ccevents_group_t		grp[1];
  ccevents_task_source_t	src[1];
  cce_location_t		L[1];
  volatile bool			error_flag = false;

  if (cce_location(L)) {
    error_flag = true;
    cce_run_error_handlers_final(L);
  } else {
    ccevents_loop_init(loop);
    ccevents_group_init(grp, 10);
    ccevents_task_source_init(src);
    ccevents_task_source_set(src, test_1_1_event_inquirer, test_1_1_event_handler);
    ccevents_group_enqueue_source(grp, ccevents_source(src));
    ccevents_loop_enqueue_group(loop, grp);
    ccevents_loop_enter(loop);
    cce_run_cleanup_handlers(L);
  }

  if (0) { fprintf(stderr, "%s: test_1_1_count=%d\n", __func__, test_1_1_count); }
  assert(false == error_flag);
  assert(4 == test_1_1_count);

  while (ccevents_loop_queue_is_not_empty(loop)) {
    ccevents_group_t *    grp CCEVENTS_UNUSED;
    grp = ccevents_loop_dequeue_group(loop);
  }
}


static volatile int test_2_0_count = 0;
static ccevents_loop_t test_2_0_loop[1];

static bool
test_2_0_event_inquirer (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
{
  return true;
}

static void
test_2_0_event_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
{
  ++test_2_0_count;
  if (4 == test_2_0_count) {
    ccevents_loop_post_request_to_leave(test_2_0_loop);
  }
  if (0) { fprintf(stderr, "%s: leaving test_2_0_count=%d\n", __func__, test_2_0_count); }
}

static void
test_2_0_loop_post_exit_in_loop (void)
/* A single group with  a single source.  A request to  exit the loop is
   posted. */
{
  ccevents_group_t		grp[1];
  ccevents_task_source_t	src[1];
  cce_location_t		L[1];
  volatile bool			error_flag = false;

  if (cce_location(L)) {
    error_flag = true;
    cce_run_error_handlers_final(L);
  } else {
    ccevents_loop_init(test_2_0_loop);
    ccevents_group_init(grp, 10);
    ccevents_task_source_init(src);
    ccevents_task_source_set(src, test_2_0_event_inquirer, test_2_0_event_handler);
    ccevents_group_enqueue_source(grp, ccevents_source(src));
    ccevents_loop_enqueue_group(test_2_0_loop, grp);
    ccevents_loop_enter(test_2_0_loop);
    cce_run_cleanup_handlers(L);
  }

  assert(false == error_flag);
  assert(4 == test_2_0_count);

  while (ccevents_loop_queue_is_not_empty(test_2_0_loop)) {
    ccevents_group_t *    grp CCEVENTS_UNUSED;
    grp = ccevents_loop_dequeue_group(test_2_0_loop);
  }
}


static ccevents_loop_t test_3_0_loop[1];
static volatile int test_3_0_countA = 0, test_3_0_countB = 0, test_3_0_countC = 0;

static bool
test_3_0_event_inquirer (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
{
  return true;
}

static void
test_3_0_event_handler_A (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
{
  ++test_3_0_countA;
  if (0) { fprintf(stderr, "%s: test_3_0_countA=%d\n", __func__, test_3_0_countA); }
}

static void
test_3_0_event_handler_B (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
{
  ++test_3_0_countB;
  if (0) { fprintf(stderr, "%s: test_3_0_countB=%d\n", __func__, test_3_0_countB); }
}

static void
test_3_0_event_handler_C (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
{
  ++test_3_0_countC;
  if (0) { fprintf(stderr, "%s: test_3_0_countC=%d\n", __func__, test_3_0_countC); }
  if (4 == test_3_0_countC) {
    ccevents_loop_post_request_to_leave(test_3_0_loop);
  }
}

static void
test_3_0_loop_multi_groups (void)
{
  ccevents_group_t		grpA[1], grpB[1], grpC[1];
  ccevents_task_source_t	srcA[1], srcB[1], srcC[1];
  cce_location_t		L[1];
  volatile bool			error_flag = false;

  if (cce_location(L)) {
    error_flag = true;
    cce_run_error_handlers_final(L);
  } else {
    ccevents_loop_init(test_3_0_loop);

    ccevents_group_init(grpA, 3);
    ccevents_group_init(grpB, 2);
    ccevents_group_init(grpC, 1);

    ccevents_task_source_init(srcA);
    ccevents_task_source_init(srcB);
    ccevents_task_source_init(srcC);

    ccevents_task_source_set(srcA, test_3_0_event_inquirer, test_3_0_event_handler_A);
    ccevents_task_source_set(srcB, test_3_0_event_inquirer, test_3_0_event_handler_B);
    ccevents_task_source_set(srcC, test_3_0_event_inquirer, test_3_0_event_handler_C);

    ccevents_group_enqueue_source(grpA, ccevents_source(srcA));
    ccevents_group_enqueue_source(grpB, ccevents_source(srcB));
    ccevents_group_enqueue_source(grpC, ccevents_source(srcC));

    ccevents_loop_enqueue_group(test_3_0_loop, grpC);
    ccevents_loop_enqueue_group(test_3_0_loop, grpB);
    ccevents_loop_enqueue_group(test_3_0_loop, grpA);

    ccevents_loop_enter(test_3_0_loop);
    cce_run_cleanup_handlers(L);
  }

  fprintf(stderr, "test_3_0_countA=%d, test_3_0_countB=%d, test_3_0_countC=%d\n", test_3_0_countA, test_3_0_countB, test_3_0_countC);

  assert(false == error_flag);
  assert(12 == test_3_0_countA);
  assert(8  == test_3_0_countB);
  assert(4  == test_3_0_countC);
}


volatile bool test_4_0_finalisation_A = false;
volatile bool test_4_0_finalisation_B = false;
volatile bool test_4_0_finalisation_C = false;
volatile bool test_4_0_finalisation_D = false;

static void
test_4_0_source_finalise_A (ccevents_source_t * src CCEVENTS_UNUSED)
{
  test_4_0_finalisation_A = true;
}

static void
test_4_0_source_finalise_B (ccevents_source_t * src CCEVENTS_UNUSED)
{
  test_4_0_finalisation_B = true;
}

static void
test_4_0_source_finalise_C (ccevents_source_t * src CCEVENTS_UNUSED)
{
  test_4_0_finalisation_C = true;
}

static void
test_4_0_source_finalise_D (ccevents_source_t * src CCEVENTS_UNUSED)
{
  test_4_0_finalisation_D = true;
}

void
test_4_0_loop_finalisation (void)
{
  ccevents_loop_t		L[1];
  ccevents_group_t		G[1];
  ccevents_task_source_t	A[1], B[1], C[1], D[1];

  const ccevents_source_otable_t otableA = { .final = test_4_0_source_finalise_A };
  const ccevents_source_otable_t otableB = { .final = test_4_0_source_finalise_B };
  const ccevents_source_otable_t otableC = { .final = test_4_0_source_finalise_C };
  const ccevents_source_otable_t otableD = { .final = test_4_0_source_finalise_D };

  ccevents_task_source_init(A);
  ccevents_task_source_init(B);
  ccevents_task_source_init(C);
  ccevents_task_source_init(D);

  ccevents_source_set_otable(ccevents_source(A), &otableA);
  ccevents_source_set_otable(ccevents_source(B), &otableB);
  ccevents_source_set_otable(ccevents_source(C), &otableC);
  ccevents_source_set_otable(ccevents_source(D), &otableD);

  ccevents_loop_init(L);
  ccevents_group_init(G, 10);

  ccevents_group_enqueue_source(G, ccevents_source(A));
  ccevents_group_enqueue_source(G, ccevents_source(B));
  ccevents_group_enqueue_source(G, ccevents_source(C));
  ccevents_group_enqueue_source(G, ccevents_source(D));

  ccevents_loop_enqueue_group(L, G);

  ccevents_loop_final(L);

  assert(true == test_4_0_finalisation_A);
  assert(true == test_4_0_finalisation_B);
  assert(true == test_4_0_finalisation_C);
  assert(true == test_4_0_finalisation_D);
}


int
main (int argc CCEVENTS_UNUSED, const char *const argv[] CCEVENTS_UNUSED)
{
  ccevents_init();
  //
  if (1) test_1_0_loop_single_group_single_source__dequeued_both();
  if (1) test_1_1_loop_single_group_single_source__dequeued_group();
  if (1) test_2_0_loop_post_exit_in_loop();
  if (1) test_3_0_loop_multi_groups();
  if (1) test_4_0_loop_finalisation();
  //
  exit(EXIT_SUCCESS);
}

/* end of file */
