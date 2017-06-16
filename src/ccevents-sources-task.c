/*
  Part of: CCEvents
  Contents: multi-step tasks events source
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

#include "ccevents-internals.h"


/** --------------------------------------------------------------------
 ** Method functions.
 ** ----------------------------------------------------------------- */

static bool
method_event_inquirer (cce_location_t * there, ccevents_source_t * src)
{
  CCEVENTS_PC(ccevents_task_source_t, tksrc, src);
  return tksrc->event_inquirer(there, tksrc);
}
static void
method_event_handler (cce_location_t * there, ccevents_source_t * src)
{
  CCEVENTS_PC(ccevents_task_source_t, tksrc, src);
  tksrc->event_handler(there, tksrc);
}
static const ccevents_source_vtable_t methods_table = {
  .event_inquirer	= method_event_inquirer,
  .event_handler	= method_event_handler,
};


/** --------------------------------------------------------------------
 ** Initialisation and setup.
 ** ----------------------------------------------------------------- */

void
ccevents_task_source_init (ccevents_task_source_t * tksrc)
/* Initialise an already  allocated source structure.  Do  not start the
   event waiting: this is done by "ccevents_task_source_set()". */
{
  ccevents_source_init(tksrc, &methods_table);
}

void
ccevents_task_source_set (ccevents_task_source_t    * tksrc,
			  ccevents_event_inquirer_t * event_inquirer,
			  ccevents_event_handler_t  * event_handler)
/* Set up an already initialised source to wait for an event.  Start the
   expiration timer.
*/
{
  tksrc->event_inquirer	= event_inquirer;
  tksrc->event_handler	= event_handler;
}

/* end of file */
