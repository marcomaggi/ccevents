/*
  Part of: CCEvents
  Contents: tests for groups of events sources
  Date: Mon Feb  6, 2017

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

#include <ccevents.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void
test_sources_removal_from_head (void)
{
  ccevents_group_t		G[1];
  ccevents_task_source_t	A[1], B[1], C[1], D[1];

  ccevents_task_source_init(A);
  ccevents_task_source_init(B);
  ccevents_task_source_init(C);
  ccevents_task_source_init(D);

  ccevents_group_init(G);

  assert(false == ccevents_source_is_enqueued(A));
  assert(false == ccevents_source_is_enqueued(B));
  assert(false == ccevents_source_is_enqueued(C));
  assert(false == ccevents_source_is_enqueued(D));

  assert(false == ccevents_group_contains_source(G, A));
  assert(false == ccevents_group_contains_source(G, B));
  assert(false == ccevents_group_contains_source(G, C));
  assert(false == ccevents_group_contains_source(G, D));

  assert(0 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, A);
  assert(1 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, B);
  assert(2 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, C);
  assert(3 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, D);
  assert(4 == ccevents_group_number_of_sources(G));

  assert(true == ccevents_source_is_enqueued(A));
  assert(true == ccevents_source_is_enqueued(B));
  assert(true == ccevents_source_is_enqueued(C));
  assert(true == ccevents_source_is_enqueued(D));

  assert(true == ccevents_group_contains_source(G, A));
  assert(true == ccevents_group_contains_source(G, B));
  assert(true == ccevents_group_contains_source(G, C));
  assert(true == ccevents_group_contains_source(G, D));

  ccevents_group_remove_source(G, A);
  assert(3 == ccevents_group_number_of_sources(G));
  ccevents_group_remove_source(G, B);
  assert(2 == ccevents_group_number_of_sources(G));
  ccevents_group_remove_source(G, C);
  assert(1 == ccevents_group_number_of_sources(G));
  ccevents_group_remove_source(G, D);
  assert(0 == ccevents_group_number_of_sources(G));

  assert(false == ccevents_source_is_enqueued(A));
  assert(false == ccevents_source_is_enqueued(B));
  assert(false == ccevents_source_is_enqueued(C));
  assert(false == ccevents_source_is_enqueued(D));

  assert(false == ccevents_group_contains_source(G, A));
  assert(false == ccevents_group_contains_source(G, B));
  assert(false == ccevents_group_contains_source(G, C));
  assert(false == ccevents_group_contains_source(G, D));
}

static void
test_sources_removal_from_tail (void)
{
  ccevents_group_t		G[1];
  ccevents_task_source_t	A[1], B[1], C[1], D[1];

  ccevents_task_source_init(A);
  ccevents_task_source_init(B);
  ccevents_task_source_init(C);
  ccevents_task_source_init(D);

  ccevents_group_init(G);

  assert(false == ccevents_source_is_enqueued(A));
  assert(false == ccevents_source_is_enqueued(B));
  assert(false == ccevents_source_is_enqueued(C));
  assert(false == ccevents_source_is_enqueued(D));

  assert(false == ccevents_group_contains_source(G, A));
  assert(false == ccevents_group_contains_source(G, B));
  assert(false == ccevents_group_contains_source(G, C));
  assert(false == ccevents_group_contains_source(G, D));

  assert(0 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, A);
  assert(1 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, B);
  assert(2 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, C);
  assert(3 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, D);
  assert(4 == ccevents_group_number_of_sources(G));

  assert(true == ccevents_source_is_enqueued(A));
  assert(true == ccevents_source_is_enqueued(B));
  assert(true == ccevents_source_is_enqueued(C));
  assert(true == ccevents_source_is_enqueued(D));

  assert(true == ccevents_group_contains_source(G, A));
  assert(true == ccevents_group_contains_source(G, B));
  assert(true == ccevents_group_contains_source(G, C));
  assert(true == ccevents_group_contains_source(G, D));

  ccevents_group_remove_source(G, D);
  assert(3 == ccevents_group_number_of_sources(G));
  ccevents_group_remove_source(G, C);
  assert(2 == ccevents_group_number_of_sources(G));
  ccevents_group_remove_source(G, B);
  assert(1 == ccevents_group_number_of_sources(G));
  ccevents_group_remove_source(G, A);
  assert(0 == ccevents_group_number_of_sources(G));

  assert(false == ccevents_source_is_enqueued(A));
  assert(false == ccevents_source_is_enqueued(B));
  assert(false == ccevents_source_is_enqueued(C));
  assert(false == ccevents_source_is_enqueued(D));

  assert(false == ccevents_group_contains_source(G, A));
  assert(false == ccevents_group_contains_source(G, B));
  assert(false == ccevents_group_contains_source(G, C));
  assert(false == ccevents_group_contains_source(G, D));
}

static void
test_sources_removal_from_middle (void)
{
  ccevents_group_t		G[1];
  ccevents_task_source_t	A[1], B[1], C[1], D[1];

  ccevents_task_source_init(A);
  ccevents_task_source_init(B);
  ccevents_task_source_init(C);
  ccevents_task_source_init(D);

  ccevents_group_init(G);

  assert(false == ccevents_source_is_enqueued(A));
  assert(false == ccevents_source_is_enqueued(B));
  assert(false == ccevents_source_is_enqueued(C));
  assert(false == ccevents_source_is_enqueued(D));

  assert(false == ccevents_group_contains_source(G, A));
  assert(false == ccevents_group_contains_source(G, B));
  assert(false == ccevents_group_contains_source(G, C));
  assert(false == ccevents_group_contains_source(G, D));

  assert(0 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, A);
  assert(1 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, B);
  assert(2 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, C);
  assert(3 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, D);
  assert(4 == ccevents_group_number_of_sources(G));

  assert(true == ccevents_source_is_enqueued(A));
  assert(true == ccevents_source_is_enqueued(B));
  assert(true == ccevents_source_is_enqueued(C));
  assert(true == ccevents_source_is_enqueued(D));

  assert(true == ccevents_group_contains_source(G, A));
  assert(true == ccevents_group_contains_source(G, B));
  assert(true == ccevents_group_contains_source(G, C));
  assert(true == ccevents_group_contains_source(G, D));

  ccevents_group_remove_source(G, B);
  assert(3 == ccevents_group_number_of_sources(G));
  ccevents_group_remove_source(G, C);
  assert(2 == ccevents_group_number_of_sources(G));
  ccevents_group_remove_source(G, A);
  assert(1 == ccevents_group_number_of_sources(G));
  ccevents_group_remove_source(G, D);
  assert(0 == ccevents_group_number_of_sources(G));

  assert(false == ccevents_source_is_enqueued(A));
  assert(false == ccevents_source_is_enqueued(B));
  assert(false == ccevents_source_is_enqueued(C));
  assert(false == ccevents_source_is_enqueued(D));

  assert(false == ccevents_group_contains_source(G, A));
  assert(false == ccevents_group_contains_source(G, B));
  assert(false == ccevents_group_contains_source(G, C));
  assert(false == ccevents_group_contains_source(G, D));
}

int
main (int argc CCEVENTS_UNUSED, const char *const argv[] CCEVENTS_UNUSED)
{
  if (1) test_sources_removal_from_head();
  if (1) test_sources_removal_from_tail();
  if (1) test_sources_removal_from_middle();
  exit(EXIT_SUCCESS);
}

/* end of file */
