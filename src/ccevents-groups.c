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
ccevents_group_init (ccevents_group_t * grp)
{
  grp->sources_queue_head	= NULL;
  grp->sources_queue_tail	= NULL;
  grp->request_to_leave_asap	= false;
  grp->served_events_count	= 0;
  grp->served_events_watermark	= 0;
}

bool
ccevents_group_queue_is_not_empty (const ccevents_group_t * grp)
{
  return (grp->sources_queue_head)? true : false;
}

size_t
ccevents_group_number_of_sources (const ccevents_group_t * grp)
{
  ccevents_source_t *	src;
  size_t		len = 0;

  for (src = grp->sources_queue_head; src; src = src->next) {
    ++len;
  }
  return len;
}

bool
ccevents_group_contains_source (const ccevents_group_t * grp, const ccevents_source_t * src)
{
  ccevents_source_t *	iter;

  for (iter = grp->sources_queue_head; iter; iter = iter->next) {
    if (iter == src) {
      return true;
    }
  }
  return false;
}

void
ccevents_group_enqueue_source (ccevents_group_t * grp, ccevents_source_t * new_tail)
/* Enqueue NEW_TAIL in the group SRC. */
{
  if (grp->sources_queue_tail) {
    /* There is at least one item in the list.  Append "new_tail" as the
       new tail. */
    ccevents_source_t *	previous_tail = grp->sources_queue_tail;
    previous_tail->next		= new_tail;
    new_tail->prev		= previous_tail;
    grp->sources_queue_tail	= new_tail;
  } else {
    /* The list is empty.  Fill it with "new_tail" as only item. */
    grp->sources_queue_head	= new_tail;
    grp->sources_queue_tail	= new_tail;
  }
}

ccevents_source_t *
ccevents_group_dequeue_source (ccevents_group_t * grp)
/* Pop an  events source  from the  head of the  list of  events sources
   enqueued for the next run.  Return NULL if the list is empty. */
{
  ccevents_source_t *	next_source = grp->sources_queue_head;
  if (next_source) {
    /* Detach the dequeued source from the group. */
    grp->sources_queue_head		= next_source->next;
    if (grp->sources_queue_head) {
      grp->sources_queue_head->prev	= NULL;
    }
    /* If the dequeued source was the only one: reset the tail too. */
    if (next_source == grp->sources_queue_tail) {
      grp->sources_queue_tail = NULL;
    }
    /* Detach the dequeued source from the list. */
    next_source->prev			= NULL;
    next_source->next			= NULL;
  }
  return next_source;
}

void
ccevents_group_remove_source (ccevents_group_t * grp, ccevents_source_t * src)
{
  if (grp->sources_queue_head == src) {
    /* Detach the source from the head. */
    grp->sources_queue_head = src->next;
    if (grp->sources_queue_head) {
      grp->sources_queue_tail->prev = NULL;
    }
    /* If the removed source was the only one: reset the tail too. */
    if (src == grp->sources_queue_tail) {
      grp->sources_queue_tail = NULL;
    }
  } else if (grp->sources_queue_tail == src) {
    /* Detach the source from the tail. */
    grp->sources_queue_tail = src->prev;
    if (grp->sources_queue_tail) {
      grp->sources_queue_tail->next = NULL;
    }
    /* If the removed source was the only one: reset the head too. */
    if (src == grp->sources_queue_head) {
      grp->sources_queue_head = NULL;
    }
  } else {
    src->prev->next = src->next;
    src->next->prev = src->prev;
  }
  src->prev = NULL;
  src->next = NULL;
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
      cce_condition_free(cce_location_condition(L));
    } else {
      ccevents_source_do_one_event(L, grp, next_source);
      cce_run_cleanup_handlers(L);
    }
  }
  return ccevents_group_queue_is_not_empty(grp);
}

void
ccevents_group_enter (ccevents_group_t * grp, size_t served_events_watermark)
/* Enter the  loop for this group  and serve events until:  a request to
   exit is  posted; no more  events sources are enqueued;  the watermark
   level has been reached.
*/
{
  /* Set up GRP to start the next run of events servicing.  The run will
     serve at most SERVED_EVENTS_WATERMARK events. */
  {
    grp->request_to_leave_asap		= false;
    grp->served_events_count		= 0;
    grp->served_events_watermark	= served_events_watermark;
  }
  /* Enter the  loop and serve events  until: no more event  sources are
     enqueued;  a  request  to  leave  the  loop-out  is  detected;  the
     watermark level has been reached. */
  {
    for (bool one_more_source = true;
	 ((!(grp->request_to_leave_asap)) && one_more_source && (grp->served_events_count < grp->served_events_watermark));
	 one_more_source = ccevents_group_run_do_one_event(grp)) {
      ++(grp->served_events_count);
      if (0) {
	fprintf(stderr, "%s: loop, one_more_source=%d, count=%ld, max=%ld\n",
		__func__, (int)one_more_source, grp->served_events_count,
		grp->served_events_watermark);
      }
    }
  }
  /* Reset GRP after ending a run of events servicing. */
  {
    grp->request_to_leave_asap		= false;
    grp->served_events_count		= 0;
    grp->served_events_watermark	= 0;
  }
}

void
ccevents_group_post_request_to_leave_asap (ccevents_group_t * grp)
/* Post a request to exit ASAP the loop for this group. */
{
  grp->request_to_leave_asap = true;
}

/* end of file */
