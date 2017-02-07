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

/** --------------------------------------------------------------------
 ** Method functions.
 ** ----------------------------------------------------------------- */

static bool
method_event_inquirer (cce_location_t * there, ccevents_group_t * grp, ccevents_source_t * src)
{
  ccevents_signal_source_t *	sigsrc = (ccevents_signal_source_t *) src;
  return sigsrc->event_inquirer(there, grp, sigsrc);
}
static void
method_event_handler (cce_location_t * there, ccevents_group_t * grp, ccevents_source_t * src)
{
  ccevents_signal_source_t *	sigsrc = (ccevents_signal_source_t *) src;
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
ccevents_signal_source_init (ccevents_signal_source_t * sigsrc)
/* Initialise an already  allocated source structure.  Do  not start the
   event waiting: this is done by "ccevents_signal_source_set()". */
{
  ccevents_source_init(sigsrc, &methods_table);
}

void
ccevents_signal_source_set (cce_location_t * there, ccevents_signal_source_t * sigsrc,
			    ccevents_source_event_inquirer_fun_t     * event_inquirer,
			    ccevents_source_event_handler_fun_t      * event_handler)
/* Set up an already initialised source to wait for an event.  Start the
   expiration timer.
*/
{
  ccevents_source_set(there, sigsrc);
  sigsrc->event_inquirer	= event_inquirer;
  sigsrc->event_handler		= event_handler;
}

/* end of file */
