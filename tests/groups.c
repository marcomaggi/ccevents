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

  ccevents_group_init(G, 10);

  assert(false == ccevents_source_is_enqueued(ccevents_source(A)));
  assert(false == ccevents_source_is_enqueued(ccevents_source(B)));
  assert(false == ccevents_source_is_enqueued(ccevents_source(C)));
  assert(false == ccevents_source_is_enqueued(ccevents_source(D)));

  assert(false == ccevents_group_contains_source(G, ccevents_source(A)));
  assert(false == ccevents_group_contains_source(G, ccevents_source(B)));
  assert(false == ccevents_group_contains_source(G, ccevents_source(C)));
  assert(false == ccevents_group_contains_source(G, ccevents_source(D)));

  assert(0 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, ccevents_source(A));
  assert(1 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, ccevents_source(B));
  assert(2 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, ccevents_source(C));
  assert(3 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, ccevents_source(D));
  assert(4 == ccevents_group_number_of_sources(G));

  assert(true == ccevents_source_is_enqueued(ccevents_source(A)));
  assert(true == ccevents_source_is_enqueued(ccevents_source(B)));
  assert(true == ccevents_source_is_enqueued(ccevents_source(C)));
  assert(true == ccevents_source_is_enqueued(ccevents_source(D)));

  assert(true == ccevents_group_contains_source(G, ccevents_source(A)));
  assert(true == ccevents_group_contains_source(G, ccevents_source(B)));
  assert(true == ccevents_group_contains_source(G, ccevents_source(C)));
  assert(true == ccevents_group_contains_source(G, ccevents_source(D)));

  ccevents_source_dequeue_itself(ccevents_source(A));
  assert(3 == ccevents_group_number_of_sources(G));
  ccevents_source_dequeue_itself(ccevents_source(B));
  assert(2 == ccevents_group_number_of_sources(G));
  ccevents_source_dequeue_itself(ccevents_source(C));
  assert(1 == ccevents_group_number_of_sources(G));
  ccevents_source_dequeue_itself(ccevents_source(D));
  assert(0 == ccevents_group_number_of_sources(G));

  assert(false == ccevents_source_is_enqueued(ccevents_source(A)));
  assert(false == ccevents_source_is_enqueued(ccevents_source(B)));
  assert(false == ccevents_source_is_enqueued(ccevents_source(C)));
  assert(false == ccevents_source_is_enqueued(ccevents_source(D)));

  assert(false == ccevents_group_contains_source(G, ccevents_source(A)));
  assert(false == ccevents_group_contains_source(G, ccevents_source(B)));
  assert(false == ccevents_group_contains_source(G, ccevents_source(C)));
  assert(false == ccevents_group_contains_source(G, ccevents_source(D)));
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

  ccevents_group_init(G, 10);

  assert(false == ccevents_source_is_enqueued(ccevents_source(A)));
  assert(false == ccevents_source_is_enqueued(ccevents_source(B)));
  assert(false == ccevents_source_is_enqueued(ccevents_source(C)));
  assert(false == ccevents_source_is_enqueued(ccevents_source(D)));

  assert(false == ccevents_group_contains_source(G, ccevents_source(A)));
  assert(false == ccevents_group_contains_source(G, ccevents_source(B)));
  assert(false == ccevents_group_contains_source(G, ccevents_source(C)));
  assert(false == ccevents_group_contains_source(G, ccevents_source(D)));

  assert(0 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, ccevents_source(A));
  assert(1 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, ccevents_source(B));
  assert(2 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, ccevents_source(C));
  assert(3 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, ccevents_source(D));
  assert(4 == ccevents_group_number_of_sources(G));

  assert(true == ccevents_source_is_enqueued(ccevents_source(A)));
  assert(true == ccevents_source_is_enqueued(ccevents_source(B)));
  assert(true == ccevents_source_is_enqueued(ccevents_source(C)));
  assert(true == ccevents_source_is_enqueued(ccevents_source(D)));

  assert(true == ccevents_group_contains_source(G, ccevents_source(A)));
  assert(true == ccevents_group_contains_source(G, ccevents_source(B)));
  assert(true == ccevents_group_contains_source(G, ccevents_source(C)));
  assert(true == ccevents_group_contains_source(G, ccevents_source(D)));

  ccevents_source_dequeue_itself(ccevents_source(D));
  assert(3 == ccevents_group_number_of_sources(G));
  ccevents_source_dequeue_itself(ccevents_source(C));
  assert(2 == ccevents_group_number_of_sources(G));
  ccevents_source_dequeue_itself(ccevents_source(B));
  assert(1 == ccevents_group_number_of_sources(G));
  ccevents_source_dequeue_itself(ccevents_source(A));
  assert(0 == ccevents_group_number_of_sources(G));

  assert(false == ccevents_source_is_enqueued(ccevents_source(A)));
  assert(false == ccevents_source_is_enqueued(ccevents_source(B)));
  assert(false == ccevents_source_is_enqueued(ccevents_source(C)));
  assert(false == ccevents_source_is_enqueued(ccevents_source(D)));

  assert(false == ccevents_group_contains_source(G, ccevents_source(A)));
  assert(false == ccevents_group_contains_source(G, ccevents_source(B)));
  assert(false == ccevents_group_contains_source(G, ccevents_source(C)));
  assert(false == ccevents_group_contains_source(G, ccevents_source(D)));
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

  ccevents_group_init(G, 10);

  assert(false == ccevents_source_is_enqueued(ccevents_source(A)));
  assert(false == ccevents_source_is_enqueued(ccevents_source(B)));
  assert(false == ccevents_source_is_enqueued(ccevents_source(C)));
  assert(false == ccevents_source_is_enqueued(ccevents_source(D)));

  assert(false == ccevents_group_contains_source(G, ccevents_source(A)));
  assert(false == ccevents_group_contains_source(G, ccevents_source(B)));
  assert(false == ccevents_group_contains_source(G, ccevents_source(C)));
  assert(false == ccevents_group_contains_source(G, ccevents_source(D)));

  assert(0 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, ccevents_source(A));
  assert(1 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, ccevents_source(B));
  assert(2 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, ccevents_source(C));
  assert(3 == ccevents_group_number_of_sources(G));
  ccevents_group_enqueue_source(G, ccevents_source(D));
  assert(4 == ccevents_group_number_of_sources(G));

  assert(true == ccevents_source_is_enqueued(ccevents_source(A)));
  assert(true == ccevents_source_is_enqueued(ccevents_source(B)));
  assert(true == ccevents_source_is_enqueued(ccevents_source(C)));
  assert(true == ccevents_source_is_enqueued(ccevents_source(D)));

  assert(true == ccevents_group_contains_source(G, ccevents_source(A)));
  assert(true == ccevents_group_contains_source(G, ccevents_source(B)));
  assert(true == ccevents_group_contains_source(G, ccevents_source(C)));
  assert(true == ccevents_group_contains_source(G, ccevents_source(D)));

  ccevents_source_dequeue_itself(ccevents_source(B));
  assert(3 == ccevents_group_number_of_sources(G));
  ccevents_source_dequeue_itself(ccevents_source(C));
  assert(2 == ccevents_group_number_of_sources(G));
  ccevents_source_dequeue_itself(ccevents_source(A));
  assert(1 == ccevents_group_number_of_sources(G));
  ccevents_source_dequeue_itself(ccevents_source(D));
  assert(0 == ccevents_group_number_of_sources(G));

  assert(false == ccevents_source_is_enqueued(ccevents_source(A)));
  assert(false == ccevents_source_is_enqueued(ccevents_source(B)));
  assert(false == ccevents_source_is_enqueued(ccevents_source(C)));
  assert(false == ccevents_source_is_enqueued(ccevents_source(D)));

  assert(false == ccevents_group_contains_source(G, ccevents_source(A)));
  assert(false == ccevents_group_contains_source(G, ccevents_source(B)));
  assert(false == ccevents_group_contains_source(G, ccevents_source(C)));
  assert(false == ccevents_group_contains_source(G, ccevents_source(D)));
}


volatile bool test_4_0_finalisation_A = false;
volatile bool test_4_0_finalisation_B = false;
volatile bool test_4_0_finalisation_C = false;
volatile bool test_4_0_finalisation_D = false;

static void
test_4_0_source_finalise_A (ccevents_source_t * src CCEVENTS_UNUSED)
{
  test_4_0_finalisation_A = true;
}

static void
test_4_0_source_finalise_B (ccevents_source_t * src CCEVENTS_UNUSED)
{
  test_4_0_finalisation_B = true;
}

static void
test_4_0_source_finalise_C (ccevents_source_t * src CCEVENTS_UNUSED)
{
  test_4_0_finalisation_C = true;
}

static void
test_4_0_source_finalise_D (ccevents_source_t * src CCEVENTS_UNUSED)
{
  test_4_0_finalisation_D = true;
}

void
test_4_0_finalisation_of_sources (void)
{
  ccevents_group_t		G[1];
  ccevents_task_source_t	A[1], B[1], C[1], D[1];

  const ccevents_source_otable_t otableA = { .final = test_4_0_source_finalise_A };
  const ccevents_source_otable_t otableB = { .final = test_4_0_source_finalise_B };
  const ccevents_source_otable_t otableC = { .final = test_4_0_source_finalise_C };
  const ccevents_source_otable_t otableD = { .final = test_4_0_source_finalise_D };

  ccevents_task_source_init(A);
  ccevents_task_source_init(B);
  ccevents_task_source_init(C);
  ccevents_task_source_init(D);

  ccevents_source_set_otable(ccevents_source(A), &otableA);
  ccevents_source_set_otable(ccevents_source(B), &otableB);
  ccevents_source_set_otable(ccevents_source(C), &otableC);
  ccevents_source_set_otable(ccevents_source(D), &otableD);

  ccevents_group_init(G, 10);

  ccevents_group_enqueue_source(G, ccevents_source(A));
  ccevents_group_enqueue_source(G, ccevents_source(B));
  ccevents_group_enqueue_source(G, ccevents_source(C));
  ccevents_group_enqueue_source(G, ccevents_source(D));

  ccevents_group_final(G);

  assert(true == test_4_0_finalisation_A);
  assert(true == test_4_0_finalisation_B);
  assert(true == test_4_0_finalisation_C);
  assert(true == test_4_0_finalisation_D);
}


int
main (int argc CCEVENTS_UNUSED, const char *const argv[] CCEVENTS_UNUSED)
{
  ccevents_init();
  if (1) test_sources_removal_from_head();
  if (1) test_sources_removal_from_tail();
  if (1) test_sources_removal_from_middle();
  if (1) test_4_0_finalisation_of_sources();
  exit(EXIT_SUCCESS);
}

/* end of file */
