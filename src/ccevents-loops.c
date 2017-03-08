/*
  Part of: CCEvents
  Contents: main loop
  Date: Fri Feb 10, 2017

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
 ** Helpers.
 ** ----------------------------------------------------------------- */

__attribute__((pure,nonnull(1),always_inline))
static inline ccevents_group_t *
ccevents_loop_current_group (const ccevents_loop_t * loop)
{
  return (ccevents_group_t *)ccevents_queue_current_node(loop->groups);
}

__attribute__((nonnull(1),always_inline))
static inline void
ccevents_loop_advance_to_next_group (ccevents_loop_t * loop)
{
  ccevents_queue_advance_current(loop->groups);
}

__attribute__((pure,nonnull(1),always_inline))
static inline bool
ccevents_loop_no_request_to_leave (const ccevents_loop_t * loop)
{
  return (loop->request_to_leave)? false : true;
}


/** --------------------------------------------------------------------
 ** Loop functions.
 ** ----------------------------------------------------------------- */

void
ccevents_loop_init (ccevents_loop_t * loop)
{
  ccevents_queue_init(loop->groups);
  loop->request_to_leave	= false;
}

void
ccevents_loop_enter (ccevents_loop_t * loop)
{
  loop->request_to_leave = false;
  {
    while (ccevents_loop_no_request_to_leave(loop) &&
	   ccevents_loop_queue_is_not_empty(loop)) {
      ccevents_group_t *	grp = ccevents_loop_current_group(loop);
      if (grp) {
	/* We need to  remember that whenever we  apply the do_one_event
	   function to a source: the  source's methods might dequeue the
	   group from the loop making the next one the new current.  So,
	   to  avoid  double  advancing,  we advance  now  to  the  next
	   group. */
	ccevents_loop_advance_to_next_group(loop);
	ccevents_group_enter(grp);
      }
    }
  }
  loop->request_to_leave = false;
}

/* end of file */
