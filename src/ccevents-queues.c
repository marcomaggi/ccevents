/*
  Part of: CCEvents
  Contents: queues
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

__attribute__((nonnull(1),always_inline))
static inline void
link_nodes_prev_next (ccevents_queue_node_t * prev, ccevents_queue_node_t * next)
{
  prev->next	= next;
  next->prev	= prev;
}


/** --------------------------------------------------------------------
 ** Queue functions.
 ** ----------------------------------------------------------------- */

void
ccevents_queue_init (ccevents_queue_t * Q)
{
  Q->current		= NULL;
  Q->number_of_nodes	= 0;
}

void
ccevents_queue_advance_current (ccevents_queue_t * Q)
{
  if (ccevents_queue_is_not_empty(Q)) {
    Q->current = Q->current->next;
  }
}

void
ccevents_queue_enqueue (ccevents_queue_t * Q, ccevents_queue_node_t * new_current)
/* Enqueue NEW_CURRENT in the queue Q.
 *
 *     Before:                                After:
 *
 *     -------------                        -------------
 *    |   prev      |                      |   prev      |
 *     -------------                       -------------
 *          | prev->next                         | prev->next
 *          v                                    v
 *     -------------                        -------------
 *    | old_current |                      | new_current |
 *     -------------                        -------------
 *                                               | new_current->next
 *                                               v
 *                                          -------------
 *                                         | old_current |
 *                                          -------------
 *
 */
{
  if (ccevents_queue_is_not_empty(Q)) {
    /* The queue is not empty. */
    ccevents_queue_node_t *	old_current = Q->current;
    ccevents_queue_node_t *	prev        = old_current->prev;
    link_nodes_prev_next(new_current, old_current);
    link_nodes_prev_next(prev,        new_current);
  } else {
    /* The queue is empty. */
    link_nodes_prev_next(new_current, new_current);
  }
  new_current->base	= Q;
  Q->current		= new_current;
  ++(Q->number_of_nodes);
}

ccevents_queue_node_t *
ccevents_queue_dequeue (ccevents_queue_t * Q)
/* Remove the  current node from the  queue and return a  pointer to it;
   return NULL if the queue is empty. */
{
  ccevents_queue_node_t *	old_current = Q->current;
  if (old_current) {
    /* Detach the queue from the node. */
    --(Q->number_of_nodes);
    if (Q->number_of_nodes) {
      /* There is at least one other node in the queue. */
      Q->current		= old_current->next;
      old_current->next->prev	= old_current->prev;
      old_current->prev->next	= old_current->next;
    } else {
      /* The old current is the only node in the queue. */
      Q->current	= NULL;
    }
    /* Detach the dequeued node from the queue. */
    ccevents_queue_node_init(old_current);
  }
  return old_current;
}


/** --------------------------------------------------------------------
 ** Node functions.
 ** ----------------------------------------------------------------- */

void
ccevents_queue_node_init (ccevents_queue_node_t * N)
{
  N->base	= NULL;
  N->next	= NULL;
  N->prev	= NULL;
}

void
ccevents_queue_node_dequeue_itself (ccevents_queue_node_t * N)
/* Remove the node from its queue.  If the node is not eneueued: nothing
   happens. */
{
  ccevents_queue_t *	Q = N->base;
  if (Q) {
    if (ccevents_queue_current_node(Q) == N) {
      /* The node is the current one.  Special handling. */
      ccevents_queue_dequeue(Q);
    } else {
      /* This node is not the current.  Just extract it. */
      N->next->prev	= N->prev;
      N->prev->next	= N->next;
      --(Q->number_of_nodes);
      N->base	= NULL;
      N->next	= NULL;
      N->prev	= NULL;
    }
  }
}

/* end of file */
