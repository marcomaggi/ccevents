/*
  Part of: CCEvents
  Contents: tests for the base sources
  Date: Wed Mar  8, 2017

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
test_enabling_and_disabling_servicing (void)
{
  ccevents_source_t	src[1];
  volatile bool		error_flag = false;
  volatile bool		event_flag = false;

  bool event_inquirer (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
  {
    return true;
  }
  void event_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
  {
    event_flag = true;
  }

  ccevents_source_vtable_t vtable = {
    .event_inquirer = event_inquirer,
    .event_handler  = event_handler
  };

  {
    cce_location_t	L[1];

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers(L);
      cce_condition_free(cce_condition(L));
    } else {
      ccevents_source_init(src, &vtable);
      ccevents_source_disable_servicing(src);
      ccevents_source_do_one_event(L, src);
      assert(false == event_flag);
      ccevents_source_enable_servicing(src);
      ccevents_source_do_one_event(L, src);
      assert(true  == event_flag);
      cce_run_cleanup_handlers(L);
    }
  }
  assert(false == error_flag);
}


int
main (int argc CCEVENTS_UNUSED, const char *const argv[] CCEVENTS_UNUSED)
{
  ccevents_init();
  if (1) test_enabling_and_disabling_servicing();
   exit(EXIT_SUCCESS);
}

/* end of file */