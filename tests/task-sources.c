/*
  Part of: CCEvents
  Contents: tests for the task sources
  Date: Sun Feb  5, 2017

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


static volatile bool test_1_0_expiration_flag = false;
static volatile bool test_1_0_step1_flag = false;
static volatile bool test_1_0_step2_flag = false;
static volatile bool test_1_0_step3_flag = false;

static ccevents_event_handler_t test_1_0_step1;
static ccevents_event_handler_t test_1_0_step2;
static ccevents_event_handler_t test_1_0_step3;

static bool
test_1_0_event_inquirer (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
/* The next step is always ready to be executed. */
{
  return true;
}
static void
test_1_0_expiration_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
{
  test_1_0_expiration_flag = true;
}
static void
test_1_0_step1 (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src)
{
  CCEVENTS_PC(ccevents_task_source_t, tksrc, src);
  test_1_0_step1_flag = true;
  ccevents_task_source_set(tksrc, test_1_0_event_inquirer, test_1_0_step2);
}
static void
test_1_0_step2 (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src)
{
  CCEVENTS_PC(ccevents_task_source_t, tksrc, src);
  test_1_0_step2_flag = true;
  ccevents_task_source_set(tksrc, test_1_0_event_inquirer, test_1_0_step3);
}
static void
test_1_0_step3 (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src)
{
  test_1_0_step3_flag = true;
  ccevents_source_dequeue_itself(src);
}

static void
test_1_0_single_task (void)
{
  ccevents_group_t		grp[1];
  ccevents_task_source_t	tksrc[1];
  volatile bool			error_flag = false;

  /* Do it. */
  {
    cce_location_t	L[1];

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      ccevents_task_source_init(tksrc);
      ccevents_source_set_timeout(ccevents_source(tksrc), *CCEVENTS_TIMEVAL_NEVER, test_1_0_expiration_handler);
      ccevents_group_init(grp, 3);
      ccevents_group_enqueue_source(grp, ccevents_source(tksrc));
      ccevents_task_source_set(tksrc, test_1_0_event_inquirer, test_1_0_step1);
      ccevents_group_enter(grp);
      cce_run_cleanup_handlers(L);
    }
  }
  assert(false == test_1_0_expiration_flag);
  assert(false == error_flag);
  assert(true  == test_1_0_step1_flag);
  assert(true  == test_1_0_step2_flag);
  assert(true  == test_1_0_step3_flag);
}


typedef struct test_2_0_task_t {
  ccevents_task_source_t	tksrc[1];
  const char *name;
  bool	expiration_flag;
  bool	step1_flag;
  bool	step2_flag;
  bool	step3_flag;
} test_2_0_task_t;

static ccevents_event_handler_t test_2_0_step1;
static ccevents_event_handler_t test_2_0_step2;
static ccevents_event_handler_t test_2_0_step3;

static void
test_2_0_task_init (test_2_0_task_t * tsk, const char * name)
{
  ccevents_task_source_init(tsk->tksrc);
  tsk->name		= name;
  tsk->expiration_flag	= false;
  tsk->step1_flag	= false;
  tsk->step2_flag	= false;
  tsk->step3_flag	= false;
}

static bool
test_2_0_event_inquirer (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src)
/* The next step is always ready to be executed. */
{
  CCEVENTS_PC(test_2_0_task_t, tsk, src);
  fprintf(stderr, "%s: query for readiness\n", tsk->name);
  return true;
}

static void
test_2_0_expiration_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src)
{
  CCEVENTS_PC(test_2_0_task_t, tsk, src);
  tsk->expiration_flag = true;
}

static void
test_2_0_step1 (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src)
{
  CCEVENTS_PC(test_2_0_task_t, tsk, src);
  fprintf(stderr, "%s: step1\n", tsk->name);
  tsk->step1_flag = true;
  ccevents_task_source_set(tsk->tksrc, test_2_0_event_inquirer, test_2_0_step2);
}

static void
test_2_0_step2 (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src)
{
  CCEVENTS_PC(test_2_0_task_t, tsk, src);
  fprintf(stderr, "%s: step2\n", tsk->name);
  tsk->step2_flag = true;
  ccevents_task_source_set(tsk->tksrc, test_2_0_event_inquirer, test_2_0_step3);
}

static void
test_2_0_step3 (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src)
{
  CCEVENTS_PC(test_2_0_task_t, tsk, src);
  fprintf(stderr, "%s: step3\n", tsk->name);
  tsk->step3_flag = true;
}

static void
test_2_0_multi_tasks (void)
/* Enqueue  3 tasks,  each  of 3  steps,  in a  group  and perform  them
   concurrently. */
{
  ccevents_group_t	grp[1];
  test_2_0_task_t	tsk1[1];
  test_2_0_task_t	tsk2[1];
  test_2_0_task_t	tsk3[1];
  volatile bool		error_flag = false;

  /* Do it. */
  {
    cce_location_t	L[1];

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      test_2_0_task_init(tsk1, "multi task 1");
      test_2_0_task_init(tsk2, "multi task 2");
      test_2_0_task_init(tsk3, "multi task 3");
      ccevents_source_set_timeout(ccevents_source(tsk1->tksrc), *CCEVENTS_TIMEVAL_NEVER, test_2_0_expiration_handler);
      ccevents_source_set_timeout(ccevents_source(tsk2->tksrc), *CCEVENTS_TIMEVAL_NEVER, test_2_0_expiration_handler);
      ccevents_source_set_timeout(ccevents_source(tsk3->tksrc), *CCEVENTS_TIMEVAL_NEVER, test_2_0_expiration_handler);
      ccevents_group_init(grp, 9);
      ccevents_group_enqueue_source(grp, ccevents_source(tsk1->tksrc));
      ccevents_group_enqueue_source(grp, ccevents_source(tsk2->tksrc));
      ccevents_group_enqueue_source(grp, ccevents_source(tsk3->tksrc));
      ccevents_task_source_set(tsk1->tksrc, test_2_0_event_inquirer, test_2_0_step1);
      ccevents_task_source_set(tsk2->tksrc, test_2_0_event_inquirer, test_2_0_step1);
      ccevents_task_source_set(tsk3->tksrc, test_2_0_event_inquirer, test_2_0_step1);
      ccevents_group_enter(grp);
      cce_run_cleanup_handlers(L);
    }
  }
  assert(false == error_flag);
  //
  assert(false == tsk1->expiration_flag);
  assert(true  == tsk1->step1_flag);
  assert(true  == tsk1->step2_flag);
  assert(true  == tsk1->step3_flag);
  //
  assert(false == tsk2->expiration_flag);
  assert(true  == tsk2->step1_flag);
  assert(true  == tsk2->step2_flag);
  assert(true  == tsk2->step3_flag);
  //
  assert(false == tsk3->expiration_flag);
  assert(true  == tsk3->step1_flag);
  assert(true  == tsk3->step2_flag);
  assert(true  == tsk3->step3_flag);
}


int
main (int argc CCEVENTS_UNUSED, const char *const argv[] CCEVENTS_UNUSED)
{
  ccevents_init();
  if (1) test_1_0_single_task();
  if (1) test_2_0_multi_tasks();
  exit(EXIT_SUCCESS);
}

/* end of file */
