/*
  Part of: CCEvents
  Contents: groups of events sources
  Date: Thu Feb  2, 2017

  Abstract

	Groups are collections of event sources that must be queried for
	pending events with the same priority.  Multiple groups are used
	to establish a  priority order between sets  of sources.  Events
	from higher priority groups should  be served before events from
	lower priority groups.

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
 ** Helpers.
 ** ----------------------------------------------------------------- */

__attribute__((pure,nonnull(1),always_inline))
static inline ccevents_source_t *
ccevents_group_current_source (ccevents_group_t * grp)
{
  return (ccevents_source_t *)ccevents_queue_current_node(grp->sources);
}

__attribute__((nonnull(1),always_inline))
static inline void
ccevents_group_advance_to_next_source (ccevents_group_t * grp)
{
  ccevents_queue_advance_current(grp->sources);
}

__attribute__((pure,nonnull(1),always_inline))
static inline bool
ccevents_group_no_request_to_leave_asap (ccevents_group_t * grp)
{
  return (grp->request_to_leave_asap)? false : true;
}

__attribute__((nonnull(1),always_inline))
static inline bool
ccevents_group_servicing_attempts_watermark_not_reached (ccevents_group_t * grp, size_t servicing_atempts_count)
{
  return (servicing_atempts_count < grp->servicing_attempts_watermark);
}


/** --------------------------------------------------------------------
 ** Group functions.
 ** ----------------------------------------------------------------- */

void
ccevents_group_init (ccevents_group_t * grp, size_t servicing_attempts_watermark)
{
  ccevents_queue_node_init(grp);
  ccevents_queue_init(grp->sources);
  grp->request_to_leave_asap		= false;
  grp->servicing_attempts_watermark	= servicing_attempts_watermark;
}

void
ccevents_group_enter (ccevents_group_t * grp)
/* Enter the  loop for this group  and serve events until:  a request to
   exit is  posted; no more  events sources are enqueued;  the watermark
   level has been reached.
*/
{
  grp->request_to_leave_asap	= false;
  {
    volatile size_t	servicing_attempts_count = 0;

    while (ccevents_group_no_request_to_leave_asap(grp) &&
	   ccevents_group_queue_is_not_empty(grp)       &&
	   ccevents_group_servicing_attempts_watermark_not_reached(grp, servicing_attempts_count)) {
      ccevents_source_t *	next_source = ccevents_group_current_source(grp);
      if (next_source) {
	cce_location_t	L[1];

	/* We need to  remember that whenever we  apply the do_one_event
	   function to a source: the  source's methods might dequeue the
	   source  itself from  the group  making the  next one  the new
	   current.  So,  to avoid double  advancing, we advance  now to
	   the next source. */
	ccevents_group_advance_to_next_source(grp);

	++servicing_attempts_count;

	if (cce_location(L)) {
	  cce_run_error_handlers(L);
	  cce_condition_free(cce_condition(L));
	} else {
	  ccevents_source_do_one_event(L, next_source);
	  cce_run_cleanup_handlers(L);
	}
      }
    }
  }
  grp->request_to_leave_asap	= false;
}

/* end of file */
