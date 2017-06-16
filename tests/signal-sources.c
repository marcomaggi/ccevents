/*
  Part of: CCEvents
  Contents: tests for interprocess signals events sources
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

#include <ccevents.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


/* Basic tests for the BUB API. */

static void
test_bub_api (void)
{
  ccevents_signal_bub_init ();
  {
    {
      bool	flag;
      raise(SIGUSR1);
      ccevents_signal_bub_acquire();
      flag = ccevents_signal_bub_delivered(SIGUSR1);
      if (0) fprintf(stderr, "flag %d\n", flag);
      assert(1 == flag);
    }

    /* Multiple signals close together are merged into one. */
    {
      bool	flag;
      raise(SIGUSR2);
      raise(SIGUSR2);
      raise(SIGUSR2);
      raise(SIGUSR2);
      ccevents_signal_bub_acquire();
      flag = ccevents_signal_bub_delivered(SIGUSR2);
      if (0) fprintf(stderr, "flag %d\n", flag);
      assert(1 == flag);
    }

    /* Different signals acquired. */
    {
      bool	flag1, flag2;
      raise(SIGUSR1);
      raise(SIGUSR2);
      ccevents_signal_bub_acquire();
      flag1 = ccevents_signal_bub_delivered(SIGUSR1);
      flag2 = ccevents_signal_bub_delivered(SIGUSR2);
      assert(true == flag1);
      assert(true == flag2);
    }
  }
  ccevents_signal_bub_final ();
}


/* Test the  signal source with  a group of  sources.  A task  source is
   used to raise signals.  A task  source is used to acquire signals.  A
   signals source is used to handle delivered SIGUSR1 signals. */

typedef struct test_2_0_sigusr1_handler_t {
  /* This events source reacts to delivery of SIGUSR1. */
  ccevents_signal_bub_source_t	src;
  /* This flag is set to 1 if a SIGUSR1 is received. */
  sig_atomic_t			signal_flag;
} test_2_0_sigusr1_handler_t;

static bool
test_2_0_signals_acquirer_inquirer (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
{
  ccevents_signal_bub_acquire();
  return false;
}

static bool
test_2_0_signals_raiser_inquirer (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
{
  raise(SIGUSR1);
  return false;
}

static void
test_2_0_sigusr1_handler_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * _src)
{
  test_2_0_sigusr1_handler_t *	sigusr1_handler = (test_2_0_sigusr1_handler_t *) _src;
  sigusr1_handler->signal_flag = true;
}

static void
test_2_0 (void)
{
  /* This struct represents the state of the SIGUSR1 signal handler. */
  test_2_0_sigusr1_handler_t	sigusr1_handler = { .signal_flag = false };
  /* This flag is set to true  if an exception is catched.  Otherwise it
     is left false. */
  volatile bool		error_flag  = false;

  ccevents_signal_bub_init();
  {
    ccevents_group_t			grp[1];
    /* This events source runs its  inquirer function again and again to
       acquire signals with the BUB API. */
    ccevents_task_source_t		signals_acquirer_src[1];
    /* This events source  runs its inquirer function once  to raise the
       signal.  The event handler is never run. */
    ccevents_task_source_t		signals_raiser_src[1];
    cce_location_t			L[1];

    ccevents_group_init(grp, 1000);
    ccevents_task_source_init(signals_acquirer_src);
    ccevents_task_source_init(signals_raiser_src);
    ccevents_signal_bub_source_init(&sigusr1_handler.src, SIGUSR1);

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      ccevents_task_source_set(signals_acquirer_src, test_2_0_signals_acquirer_inquirer, ccevents_dummy_event_handler);
      ccevents_task_source_set(signals_raiser_src,   test_2_0_signals_raiser_inquirer,   ccevents_dummy_event_handler);
      ccevents_signal_bub_source_set(&sigusr1_handler.src, test_2_0_sigusr1_handler_handler);
      ccevents_group_enqueue_source(grp, &sigusr1_handler.src);
      ccevents_group_enqueue_source(grp, signals_acquirer_src);
      ccevents_group_enqueue_source(grp, signals_raiser_src);
      ccevents_group_enter(grp);
      cce_run_cleanup_handlers(L);
    }
  }
  ccevents_signal_bub_final();
  assert(false == error_flag);
  assert(true == sigusr1_handler.signal_flag);
}


/* Code  chunks for  documentation purposes.   Tests the  signals source
   with a group of sources. */

typedef struct test_3_0_sigusr1_handler_t {
  /* This events source reacts to the delivery of SIGUSR1. */
  ccevents_signal_bub_source_t	src;
  /* This flag is set to 1 if a SIGUSR1 is received. */
  sig_atomic_t			signal_flag;
} test_3_0_sigusr1_handler_t;

static bool
test_3_0_signals_acquirer_inquirer (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
{
  ccevents_signal_bub_acquire();
  return false;
}

static void
test_3_0_signal_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * _src)
{
  test_3_0_sigusr1_handler_t *	sigusr1_handler = (test_3_0_sigusr1_handler_t *) _src;
  sigusr1_handler->signal_flag = 1;
}

static void
test_3_0 (void)
{
  ccevents_group_t grp[1];
  /* This events  source runs its  inquirer function again and  again to
     acquire signals with the BUB API. */
  ccevents_task_source_t	signals_acquirer_src[1];
  /* This struct represents the state of the SIGUSR1 signal handler. */
  test_3_0_sigusr1_handler_t	sigusr1_handler = { .signal_flag = 0 };
  /* This flag is set to true  if an exception is catched.  Otherwise it
     is left false. */
  volatile bool			error_flag = false;
  cce_location_t L[1];

  ccevents_signal_bub_init();
  {
    ccevents_group_init(grp, 1000);
    ccevents_task_source_init(signals_acquirer_src);
    ccevents_signal_bub_source_init(&sigusr1_handler.src, SIGUSR1);

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      ccevents_task_source_set(signals_acquirer_src, test_3_0_signals_acquirer_inquirer, ccevents_dummy_event_handler);
      ccevents_signal_bub_source_set(&sigusr1_handler.src, test_3_0_signal_handler);
      ccevents_group_enqueue_source(grp, &sigusr1_handler.src);
      ccevents_group_enqueue_source(grp, signals_acquirer_src);
      ccevents_group_enter(grp);
      cce_run_cleanup_handlers(L);
    }
  }
  assert(false == error_flag);
  assert(0 == sigusr1_handler.signal_flag);
  ccevents_signal_bub_final();
}


int
main (int argc CCEVENTS_UNUSED, const char *const argv[] CCEVENTS_UNUSED)
{
  ccevents_init();
  if (1) test_bub_api();
  if (1) test_2_0();
  if (1) test_3_0();
  exit(EXIT_SUCCESS);
}

/* end of file */
