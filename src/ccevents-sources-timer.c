/*
  Part of: CCEvents
  Contents: timer events sources
  Date: Mon Feb  6, 2017

  Abstract

	Actually this module does *not* operating system timers.

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


/** --------------------------------------------------------------------
 ** Method functions.
 ** ----------------------------------------------------------------- */

static bool
method_event_inquirer (cce_location_t    * there CCEVENTS_UNUSED,
		       ccevents_source_t * src   CCEVENTS_UNUSED)
{
  return false;
}
static void
method_event_handler (cce_location_t    * there CCEVENTS_UNUSED,
		      ccevents_source_t * src   CCEVENTS_UNUSED)
{
  return;
}
static const ccevents_source_vtable_t methods_table = {
  .event_inquirer	= method_event_inquirer,
  .event_handler	= method_event_handler,
};


/** --------------------------------------------------------------------
 ** Initialisation and setup.
 ** ----------------------------------------------------------------- */

void
ccevents_timer_source_init (ccevents_timer_source_t * timsrc)
{
  ccevents_source_init(timsrc, &methods_table);
}

/* end of file */
