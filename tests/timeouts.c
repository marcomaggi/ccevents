/*
  Part of: CCEvents
  Contents: tests for timeouts API
  Date: Tue Jan 17, 2017

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

#define _GNU_SOURCE	1
#include <ccevents.h>
#include <ccexceptions.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* The purpose of this macro is to use TO so that the compiler will: not
   raise  warning  for  unused  variables; not  remove  pure  code  that
   discards return values. */
#define PRINTO(TO)	fprintf(stderr, "%ld,%ld,%ld",			\
				ccevents_timeout_seconds(TO),		\
				ccevents_timeout_milliseconds(TO),	\
				ccevents_timeout_microseconds(TO));

static void
test_timeout_initialisation (void)
{
  cce_location_t	L[1];
  ccevents_timeout_t	to;
  bool			error_flag = false;

  if (cce_location(L)) {
    error_flag = true;
    cce_run_error_handlers_final(L);
  } else {
    to = ccevents_timeout_init(L, 1, 2, 3);
    cce_run_cleanup_handlers(L);
  }
  assert(false == error_flag);
  assert(1 == to.seconds);
  assert(2 == to.milliseconds);
  assert(3 == to.microseconds);
}
static void
test_timeout_initialisation_correct_milliseconds_overflow (void)
{
  /* Positive milliseconds overflow. */
  if (1) {
    cce_location_t	L[1];
    ccevents_timeout_t	to;
    bool		error_flag = false;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      to = ccevents_timeout_init(L, 0, 1001, 0);
      cce_run_cleanup_handlers(L);
    }
    assert(false == error_flag);
    assert(1 == to.seconds);
    assert(1 == to.milliseconds);
    assert(0 == to.microseconds);
  }
  /* LONG_MAX milliseconds overflow. */
  if (1) {
    cce_location_t	L[1];
    ccevents_timeout_t	to;
    bool		error_flag = false;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      to = ccevents_timeout_init(L, 0, LONG_MAX, 0);
      cce_run_cleanup_handlers(L);
    }
    assert(false == error_flag);
    assert(9223372036854775 == to.seconds);
    assert(807 == to.milliseconds);
    assert(0 == to.microseconds);
  }
  /* Negative milliseconds distribution. */
  if (1) {
    cce_location_t	L[1];
    ccevents_timeout_t	to;
    bool		error_flag = false;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      to = ccevents_timeout_init(L, 1, -100, 0);
      cce_run_cleanup_handlers(L);
    }
    assert(false == error_flag);
    assert(0 == to.seconds);
    assert(900 == to.milliseconds);
    assert(0 == to.microseconds);
  }
  /* LONG_MIN milliseconds distribution. */
  if (1) {
    cce_location_t	L[1];
    ccevents_timeout_t	to;
    bool		error_flag = false;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      to = ccevents_timeout_init(L, LONG_MAX-1000, LONG_MIN, 0);
      cce_run_cleanup_handlers(L);
    }
    assert(false == error_flag);
    //fprintf(stderr, "secs = %ld, msecs = %ld, usecs = %ld\n", to.seconds, to.milliseconds, to.microseconds);
    assert(9214148664817920031 == to.seconds);
    assert(192 == to.milliseconds);
    assert(0 == to.microseconds);
  }
}

static void
test_timeout_initialisation_correct_microseconds_overflow (void)
{
  /* Positive microseconds overflow. */
  if (1) {
    cce_location_t	L[1];
    ccevents_timeout_t	to;
    bool		error_flag = false;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      to = ccevents_timeout_init(L, 0, 0, 1001);
      cce_run_cleanup_handlers(L);
    }
    assert(false == error_flag);
    //fprintf(stderr, "secs = %ld, msecs = %ld, usecs = %ld\n", to.seconds, to.milliseconds, to.microseconds);
    assert(0 == to.seconds);
    assert(1 == to.milliseconds);
    assert(1 == to.microseconds);
  }
  /* Negative microseconds distribution. */
  if (1) {
    cce_location_t	L[1];
    ccevents_timeout_t	to;
    bool		error_flag = false;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      to = ccevents_timeout_init(L, 0, 1, -100);
      cce_run_cleanup_handlers(L);
    }
    assert(false == error_flag);
    //fprintf(stderr, "secs = %ld, msecs = %ld, usecs = %ld\n", to.seconds, to.milliseconds, to.microseconds);
    assert(0 == to.seconds);
    assert(0 == to.milliseconds);
    assert(900 == to.microseconds);
  }
  /* Negative microseconds distribution. */
  if (1) {
    cce_location_t	L[1];
    ccevents_timeout_t	to;
    bool		error_flag = false;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      to = ccevents_timeout_init(L, 0, 1, -100);
      cce_run_cleanup_handlers(L);
    }
    assert(false == error_flag);
    //fprintf(stderr, "secs = %ld, msecs = %ld, usecs = %ld\n", to.seconds, to.milliseconds, to.microseconds);
    assert(0 == to.seconds);
    assert(0 == to.milliseconds);
    assert(900 == to.microseconds);
  }
  /* LONG_MIN milliseconds distribution. */
  if (1) {
    cce_location_t	L[1];
    ccevents_timeout_t	to;
    bool		error_flag = false;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      to = ccevents_timeout_init(L, 0, LONG_MAX, LONG_MIN);
      cce_run_cleanup_handlers(L);
    }
    assert(false == error_flag);
    //fprintf(stderr, "secs = %ld, msecs = %ld, usecs = %ld\n", to.seconds, to.milliseconds, to.microseconds);
    assert(9214148664817921 == to.seconds);
    assert(31 == to.milliseconds);
    assert(192 == to.microseconds);
  }
}

static void
test_timeout_initialisation_double_microseconds_overflow (void)
/* The microseconds value  overflows into both the  milliseconds and the
   seconds fields. */
{
  cce_location_t	L[1];
  ccevents_timeout_t	to;
  bool			error_flag = false;

  if (cce_location(L)) {
    error_flag = true;
    cce_run_error_handlers_final(L);
  } else {
    to = ccevents_timeout_init(L, 0, 0, 1001001);
    cce_run_cleanup_handlers(L);
  }
  assert(false == error_flag);
  //fprintf(stderr, "secs = %ld, msecs = %ld, usecs = %ld\n", to.seconds, to.milliseconds, to.microseconds);
  assert(1 == to.seconds);
  assert(1 == to.milliseconds);
  assert(1 == to.microseconds);
}
static void
test_condition_invalid_milliseconds_overflow (void)
{
  cce_location_t	L[1];
  ccevents_timeout_t	to;
  bool			error_flag = false;

  if (cce_location(L)) {
    error_flag = true;
    assert(true == ccevents_condition_is_timeout_overflow(cce_condition(L)));
    cce_run_error_handlers_final(L);
  } else {
    to = ccevents_timeout_init(L, LONG_MAX - (LONG_MAX / 1000) + 1, LONG_MAX, 0);
    PRINTO(to);
    cce_run_cleanup_handlers(L);
  }
  //fprintf(stderr, "secs = %ld, msecs = %ld, usecs = %ld\n", to.seconds, to.milliseconds, to.microseconds);
  assert(true == error_flag);
}
static void
test_condition_invalid_microseconds_overflow (void)
{
  cce_location_t	L[1];
  ccevents_timeout_t	to;
  bool			error_flag = false;

  if (cce_location(L)) {
    error_flag = true;
    assert(true == ccevents_condition_is_timeout_overflow(cce_condition(L)));
    cce_run_error_handlers_final(L);
  } else {
    to = ccevents_timeout_init(L, LONG_MAX-1, 1000, 1000000);
    PRINTO(to);
    cce_run_cleanup_handlers(L);
  }
  //fprintf(stderr, "secs = %ld, msecs = %ld, usecs = %ld\n", to.seconds, to.milliseconds, to.microseconds);
  assert(true == error_flag);
}

static void
test_condition_invalid_seconds (void)
{
  cce_location_t	L[1];
  ccevents_timeout_t	to;
  bool			error_flag = false;

  if (cce_location(L)) {
    error_flag = true;
    assert(true == ccevents_condition_is_timeout_invalid(cce_condition(L)));
    cce_run_error_handlers_final(L);
  } else {
    to = ccevents_timeout_init(L, -10, 0, 0);
    PRINTO(to);
    cce_run_cleanup_handlers(L);
  }
  //fprintf(stderr, "secs = %ld, msecs = %ld, usecs = %ld\n", to.seconds, to.milliseconds, to.microseconds);
  assert(true == error_flag);
}
void
test_timeout_getters (void)
{
  cce_location_t	L[1];
  ccevents_timeout_t	to;
  bool			error_flag = false;

  if (cce_location(L)) {
    error_flag = true;
    cce_run_error_handlers_final(L);
  } else {
    to = ccevents_timeout_init(L, 1, 2, 3);
  }
  assert(false == error_flag);
  assert(1 == ccevents_timeout_seconds(to));
  assert(2 == ccevents_timeout_milliseconds(to));
  assert(3 == ccevents_timeout_microseconds(to));
}

static void
test_timeout_predicates (void)
{
  /* Infinite time span. */
  if (1) {
    cce_location_t	L[1];
    ccevents_timeout_t	to;
    bool		error_flag = false;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      to = ccevents_timeout_init(L, LONG_MAX, 0, 0);
    }
    assert(false == error_flag);
    assert(true  == ccevents_timeout_is_infinite(to));
  }
  /* Not infinite time span. */
  if (1) {
    cce_location_t	L[1];
    ccevents_timeout_t	to;
    bool		error_flag = false;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      to = ccevents_timeout_init(L, 123, 0, 0);
    }
    assert(false == error_flag);
    assert(false == ccevents_timeout_is_infinite(to));
  }
}

static void
test_timeout_span_comparisons (void)
{
  /* Equal. */
  if (1) {
    cce_location_t	L[1];
    ccevents_timeout_t	A;
    ccevents_timeout_t	B;
    bool		error_flag = false;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      A = ccevents_timeout_init(L, 1, 0, 0);
      B = ccevents_timeout_init(L, 1, 0, 0);
      assert(0 == ccevents_timeout_compare(A, B));
    }
    assert(false == error_flag);
  }
  /* Less by seconds. */
  if (1) {
    cce_location_t	L[1];
    ccevents_timeout_t	A;
    ccevents_timeout_t	B;
    bool		error_flag = false;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      A = ccevents_timeout_init(L, 1, 0, 0);
      B = ccevents_timeout_init(L, 2, 0, 0);
      assert(-1 == ccevents_timeout_compare(A, B));
    }
    assert(false == error_flag);
  }
  /* Less by milliseconds. */
  if (1) {
    cce_location_t	L[1];
    ccevents_timeout_t	A;
    ccevents_timeout_t	B;
    bool		error_flag = false;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      A = ccevents_timeout_init(L, 0, 1, 0);
      B = ccevents_timeout_init(L, 0, 2, 0);
      assert(-1 == ccevents_timeout_compare(A, B));
    }
    assert(false == error_flag);
  }
  /* Less by microseconds. */
  if (1) {
    cce_location_t	L[1];
    ccevents_timeout_t	A;
    ccevents_timeout_t	B;
    bool		error_flag = false;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      A = ccevents_timeout_init(L, 0, 0, 1);
      B = ccevents_timeout_init(L, 0, 0, 2);
      assert(-1 == ccevents_timeout_compare(A, B));
    }
    assert(false == error_flag);
  }
  /* Greater by seconds. */
  if (1) {
    cce_location_t	L[1];
    ccevents_timeout_t	A;
    ccevents_timeout_t	B;
    bool		error_flag = false;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      A = ccevents_timeout_init(L, 2, 0, 0);
      B = ccevents_timeout_init(L, 1, 0, 0);
      assert(+1 == ccevents_timeout_compare(A, B));
    }
    assert(false == error_flag);
  }
  /* Greater by milliseconds. */
  if (1) {
    cce_location_t	L[1];
    ccevents_timeout_t	A;
    ccevents_timeout_t	B;
    bool		error_flag = false;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      A = ccevents_timeout_init(L, 0, 2, 0);
      B = ccevents_timeout_init(L, 0, 1, 0);
      assert(+1 == ccevents_timeout_compare(A, B));
    }
    assert(false == error_flag);
  }
  /* Greater by microseconds. */
  if (1) {
    cce_location_t	L[1];
    ccevents_timeout_t	A;
    ccevents_timeout_t	B;
    bool		error_flag = false;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      A = ccevents_timeout_init(L, 0, 0, 2);
      B = ccevents_timeout_init(L, 0, 0, 1);
      assert(+1 == ccevents_timeout_compare(A, B));
    }
    assert(false == error_flag);
  }
}

static void
test_timeout_expiration_time_comparisons (void)
{
  /* Less. */
  if (1) {
    cce_location_t	L[1];
    ccevents_timeout_t	A, B;
    ccevents_timeval_t	a, b;
    bool		error_flag = false;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      A = ccevents_timeout_init(L, 1, 0, 0);
      B = ccevents_timeout_init(L, 1, 0, 1);
      a = ccevents_timeout_start(L, A);
      b = ccevents_timeout_start(L, B);
      /* fprintf(stderr, "a.tv_sec = %ld, a.tv_usec = %ld\n", a.tv_sec, a.tv_usec); */
      /* fprintf(stderr, "b.tv_sec = %ld, b.tv_usec = %ld\n", b.tv_sec, b.tv_usec); */
      assert(-1 == ccevents_timeval_compare(a, b));
    }
    assert(false == error_flag);
  }
  /* Greater. */
  if (1) {
    cce_location_t	L[1];
    ccevents_timeout_t	A, B;
    ccevents_timeval_t	a, b;
    bool		error_flag = false;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      A = ccevents_timeout_init(L, 10, 0, 0);
      B = ccevents_timeout_init(L, 1, 0, 0);
      a = ccevents_timeout_start(L, A);
      b = ccevents_timeout_start(L, B);
      //fprintf(stderr, "a.tv_sec = %ld, a.tv_usec = %ld\n", a.tv_sec, a.tv_usec);
      //fprintf(stderr, "b.tv_sec = %ld, b.tv_usec = %ld\n", b.tv_sec, b.tv_usec);
      assert(+1 == ccevents_timeval_compare(a, b));
    }
    assert(false == error_flag);
  }
}

static void
test_timeout_operations (void)
{
  /* Start and expire. */
  if (1) {
    cce_location_t	L[1];
    ccevents_timeout_t	to;
    ccevents_timeval_t	tv;
    struct timespec	sleep_span = { .tv_sec = 1, .tv_nsec = 0 };
    struct timespec	remaining_time;
    bool		error_flag = false;

    if (cce_location(L)) {
      error_flag = true;
      cce_run_error_handlers_final(L);
    } else {
      to = ccevents_timeout_init(L, 1, 0, 0);
      tv = ccevents_timeout_start(L, to);
      assert(false == ccevents_timeval_is_expired_timeout(tv));
      nanosleep(&sleep_span, &remaining_time);
      //fprintf(stderr, "to.tv_sec = %ld, to.tv_usec = %ld\n", to.tv_sec, to.tv_usec);
      assert(true == ccevents_timeval_is_expired_timeout(tv));
    }
    assert(false == error_flag);
  }
}

int
main (int argc CCEVENTS_UNUSED, const char *const argv[] CCEVENTS_UNUSED)
{
  ccevents_init();
  if (1) test_timeout_initialisation();
  if (1) test_timeout_initialisation_correct_milliseconds_overflow();
  if (1) test_timeout_initialisation_correct_microseconds_overflow();
  if (1) test_timeout_initialisation_double_microseconds_overflow();
  if (1) test_condition_invalid_milliseconds_overflow();
  if (1) test_condition_invalid_microseconds_overflow();
  if (1) test_condition_invalid_seconds();
  if (1) test_timeout_getters();
  if (1) test_timeout_predicates();
  if (1) test_timeout_span_comparisons();
  if (1) test_timeout_expiration_time_comparisons();
  if (1) test_timeout_operations();
  exit(EXIT_SUCCESS);
}

/* end of file */
