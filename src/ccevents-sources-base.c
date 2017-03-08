/*
  Part of: CCEvents
  Contents: base event sources facilities
  Date: Thu Feb  2, 2017

  Abstract

	The struct type "ccevents_source_t"  represents the base type of
	all  the events  sources.   Through a  table  of functions,  the
	functions  implemented here  are  applicable to  all the  events
	sources.

  Copyright (C) 2017 Marco Maggi <marco.maggi-ipsu@poste.it>

  This is free software; you can  redistribute it and/or modify it under
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
 ** Dummy handler functions.
 ** ----------------------------------------------------------------- */

bool
ccevents_dummy_event_inquirer_false (cce_location_t    * L   CCEVENTS_UNUSED,
				     ccevents_source_t * src CCEVENTS_UNUSED)
{
  return false;
}
bool
ccevents_dummy_event_inquirer_true (cce_location_t    * L   CCEVENTS_UNUSED,
				    ccevents_source_t * src CCEVENTS_UNUSED)
{
  return true;
}

void
ccevents_dummy_event_handler (cce_location_t     * L   CCEVENTS_UNUSED,
			      ccevents_source_t  * src CCEVENTS_UNUSED)
{
  return;
}

void
ccevents_dummy_timeout_handler (cce_location_t     * L   CCEVENTS_UNUSED,
				ccevents_source_t  * src CCEVENTS_UNUSED)
{
  return;
}

static void
ccevents_source_final_dummy (ccevents_source_t * src CCEVENTS_UNUSED)
{
  return;
}


/** --------------------------------------------------------------------
 ** Event sources object methods table.
 ** ----------------------------------------------------------------- */

static const ccevents_source_otable_t ccevents_source_otable_default_stru = {
  .final = ccevents_source_final_dummy
};

const ccevents_source_otable_t * const ccevents_source_otable_default = &ccevents_source_otable_default_stru;


/** --------------------------------------------------------------------
 ** Event sources API.
 ** ----------------------------------------------------------------- */

void
ccevents_source_init (ccevents_source_t * src, const ccevents_source_vtable_t * vtable)
{
  ccevents_queue_node_init(src);
  src->otable			= &ccevents_source_otable_default_stru;
  src->vtable			= vtable;
  src->expiration_time		= *CCEVENTS_TIMEVAL_NEVER;
  src->expiration_handler	= ccevents_dummy_timeout_handler;
  src->enabled			= 1;
}

void
ccevents_source_set_otable (ccevents_source_t * src, const ccevents_source_otable_t * otable)
{
  src->otable = otable;
}

void
ccevents_source_final (ccevents_source_t * src)
{
  src->otable->final(src);
}

/* ------------------------------------------------------------------ */

void
ccevents_source_set_timeout (ccevents_source_t * src, ccevents_timeval_t expiration_time,
			     ccevents_timeout_handler_t * expiration_handler)
{
  src->expiration_time		= expiration_time;
  src->expiration_handler	= expiration_handler;
}

bool
ccevents_source_query (cce_location_t * there, ccevents_source_t * src)
/* Query SRC for a pending event to be served.  Return TRUE if a pending
   event exists, otherwise return FALSE.

   If querying for an event raises an exceptional condition: a non-local
   exit is performed to THERE.
*/
{
  return src->vtable->event_inquirer(there, src);
}

void
ccevents_source_handle_event (cce_location_t * there, ccevents_source_t * src)
/* Handle a  pending event for SRC.   This function must be  called only
   after  a  call  to  "ccevents_source_query()"  applied  to  the  same
   operands returns TRUE.   If handling the event  raises an exceptional
   condition: a non-local exit is performed to THERE.
*/
{
  src->vtable->event_handler(there, src);
}

void
ccevents_source_handle_expiration (cce_location_t * there, ccevents_source_t * src)
/* Handle a  timeout expiration  event for SRC.   If handling  the event
   raises an  exceptional condition:  a non-local  exit is  performed to
   THERE. */
{
  src->expiration_handler(there, src);
}

bool
ccevents_source_do_one_event (cce_location_t * there, ccevents_source_t * src)
/* Consume one event  from SRC, if there is a  pending one.  Return TRUE
   if  one event  was served  or the  timeout expired;  otherwise FALSE.
   Exceptions raised while querying an  event source or serving an event
   will cause a non-local exit to THERE.
*/
{
  if (ccevents_source_servicing_is_enabled(src)) {
    if (ccevents_timeval_is_expired_timeout(src->expiration_time)) {
      ccevents_source_handle_expiration(there, src);
      return true;
    } else if (ccevents_source_query(there, src)) {
      ccevents_source_handle_event(there, src);
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

/* end of file */
