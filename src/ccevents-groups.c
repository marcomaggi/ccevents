/*
  Part of: CCEvents
  Contents: groups of events sources
  Date: Thu Feb  2, 2017

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

void
ccevents_group_init (ccevents_group_t * grp, size_t served_events_watermark)
{
  ccevents_queue_init(&(grp->sources));
  grp->prev			= NULL;
  grp->next			= NULL;
  grp->request_to_leave_asap	= false;
  grp->served_events_watermark	= served_events_watermark;
}

bool
ccevents_group_queue_is_not_empty (const ccevents_group_t * grp)
{
  return ccevents_queue_is_not_empty(&(grp->sources));
}

size_t
ccevents_group_number_of_sources (const ccevents_group_t * grp)
{
  return ccevents_queue_number_of_items(&(grp->sources));
}

bool
ccevents_group_contains_source (const ccevents_group_t * grp, const ccevents_source_t * src)
{
  return ccevents_queue_contains_item (&(grp->sources), src);
}

void
ccevents_group_enqueue_source (ccevents_group_t * grp, ccevents_source_t * new_tail)
{
  ccevents_queue_enqueue (&(grp->sources), new_tail);
}

ccevents_source_t *
ccevents_group_dequeue_source (ccevents_group_t * grp)
{
  return (ccevents_source_t *)ccevents_queue_dequeue(&(grp->sources));
}

void
ccevents_group_remove_source (ccevents_group_t * grp, ccevents_source_t * src)
{
  ccevents_queue_remove(&(grp->sources), src);
}

bool
ccevents_group_run_do_one_event (ccevents_group_t * grp)
/* Dequeue the next  events source and serve one event  from it.  Return
   TRUE if, after  serving one event: more event sources  are waiting to
   be  served; otherwise  return FALSE  if  no more  events sources  are
   waiting.
*/
{
  ccevents_source_t *	next_source = ccevents_group_dequeue_source(grp);
  if (next_source) {
    /* We establish  a location  here to  catch and  discard exceptional
       conditions raised by the events source. */
    cce_location_t	L[1];

    if (cce_location(L)) {
      cce_run_error_handlers(L);
      cce_condition_free(cce_condition(L));
    } else {
      ccevents_source_do_one_event(L, grp, next_source);
      cce_run_cleanup_handlers(L);
    }
  }
  return ccevents_group_queue_is_not_empty(grp);
}

void
ccevents_group_enter (ccevents_group_t * grp)
/* Enter the  loop for this group  and serve events until:  a request to
   exit is  posted; no more  events sources are enqueued;  the watermark
   level has been reached.
*/
{
  size_t	served_events_count = 0;

  /* Set up GRP to start the next run of events servicing.  The run will
     serve at most SERVED_EVENTS_WATERMARK events. */
  {
    grp->request_to_leave_asap	= false;
    served_events_count		= 0;
  }
  /* Enter the  loop and serve events  until: no more event  sources are
     enqueued;  a  request  to  leave  the  loop-out  is  detected;  the
     watermark level has been reached. */
  {
    for (bool one_more_source = true;
	 ((!(grp->request_to_leave_asap)) && one_more_source && (served_events_count < grp->served_events_watermark));
	 one_more_source = ccevents_group_run_do_one_event(grp)) {
      ++(served_events_count);
      if (0) {
	fprintf(stderr, "%s: loop, one_more_source=%d, count=%ld, max=%ld\n",
		__func__, (int)one_more_source, served_events_count,
		grp->served_events_watermark);
      }
    }
  }
  /* Reset GRP after ending a run of events servicing. */
  {
    grp->request_to_leave_asap	= false;
    served_events_count		= 0;
  }
}

void
ccevents_group_post_request_to_leave_asap (ccevents_group_t * grp)
/* Post a request to exit ASAP the loop for this group. */
{
  grp->request_to_leave_asap = true;
}

/* end of file */
