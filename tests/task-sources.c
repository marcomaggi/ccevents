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

static void
test_single_task (void)
{
  ccevents_group_t		grp[1];
  ccevents_task_source_t	tksrc[1];
  volatile bool			expiration_flag = false;
  volatile bool			error_flag = false;
  volatile bool			step1_flag = false;
  volatile bool			step2_flag = false;
  volatile bool			step3_flag = false;

  auto ccevents_source_event_inquirer_fun_t	event_inquirer;
  auto ccevents_source_expiration_handler_fun_t	expiration_handler;
  auto ccevents_source_event_handler_fun_t	step1;
  auto ccevents_source_event_handler_fun_t	step2;
  auto ccevents_source_event_handler_fun_t	step3;

  bool event_inquirer (cce_location_t * there, ccevents_group_t * grp, ccevents_source_t * src)
  /* The next step is always ready to be executed. */
  {
    return true;
  }
  void expiration_handler (cce_location_t * there, ccevents_group_t * grp, ccevents_source_t * src)
  {
    expiration_flag = true;
  }
  void step1 (cce_location_t * there, ccevents_group_t * grp, ccevents_source_t * src)
  {
    ccevents_task_source_t *	tksrc = (ccevents_task_source_t *) src;
    step1_flag = true;
    ccevents_task_source_set(there, tksrc, event_inquirer, step2);
    ccevents_group_enqueue_source(grp, tksrc);
  }
  void step2 (cce_location_t * there, ccevents_group_t * grp, ccevents_source_t * src)
  {
    ccevents_task_source_t *	tksrc = (ccevents_task_source_t *) src;
    step2_flag = true;
    ccevents_task_source_set(there, tksrc, event_inquirer, step3);
    ccevents_group_enqueue_source(grp, tksrc);
  }
  void step3 (cce_location_t * there, ccevents_group_t * grp, ccevents_source_t * src)
  {
    step3_flag = true;
  }

  /* Do it. */
  {
    cce_location_t	L[1];

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers(L);
      cce_condition_free(cce_location_condition(L));
    } else {
      ccevents_task_source_init(tksrc);
      ccevents_source_set_timeout(tksrc, *CCEVENTS_TIMEOUT_NEVER, expiration_handler);
      ccevents_group_init(grp);
      ccevents_group_enqueue_source(grp, tksrc);
      ccevents_task_source_set(L, tksrc, event_inquirer, step1);
      ccevents_group_enter(grp, 3);
      cce_run_cleanup_handlers(L);
    }
  }
  assert(false == expiration_flag);
  assert(false == error_flag);
  assert(true  == step1_flag);
  assert(true  == step2_flag);
  assert(true  == step3_flag);
}

static void
test_multi_tasks (void)
/* Enqueue  3 tasks,  each  of 3  steps,  in a  group  and perform  them
   concurrently. */
{
  typedef struct task_t {
    ccevents_task_source_t;
    const char *name;
    bool	expiration_flag;
    bool	step1_flag;
    bool	step2_flag;
    bool	step3_flag;
  } task_t;

  ccevents_group_t	grp[1];
  task_t		tsk1[1];
  task_t		tsk2[1];
  task_t		tsk3[1];
  volatile bool		error_flag = false;

  auto void task_init (task_t * tsk, const char * name);
  auto ccevents_source_event_inquirer_fun_t	event_inquirer;
  auto ccevents_source_expiration_handler_fun_t	expiration_handler;
  auto ccevents_source_event_handler_fun_t	step1;
  auto ccevents_source_event_handler_fun_t	step2;
  auto ccevents_source_event_handler_fun_t	step3;

  void task_init (task_t * tsk, const char * name)
  {
    ccevents_task_source_init(tsk);
    tsk->name			= name;
    tsk->expiration_flag	= false;
    tsk->step1_flag		= false;
    tsk->step2_flag		= false;
    tsk->step3_flag		= false;
  }

  bool event_inquirer (cce_location_t * there CCEVENTS_UNUSED, ccevents_group_t * grp CCEVENTS_UNUSED, ccevents_source_t * src)
  /* The next step is always ready to be executed. */
  {
    task_t *	tsk = (task_t *) src;
    fprintf(stderr, "%s: query for readiness\n", tsk->name);
    return true;
  }
  void expiration_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_group_t * grp CCEVENTS_UNUSED, ccevents_source_t * src)
  {
    task_t *	tsk = (task_t *) src;
    tsk->expiration_flag = true;
  }
  void step1 (cce_location_t * there CCEVENTS_UNUSED, ccevents_group_t * grp CCEVENTS_UNUSED, ccevents_source_t * src)
  {
    task_t *	tsk = (task_t *) src;
    fprintf(stderr, "%s: step1\n", tsk->name);
    tsk->step1_flag = true;
    ccevents_task_source_set(there, tsk, event_inquirer, step2);
    ccevents_group_enqueue_source(grp, tsk);
  }
  void step2 (cce_location_t * there CCEVENTS_UNUSED, ccevents_group_t * grp CCEVENTS_UNUSED, ccevents_source_t * src)
  {
    task_t *	tsk = (task_t *) src;
    fprintf(stderr, "%s: step2\n", tsk->name);
    tsk->step2_flag = true;
    ccevents_task_source_set(there, tsk, event_inquirer, step3);
    ccevents_group_enqueue_source(grp, tsk);
  }
  void step3 (cce_location_t * there CCEVENTS_UNUSED, ccevents_group_t * grp CCEVENTS_UNUSED, ccevents_source_t * src)
  {
    task_t *	tsk = (task_t *) src;
    fprintf(stderr, "%s: step3\n", tsk->name);
    tsk->step3_flag = true;
  }

  /* Do it. */
  {
    cce_location_t	L[1];

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers(L);
      cce_condition_free(cce_location_condition(L));
    } else {
      task_init(tsk1, "multi task 1");
      task_init(tsk2, "multi task 2");
      task_init(tsk3, "multi task 3");
      ccevents_source_set_timeout(tsk1, *CCEVENTS_TIMEOUT_NEVER, expiration_handler);
      ccevents_source_set_timeout(tsk2, *CCEVENTS_TIMEOUT_NEVER, expiration_handler);
      ccevents_source_set_timeout(tsk3, *CCEVENTS_TIMEOUT_NEVER, expiration_handler);
      ccevents_group_init(grp);
      ccevents_group_enqueue_source(grp, tsk1);
      ccevents_group_enqueue_source(grp, tsk2);
      ccevents_group_enqueue_source(grp, tsk3);
      ccevents_task_source_set(L, tsk1, event_inquirer, step1);
      ccevents_task_source_set(L, tsk2, event_inquirer, step1);
      ccevents_task_source_set(L, tsk3, event_inquirer, step1);
      ccevents_group_enter(grp, 9);
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
  if (0) test_single_task();
  if (1) test_multi_tasks();
  exit(EXIT_SUCCESS);
}

/* end of file */
