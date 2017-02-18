/*
  Part of: CCEvents
  Contents: interprocess signals events sources
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

#include "ccevents-internals.h"
#include <signal.h>

/** --------------------------------------------------------------------
 ** Signal handler.
 ** ----------------------------------------------------------------- */

/* An integer falg  for every signal.  When the BUB  API is initialised:
   every time a signal arrives the corresponding flag is set. */
static volatile sig_atomic_t	arrived_signals[NSIG];

static sigset_t	all_signals_set;

static void
signal_bub_handler (int signum)
/* The signal handler registered by the BUB API. */
{
  arrived_signals[signum] = 1;
  if (0) {
    fprintf(stderr, "%s: %d\n", __func__, signum);
  }
}

/** --------------------------------------------------------------------
 ** Signal BUB API initialisation and finalisation.
 ** ----------------------------------------------------------------- */

void
ccevents_signal_bub_init (void)
/* Initialise  the BUB  API, enabling  signal handling.   Block all  the
   signals and register our handler for each.
*/
{
  struct sigaction	ac = {
    .sa_handler	= signal_bub_handler,
    .sa_flags	= SA_RESTART | SA_NOCLDSTOP
  };
  int	signum;
  sigfillset(&all_signals_set);
  sigprocmask(SIG_BLOCK, &all_signals_set, NULL);
  for (signum=0; signum<NSIG; ++signum) {
    arrived_signals[signum] = 0;
    sigaction(signum, &ac, NULL);
  }
}

void
ccevents_signal_bub_final (void)
/* Finalise  the  BUB  API,  disabling signal  handling.   Set  all  the
   handlers to SIG_IGN, then unblock the signals.
*/
{
  struct sigaction	ac = {
    .sa_handler	= SIG_IGN,
    .sa_flags	= SA_RESTART
  };
  for (int signum=0; signum<NSIG; ++signum) {
    arrived_signals[signum] = 0;
    sigaction(signum, &ac, NULL);
  }
  sigprocmask(SIG_UNBLOCK, &all_signals_set, NULL);
}

void
ccevents_signal_bub_acquire (void)
/* Unblock then block  all the signals.  This causes  blocked signals to
   be delivered.
*/
{
  sigprocmask(SIG_UNBLOCK, &all_signals_set, NULL);
  sigprocmask(SIG_BLOCK,   &all_signals_set, NULL);
}

bool
ccevents_signal_bub_delivered (int signum)
{
  bool	flag = (arrived_signals[signum])? true : false;
  arrived_signals[signum] = 0;
  return flag;
}

/** --------------------------------------------------------------------
 ** Method functions.
 ** ----------------------------------------------------------------- */

static bool
method_event_inquirer (cce_location_t * there CCEVENTS_UNUSED, ccevents_group_t * grp, ccevents_source_t * src)
/* Return true if the signal  SIGSRC->SIGNUM has been delivered at least
   once since  the last call to  "ccevents_signal_bub_acquire()".  Clear
   the signal flag.
*/
{
  CCEVENTS_PC(ccevents_signal_bub_source_t, sigsrc, src);
  bool	flag = ccevents_signal_bub_delivered(sigsrc->signum);
  if (! flag) {
    ccevents_group_enqueue_source(grp, src);
  }
  return flag;
}
static void
method_event_handler (cce_location_t * there, ccevents_group_t * grp, ccevents_source_t * src)
{
  CCEVENTS_PC(ccevents_signal_bub_source_t, sigsrc, src);
  return sigsrc->event_handler(there, grp, sigsrc);
}
static const ccevents_source_vtable_t methods_table = {
  .event_inquirer	= method_event_inquirer,
  .event_handler	= method_event_handler,
};

/** --------------------------------------------------------------------
 ** Initialisation and setup.
 ** ----------------------------------------------------------------- */

void
ccevents_signal_bub_source_init (ccevents_signal_bub_source_t * sigsrc, int signum)
/* Initialise an already  allocated source structure.  Do  not start the
   event waiting: this is done by "ccevents_signal_bub_source_set()". */
{
  ccevents_source_init(sigsrc, &methods_table);
  sigsrc->signum = signum;
  arrived_signals[signum] = 0;
}

void
ccevents_signal_bub_source_set (cce_location_t * there, ccevents_signal_bub_source_t * sigsrc,
				ccevents_source_event_handler_fun_t      * event_handler)
/* Set up an already initialised source to wait for an event.  Start the
   expiration timer.
*/
{
  sigsrc->event_handler = event_handler;
}

/* end of file */
