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

#include <ccevents.h>
#include <ccexceptions.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


static void
test_timeout_initialisation (void)
{
  cce_location_t	L;
  ccevents_timeout_t	to;
  bool			flag = false;

  if (cce_location(L)) {
    cce_condition_free(cce_location_condition(L));
    cce_run_error_handlers(L);
    flag = true;
  } else {
    ccevents_timeout_init(L, &to, 1, 2, 3);
    cce_run_cleanup_handlers(L);
  }
  assert(false == flag);
  assert(1 == to.seconds);
  assert(2 == to.milliseconds);
  assert(3 == to.microseconds);
}

static void
test_timeout_initialisation_correct_milliseconds_overflow (void)
{
  cce_location_t	L;
  ccevents_timeout_t	to;
  bool			flag = false;

  if (cce_location(L)) {
    cce_condition_free(cce_location_condition(L));
    cce_run_error_handlers(L);
    flag = true;
  } else {
    ccevents_timeout_init(L, &to, 0, 1001, 0);
    cce_run_cleanup_handlers(L);
  }
  assert(false == flag);
  //fprintf(stderr, "secs = %ld, msecs = %ld, usecs = %ld\n", to.seconds, to.milliseconds, to.microseconds);
  assert(1 == to.seconds);
  assert(1 == to.milliseconds);
  assert(0 == to.microseconds);
}

static void
test_timeout_initialisation_correct_microseconds_overflow (void)
{
  cce_location_t	L;
  ccevents_timeout_t	to;
  bool			flag = false;

  if (cce_location(L)) {
    cce_condition_free(cce_location_condition(L));
    cce_run_error_handlers(L);
    flag = true;
  } else {
    ccevents_timeout_init(L, &to, 0, 0, 1001);
    cce_run_cleanup_handlers(L);
  }
  assert(false == flag);
  //fprintf(stderr, "secs = %ld, msecs = %ld, usecs = %ld\n", to.seconds, to.milliseconds, to.microseconds);
  assert(0 == to.seconds);
  assert(1 == to.milliseconds);
  assert(1 == to.microseconds);
}

static void
test_timeout_initialisation_double_microseconds_overflow (void)
/* The microseconds value  overflows into both the  milliseconds and the
   seconds fields. */
{
  cce_location_t	L;
  ccevents_timeout_t	to;
  bool			flag = false;

  if (cce_location(L)) {
    cce_condition_free(cce_location_condition(L));
    cce_run_error_handlers(L);
    flag = true;
  } else {
    ccevents_timeout_init(L, &to, 0, 0, 1001001);
    cce_run_cleanup_handlers(L);
  }
  assert(false == flag);
  //fprintf(stderr, "secs = %ld, msecs = %ld, usecs = %ld\n", to.seconds, to.milliseconds, to.microseconds);
  assert(1 == to.seconds);
  assert(1 == to.milliseconds);
  assert(1 == to.microseconds);
}

static void
test_condition_invalid_milliseconds_overflow (void)
{
  cce_location_t	L;
  ccevents_timeout_t	to;
  bool			flag = false;

  if (cce_location(L)) {
    cce_condition_t *	C = cce_location_condition(L);
    {
      flag = true;
      assert(cce_condition_is_a(C, ccevents_condition_timeout_overflow_descriptor));
    }
    cce_condition_free(C);
    cce_run_error_handlers(L);
  } else {
    ccevents_timeout_init(L, &to, LONG_MAX - (LONG_MAX / 1000) + 1, LONG_MAX, 0);
    cce_run_cleanup_handlers(L);
  }
  //fprintf(stderr, "secs = %ld, msecs = %ld, usecs = %ld\n", to.seconds, to.milliseconds, to.microseconds);
  assert(true == flag);
}

static void
test_condition_invalid_microseconds_overflow (void)
{
  cce_location_t	L;
  ccevents_timeout_t	to;
  bool			flag = false;

  if (cce_location(L)) {
    cce_condition_t *	C = cce_location_condition(L);
    {
      assert(cce_condition_is_a(C, ccevents_condition_timeout_overflow_descriptor));
    }
    cce_condition_free(C);
    cce_run_error_handlers(L);
    flag = true;
  } else {
    ccevents_timeout_init(L, &to, LONG_MAX-1, 1000, 1000000);
    cce_run_cleanup_handlers(L);
  }
  //fprintf(stderr, "secs = %ld, msecs = %ld, usecs = %ld\n", to.seconds, to.milliseconds, to.microseconds);
  assert(true == flag);
}

static void
test_condition_invalid_seconds (void)
{
  cce_location_t	L;
  ccevents_timeout_t	to;
  bool			flag = false;

  if (cce_location(L)) {
    cce_condition_t *	C = cce_location_condition(L);
    {
      assert(cce_condition_is_a(C, ccevents_condition_timeout_invalid_descriptor));
    }
    cce_condition_free(C);
    cce_run_error_handlers(L);
      flag = true;
  } else {
    ccevents_timeout_init(L, &to, -10, 0, 0);
    cce_run_cleanup_handlers(L);
  }
  //fprintf(stderr, "secs = %ld, msecs = %ld, usecs = %ld\n", to.seconds, to.milliseconds, to.microseconds);
  assert(true == flag);
}

void
test_timeout_getters (void)
{
  cce_location_t	L;
  ccevents_timeout_t	to;
  bool			flag = false;

  if (cce_location(L)) {
    flag = true;
    cce_condition_free(cce_location_condition(L));
    cce_run_error_handlers(L);
  } else {
    ccevents_timeout_init(L, &to, 1, 2, 3);
  }
  assert(false == flag);
  assert(1 == ccevents_timeout_seconds(&to));
  assert(2 == ccevents_timeout_milliseconds(&to));
  assert(3 == ccevents_timeout_microseconds(&to));
  {
    ccevents_timeval_t	span;
    span = ccevents_timeout_time_span(&to);
    assert(1 == span.tv_sec);
    assert(2003 == span.tv_usec);
  }
  {
    ccevents_timeval_t	abs_time;
    abs_time = ccevents_timeout_time(&to);
    /* The timeout has not been started. */
    assert(LONG_MAX == abs_time.tv_sec);
    assert(0        == abs_time.tv_usec);
  }
}


static void
test_timeout_predicates (void)
{
  cce_location_t	L;
  ccevents_timeout_t	to;
  bool			flag = false;

  if (cce_location(L)) {
    flag = true;
    cce_condition_free(cce_location_condition(L));
    cce_run_error_handlers(L);
  } else {
    ccevents_timeout_init(L, &to, LONG_MAX, 0, 0);
  }
  assert(false == flag);
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
  exit(EXIT_SUCCESS);
}

/* end of file */
