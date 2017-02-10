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

void
ccevents_loop_init (ccevents_loop_t * loop)
{
  ccevents_queue_init(&(loop->groups));
  loop->request_to_leave_asap	= false;
}

bool
ccevents_loop_queue_is_not_empty (const ccevents_loop_t * loop)
{
  return ccevents_queue_is_not_empty(&(loop->groups));
}

size_t
ccevents_loop_number_of_groups (const ccevents_loop_t * loop)
{
  return ccevents_queue_number_of_items(&(loop->groups));
}

bool
ccevents_loop_contains_group (const ccevents_loop_t * loop, const ccevents_group_t * grp)
{
  return ccevents_queue_contains_item (&(loop->groups), grp);
}

void
ccevents_loop_enqueue_group (ccevents_loop_t * loop, ccevents_group_t * new_tail)
{
  ccevents_queue_enqueue (&(loop->groups), new_tail);
}

ccevents_group_t *
ccevents_loop_dequeue_group (ccevents_loop_t * loop)
{
  return (ccevents_group_t *)ccevents_queue_dequeue(&(loop->groups));
}

void
ccevents_loop_remove_group (ccevents_loop_t * loop, ccevents_group_t * grp)
{
  ccevents_queue_remove(&(loop->groups), grp);
}

void
ccevents_loop_enter (ccevents_loop_t * loop)
{
  loop->request_to_leave_asap = false;
  {
    ccevents_group_t *	grp;
    for (grp = (ccevents_group_t *)loop->groups.head;
	 grp && (!(loop->request_to_leave_asap));
	 grp = (ccevents_group_t *)grp->next) {
      ccevents_group_enter(grp);
    }
  }
  loop->request_to_leave_asap = false;
}

void
ccevents_loop_post_request_to_leave_asap (ccevents_loop_t * loop)
/* Post a request to exit ASAP the loop for this loop. */
{
  loop->request_to_leave_asap = true;
}

/* end of file */
