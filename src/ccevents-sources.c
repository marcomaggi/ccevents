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

static void
default_expiration_handler (cce_location_t * L   CCEVENTS_UNUSED,
			    ccevents_group_t   * grp CCEVENTS_UNUSED,
			    ccevents_source_t  * src CCEVENTS_UNUSED)
{
  return;
}

void
ccevents_source_init (ccevents_source_t * src, const ccevents_source_vtable_t * vtable)
/* Initialise an already  allocated source structure.  Do  not start the
   event waiting: this is done by "ccevents_source_set()". */
{
  //fprintf(stderr, "%s: enter\n", __func__);
  src->vtable			= vtable;
  src->prev			= NULL;
  src->next			= NULL;
  src->expiration_time		= *CCEVENTS_TIMEOUT_NEVER;
  src->expiration_handler	= default_expiration_handler;
}

void
ccevents_source_set_timeout (ccevents_source_t * src, ccevents_timeout_t expiration_time,
			     ccevents_source_expiration_handler_fun_t * expiration_handler)
{
  src->expiration_time		= expiration_time;
  src->expiration_handler	= expiration_handler;
}

void
ccevents_source_set (cce_location_t * there, ccevents_source_t * src)
/* Set up an already initialised source to wait for an event.  Start the
   expiration timer.
*/
{
  /* We  reset the  timeout just  in  case the  this set  call resets  a
     previously event selection. */
  ccevents_timeout_reset(&(src->expiration_time));
  /* Start the waiting. */
  ccevents_timeout_start(there, &(src->expiration_time));
}

bool
ccevents_source_query (cce_location_t * there, ccevents_group_t * grp, ccevents_source_t * src)
/* Query SRC for a pending event to be served.  Return TRUE if a pending
   event exists, otherwise return FALSE.

   If querying for an event raises an exceptional condition: a non-local
   exit is performed to THERE.

   When this function is called SRC has been dequeued from GRP: it is up
   to  the inquiry  function, or  an exceptional  condition handler,  to
   re-enqueue SRC into GRP or not.
*/
{
  return src->vtable->event_inquirer(there, grp, src);
}

void
ccevents_source_handle_event (cce_location_t * there, ccevents_group_t * grp, ccevents_source_t * src)
/* Handle a  pending event for SRC.   This function must be  called only
   after  a  call  to  "ccevents_source_query()"  applied  to  the  same
   operands returns TRUE.

   If handling  the event raises  an exceptional condition:  a non-local
   exit is performed to THERE.

   When this function is called SRC has been dequeued from GRP: it is up
   to  the  event  handler,  or an  exceptional  condition  handler,  to
   re-enqueue SRC into GRP or not.
*/
{
  src->vtable->event_handler(there, grp, src);
}

void
ccevents_source_handle_expiration (cce_location_t * there, ccevents_group_t * grp, ccevents_source_t * src)
/* Handle a timeout expiration event for SRC.

   If handling  the event raises  an exceptional condition:  a non-local
   exit is performed to THERE.

   When this function is called SRC has been dequeued from GRP: it is up
   to the  expiration handler, or  an exceptional condition  handler, to
   re-enqueue SRC into GRP or not.
*/
{
  src->expiration_handler(there, grp, src);
}

bool
ccevents_source_do_one_event (cce_location_t * there, ccevents_group_t * grp, ccevents_source_t * src)
/* Consume one event  from SRC, if there is a  pending one.  Return TRUE
   if one event was served or the timeout expired; otherwise FALSE.

   Exceptions raised while querying an  event source or serving an event
   will cause a non-local exit to THERE.

   When this function is called SRC has been dequeued from GRP: it is up
   to the handlers,  or an exceptional condition  handler, to re-enqueue
   SRC into GRP or not.
*/
{
  if (ccevents_timeout_expired(&(src->expiration_time))) {
    ccevents_source_handle_expiration(there, grp, src);
    return true;
  } else if (ccevents_source_query(there, grp, src)) {
    ccevents_source_handle_event(there, grp, src);
    return true;
  } else {
    return false;
  }
}

/* end of file */
