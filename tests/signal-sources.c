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

void
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

static void
test_source (void)
{
  volatile bool		error_flag  = false;
  volatile bool		signal_flag = false;

  ccevents_signal_bub_init();
  {
    ccevents_group_t			grp[1];
    /* This events source runs its  inquirer function again and again to
       acquire signals with the BUB API. */
    ccevents_task_source_t		acquire_signals_src[1];
    /* This events source  runs its inquirer function once  to raise the
       signal.  The event handler is never run. */
    ccevents_task_source_t		raise_signal_src[1];
    /* This events source reacts to delivery of SIGUSR1. */
    ccevents_signal_bub_source_t	sigsrc[1];
    cce_location_t			L[1];

    bool acquire_signals_inquirer (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
    {
      ccevents_signal_bub_acquire();
      return false;
    }
    void acquire_signals_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
    {
      return;
    }
    bool raise_signal_inquirer (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
    {
      raise(SIGUSR1);
      return false;
    }
    void raise_signal_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
    {
      return;
    }
    void signal_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
    {
      signal_flag = true;
    }

    ccevents_group_init(grp, 1000);
    ccevents_task_source_init(acquire_signals_src);
    ccevents_task_source_init(raise_signal_src);
    ccevents_signal_bub_source_init(sigsrc, SIGUSR1);

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      ccevents_task_source_set(acquire_signals_src, acquire_signals_inquirer, ccevents_dummy_event_handler);
      ccevents_task_source_set(raise_signal_src,       raise_signal_inquirer, ccevents_dummy_event_handler);
      ccevents_signal_bub_source_set(sigsrc, signal_handler);
      ccevents_group_enqueue_source(grp, sigsrc);
      ccevents_group_enqueue_source(grp, acquire_signals_src);
      ccevents_group_enqueue_source(grp, raise_signal_src);
      ccevents_group_enter(grp);
      cce_run_cleanup_handlers(L);
    }
  }
  ccevents_signal_bub_final();
  assert(false == error_flag);
  assert(true == signal_flag);
}

static void
test_source_for_documentation (void)
{
  ccevents_group_t grp[1];

  /* This events  source runs its  inquirer function again and  again to
     acquire signals with the BUB API. */
  ccevents_task_source_t acquire_signals_src[1];

  /* This events source reacts to the delivery of SIGUSR1. */
  ccevents_signal_bub_source_t sigsrc[1];

  cce_location_t L[1];
  volatile sig_atomic_t signal_flag = 0;
  volatile bool		error_flag = false;

  bool acquire_signals_inquirer (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
  {
    ccevents_signal_bub_acquire();
    return false;
  }
  void signal_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
  {
    signal_flag = 1;
  }

  ccevents_signal_bub_init();
  {
    ccevents_group_init(grp, 1000);
    ccevents_task_source_init(acquire_signals_src);
    ccevents_signal_bub_source_init(sigsrc, SIGUSR1);

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      ccevents_task_source_set(acquire_signals_src, acquire_signals_inquirer, ccevents_dummy_event_handler);
      ccevents_signal_bub_source_set(sigsrc, signal_handler);
      ccevents_group_enqueue_source(grp, sigsrc);
      ccevents_group_enqueue_source(grp, acquire_signals_src);
      ccevents_group_enter(grp);
      cce_run_cleanup_handlers(L);
    }
  }
  assert(false == error_flag);
  assert(0 == signal_flag);
  ccevents_signal_bub_final();
}

int
main (int argc CCEVENTS_UNUSED, const char *const argv[] CCEVENTS_UNUSED)
{
  ccevents_init();
  if (1) test_bub_api();
  if (1) test_source();
  if (1) test_source_for_documentation();
  exit(EXIT_SUCCESS);
}

/* end of file */
