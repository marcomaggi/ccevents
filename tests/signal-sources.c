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

int
main (int argc CCEVENTS_UNUSED, const char *const argv[] CCEVENTS_UNUSED)
{
  ccevents_signal_bub_init ();
  {
    if (1) test_bub_api();
  }
  ccevents_signal_bub_final ();
  exit(EXIT_SUCCESS);
}

/* end of file */
