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

void
ccevents_queue_init (ccevents_queue_t * Q)
{
  Q->head	= NULL;
  Q->tail	= NULL;
}

bool
ccevents_queue_is_not_empty (const ccevents_queue_t * Q)
{
  return (NULL != Q->head)? true : false;
}

size_t
ccevents_queue_number_of_items (const ccevents_queue_t * Q)
{
  const ccevents_queue_node_t *	N;
  size_t			len = 0;

  for (N = Q->head; N; N = N->next) {
    ++len;
  }
  return len;
}

bool
ccevents_queue_contains_item (const ccevents_queue_t * Q, const ccevents_queue_node_t * N)
{
  const ccevents_queue_node_t *	iter;

  for (iter = Q->head; iter; iter = iter->next) {
    if (iter == N) {
      return true;
    }
  }
  return false;
}

void
ccevents_queue_enqueue (ccevents_queue_t * Q, ccevents_queue_node_t * new_tail)
/* Enqueue NEW_TAIL in the group N. */
{
  if (Q->tail) {
    /* There is at least one item in the list.  Append "new_tail" as the
       new tail. */
    ccevents_queue_node_t *	previous_tail = Q->tail;
    previous_tail->next		= new_tail;
    new_tail->prev		= previous_tail;
    Q->tail	= new_tail;
  } else {
    /* The list is empty.  Fill it with "new_tail" as only item. */
    Q->head	= new_tail;
    Q->tail	= new_tail;
  }
}

ccevents_queue_node_t *
ccevents_queue_dequeue (ccevents_queue_t * Q)
/* Pop an  events source  from the  head of the  list of  events sources
   enqueued for the next run.  Return NULL if the list is empty. */
{
  ccevents_queue_node_t *	next_source = Q->head;
  if (next_source) {
    /* Detach the dequeued source from the group. */
    Q->head		= next_source->next;
    if (Q->head) {
      Q->head->prev	= NULL;
    }
    /* If the dequeued source was the only one: reset the tail too. */
    if (next_source == Q->tail) {
      Q->tail = NULL;
    }
    /* Detach the dequeued source from the list. */
    next_source->prev			= NULL;
    next_source->next			= NULL;
  }
  return next_source;
}

void
ccevents_queue_remove (ccevents_queue_t * Q, ccevents_queue_node_t * N)
{
  if (Q->head == N) {
    /* Detach the source from the head. */
    Q->head = N->next;
    if (Q->head) {
      Q->tail->prev = NULL;
    }
    /* If the removed source was the only one: reset the tail too. */
    if (N == Q->tail) {
      Q->tail = NULL;
    }
  } else if (Q->tail == N) {
    /* Detach the source from the tail. */
    Q->tail = N->prev;
    if (Q->tail) {
      Q->tail->next = NULL;
    }
    /* If the removed source was the only one: reset the head too. */
    if (N == Q->head) {
      Q->head = NULL;
    }
  } else {
    N->prev->next = N->next;
    N->next->prev = N->prev;
  }
  N->prev = NULL;
  N->next = NULL;
}

/* end of file */
