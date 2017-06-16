/*
  Part of: CCEvents
  Contents: tests for the timevals module
  Date: Fri Jan 20, 2017

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
test_timeval_initialisation_from_values (void)
{
  /* Small positive values. */
  if (1) {
    cce_location_t	L[1];
    bool			flag = false;
    ccevents_timeval_t	T;

    if (cce_location(L)) {
      cce_run_error_handlers_final(L);
    } else {
      long	seconds		= 1;
      long	microseconds	= 2;
      T = ccevents_timeval_init(L, seconds, microseconds);
      cce_run_cleanup_handlers(L);
      flag = true;
    }
    assert(true == flag);
    assert(1 == T.tv_sec);
    assert(2 == T.tv_usec);
  }
  /* Negative microseconds: small value. */
  if (1) {
    cce_location_t	L[1];
    bool			flag = false;
    ccevents_timeval_t	T;

    if (cce_location(L)) {
      cce_run_error_handlers_final(L);
    } else {
      long	seconds		= 1;
      long	microseconds	= -2;
      T = ccevents_timeval_init(L, seconds, microseconds);
      cce_run_cleanup_handlers(L);
      flag = true;
    }
    if (0) fprintf(stderr, "tv_sec = %ld, tv_usec = %ld\n", T.tv_sec, T.tv_usec);
    assert(true == flag);
    assert(0 == T.tv_sec);
    assert(999998 == T.tv_usec);
  }
  /* Negative microseconds: big value,  correct overflow of microseconds
     into seconds. */
  if (1) {
    cce_location_t	L[1];
    bool			flag = false;
    ccevents_timeval_t	T;

    if (cce_location(L)) {
      cce_run_error_handlers_final(L);
    } else {
      long	seconds		= 2;
      long	microseconds	= -1000000;
      T = ccevents_timeval_init(L, seconds, microseconds);
      cce_run_cleanup_handlers(L);
      flag = true;
    }
    if (0) fprintf(stderr, "tv_sec = %ld, tv_usec = %ld\n", T.tv_sec, T.tv_usec);
    assert(true == flag);
    assert(1 == T.tv_sec);
    assert(0 == T.tv_usec);
  }
  /* Negative microseconds: big value,  correct overflow of microseconds
     into seconds. */
  if (1) {
    cce_location_t	L[1];
    bool			flag = false;
    ccevents_timeval_t	T;

    if (cce_location(L)) {
      cce_run_error_handlers_final(L);
    } else {
      long	seconds		= 2;
      long	microseconds	= -1000001;
      T = ccevents_timeval_init(L, seconds, microseconds);
      cce_run_cleanup_handlers(L);
      flag = true;
    }
    if (0) fprintf(stderr, "tv_sec = %ld, tv_usec = %ld\n", T.tv_sec, T.tv_usec);
    assert(true == flag);
    assert(0 == T.tv_sec);
    assert(999999 == T.tv_usec);
  }
  /* Negative microseconds: LONG_MIN,  erroneous overflow.  Distributing
     the  microseconds on  the seconds  fields causes  the latter  to be
     negative. */
  if (1) {
    cce_location_t	L[1];
    bool		flag = false;

    if (cce_location(L)) {
      cce_condition_t *	C = cce_condition(L);
      assert(ccevents_condition_is_timeval_overflow(C));
      cce_run_error_handlers_final(L);
      flag = true;
    } else {
      long	seconds		= 0;
      long	microseconds	= LONG_MIN;
      ccevents_timeval_init(L, seconds, microseconds);
      cce_run_cleanup_handlers(L);
    }
    assert(true == flag);
  }
}
static void
test_timeval_initialisation_from_timeval (void)
{
  cce_location_t	L[1];
  bool			flag = false;
  ccevents_timeval_t	T;

  if (cce_location(L)) {
    cce_run_error_handlers_final(L);
  } else {
    struct timeval	tv = { .tv_sec = 1, .tv_usec = 2};
    T = ccevents_timeval_normalise(L, tv);
    cce_run_cleanup_handlers(L);
    flag = true;
  }
  assert(true == flag);
  assert(1 == T.tv_sec);
  assert(2 == T.tv_usec);
}


static void
test_timeval_initialisation_invalid_from_values (void)
{
  cce_location_t	L[1];
  bool			flag = false;

  if (cce_location(L)) {
    cce_condition_t *	C = cce_condition(L);
    assert(ccevents_condition_is_timeval_invalid(C));
    cce_run_error_handlers_final(L);
    flag = true;
  } else {
    long	seconds		= -123;
    long	microseconds	= 2;
    ccevents_timeval_init(L, seconds, microseconds);
    cce_run_cleanup_handlers(L);
  }
  assert(true == flag);
}
static void
test_timeval_initialisation_overflow_from_values (void)
{
  cce_location_t	L[1];
  bool			flag = false;

  if (cce_location(L)) {
    cce_condition_t *	C = cce_condition(L);
    assert(ccevents_condition_is_timeval_overflow(C));
    cce_run_error_handlers_final(L);
    flag = true;
  } else {
    long	seconds		= LONG_MAX;
    long	microseconds	= LONG_MAX;
    ccevents_timeval_init(L, seconds, microseconds);
    cce_run_cleanup_handlers(L);
  }
  assert(true == flag);
}


static void
test_timeval_initialisation_invalid_from_timeval (void)
{
  cce_location_t	L[1];
  bool			error_flag = false;

  if (cce_location(L)) {
    cce_condition_t *	C = cce_condition(L);
    assert(ccevents_condition_is_timeval_invalid(C));
    cce_run_error_handlers_final(L);
    error_flag = true;
  } else {
    struct timeval	T = { .tv_sec = -123, .tv_usec = 2 };
    ccevents_timeval_t	tv;
    tv = ccevents_timeval_normalise(L, T);
    /* Let's print to make "tv" used: GCC will not remove it. */
    fprintf(stderr, "%ld\n", tv.tv_sec);
    cce_run_cleanup_handlers(L);
  }
  assert(true == error_flag);
}
static void
test_timeval_initialisation_overflow_from_timeval (void)
{
  cce_location_t	L[1];
  bool			flag = false;

  if (cce_location(L)) {
    cce_condition_t *	C = cce_condition(L);
    assert(ccevents_condition_is_timeval_overflow(C));
    cce_run_error_handlers_final(L);
    flag = true;
  } else {
    struct timeval	T = { .tv_sec = LONG_MAX, .tv_usec = LONG_MAX };
    ccevents_timeval_t	R;
    R = ccevents_timeval_normalise(L, T);
    /* Let's print to make R used: GCC will not remove it. */
    fprintf(stderr, "%ld\n", R.tv_sec);
    cce_run_cleanup_handlers(L);
  }
  assert(true == flag);
}


static void
test_timeval_addition (void)
{
  /* Successful addition. */
  if (1) {
    cce_location_t	L[1];
    bool		flag = false;
    ccevents_timeval_t	A, B, C;

    if (cce_location(L)) {
      cce_run_error_handlers_final(L);
    } else {
      A = ccevents_timeval_init(L, 10, 300);
      B = ccevents_timeval_init(L, 2, 44);
      if (0) fprintf(stderr, "A.tv_sec = %ld, A.tv_usec = %ld\n", A.tv_sec, A.tv_usec);
      if (0) fprintf(stderr, "B.tv_sec = %ld, B.tv_usec = %ld\n", B.tv_sec, B.tv_usec);
      C = ccevents_timeval_add(L, A, B);
      if (0) fprintf(stderr, "C.tv_sec = %ld, C.tv_usec = %ld\n", C.tv_sec, C.tv_usec);
      cce_run_cleanup_handlers(L);
      flag = true;
    }
    assert(true == flag);
    assert((10+2) == C.tv_sec);
    assert((44+300) == C.tv_usec);
  }

  /* Overflow adding seconds. */
  if (1) {
    cce_location_t	L[1];
    bool		flag = false;
    ccevents_timeval_t	A, B;

    if (cce_location(L)) {
      cce_condition_t *	C = cce_condition(L);
      assert(ccevents_condition_is_timeval_overflow(C));
      cce_run_error_handlers_final(L);
      flag = true;
    } else {
      ccevents_timeval_t	C;
      A = ccevents_timeval_init(L, LONG_MAX, 300);
      B = ccevents_timeval_init(L, 100, 44);
      C = ccevents_timeval_add(L, A, B);
      /* Let's print to make C used: GCC will not remove it. */
      fprintf(stderr, "%ld\n", C.tv_sec);
      cce_run_cleanup_handlers(L);
    }
    assert(true == flag);
  }

  /* Overflow distributing microseconds. */
  if (1) {
    cce_location_t	L[1];
    bool		flag = false;
    ccevents_timeval_t	A, B;

    if (cce_location(L)) {
      cce_condition_t *	C = cce_condition(L);
      assert(ccevents_condition_is_timeval_overflow(C));
      cce_run_error_handlers_final(L);
      flag = true;
    } else {
      ccevents_timeval_t	R;
      A = ccevents_timeval_init(L, LONG_MAX, 300);
      B = ccevents_timeval_init(L, 0, 2000000);
      R = ccevents_timeval_add(L, A, B);
      /* Let's print to make R used: GCC will not remove it. */
      fprintf(stderr, "%ld\n", R.tv_sec);
      cce_run_cleanup_handlers(L);
    }
    assert(true == flag);
  }
}


static void
test_timeval_subtraction (void)
{
  /* Successful subtraction. */
  if (1) {
    cce_location_t	L[1];
    bool		flag = false;
    ccevents_timeval_t	A, B, C;

    if (cce_location(L)) {
      cce_run_error_handlers_final(L);
    } else {
      A = ccevents_timeval_init(L, 10, 300);
      B = ccevents_timeval_init(L, 2, 44);
      if (0) fprintf(stderr, "A.tv_sec = %ld, A.tv_usec = %ld\n", A.tv_sec, A.tv_usec);
      if (0) fprintf(stderr, "B.tv_sec = %ld, B.tv_usec = %ld\n", B.tv_sec, B.tv_usec);
      C = ccevents_timeval_sub(L, A, B);
      if (0) fprintf(stderr, "C.tv_sec = %ld, C.tv_usec = %ld\n", C.tv_sec, C.tv_usec);
      cce_run_cleanup_handlers(L);
      flag = true;
    }
    assert(true == flag);
    assert((10-2) == C.tv_sec);
    assert((300-44) == C.tv_usec);
  }

  /* Invalid value subtracting seconds. */
  if (1) {
    cce_location_t	L[1];
    bool		flag = false;
    ccevents_timeval_t	A, B;

    if (cce_location(L)) {
      cce_condition_t *	C = cce_condition(L);
      assert(ccevents_condition_is_timeval_invalid(C));
      cce_run_error_handlers_final(L);
      flag = true;
    } else {
      ccevents_timeval_t	R;
      A = ccevents_timeval_init(L, 10, 300);
      B = ccevents_timeval_init(L, 99, 44);
      R = ccevents_timeval_sub(L, A, B);
      /* Let's print to make R used: GCC will not remove it. */
      fprintf(stderr, "%ld\n", R.tv_sec);
      cce_run_cleanup_handlers(L);
    }
    assert(true == flag);
  }

  /* Overflow distributing microseconds. */
  if (1) {
    cce_location_t	L[1];
    bool		flag = false;
    ccevents_timeval_t	A, B;

    if (cce_location(L)) {
      cce_condition_t *	C = cce_condition(L);
      assert(ccevents_condition_is_timeval_overflow(C));
      cce_run_error_handlers_final(L);
      flag = true;
    } else {
      ccevents_timeval_t	R;
      A = ccevents_timeval_init(L, LONG_MAX, 0);
      B = ccevents_timeval_init(L, 0, -100);
      R = ccevents_timeval_add(L, A, B);
      /* Let's print to make R used: GCC will not remove it. */
      fprintf(stderr, "%ld\n", R.tv_sec);
      cce_run_cleanup_handlers(L);
    }
    assert(true == flag);
  }
}


static void
test_timeval_comparison (void)
{
  /* Equal structs. */
  if (1) {
    cce_location_t	L[1];
    bool		flag = false;
    ccevents_timeval_t	A, B;

    if (cce_location(L)) {
      cce_run_error_handlers_final(L);
    } else {
      A = ccevents_timeval_init(L, 0, 0);
      B = ccevents_timeval_init(L, 0, 0);
      assert(0 == ccevents_timeval_compare(A, B));
      flag = true;
    }
    assert(true == flag);
  }

  /* Greater structs by seconds. */
  if (1) {
    cce_location_t	L[1];
    bool		flag = false;
    ccevents_timeval_t	A, B;

    if (cce_location(L)) {
      cce_run_error_handlers_final(L);
    } else {
      A = ccevents_timeval_init(L, 1, 0);
      B = ccevents_timeval_init(L, 0, 0);
      assert(+1 == ccevents_timeval_compare(A, B));
      flag = true;
    }
    assert(true == flag);
  }

  /* Greater structs by microseconds. */
  if (1) {
    cce_location_t	L[1];
    bool		flag = false;
    ccevents_timeval_t	A, B;

    if (cce_location(L)) {
      cce_run_error_handlers_final(L);
    } else {
      A = ccevents_timeval_init(L, 0, 1);
      B = ccevents_timeval_init(L, 0, 0);
      assert(+1 == ccevents_timeval_compare(A, B));
      flag = true;
    }
    assert(true == flag);
  }

  /* Less structs by seconds. */
  if (1) {
    cce_location_t	L[1];
    bool		flag = false;
    ccevents_timeval_t	A, B;

    if (cce_location(L)) {
      cce_run_error_handlers_final(L);
    } else {
      A = ccevents_timeval_init(L, 0, 0);
      B = ccevents_timeval_init(L, 1, 0);
      assert(-1 == ccevents_timeval_compare(A, B));
      flag = true;
    }
    assert(true == flag);
  }

  /* Less structs by microseconds. */
  if (1) {
    cce_location_t	L[1];
    bool		flag = false;
    ccevents_timeval_t	A, B;

    if (cce_location(L)) {
      cce_run_error_handlers_final(L);
    } else {
      A = ccevents_timeval_init(L, 0, 0);
      B = ccevents_timeval_init(L, 0, 1);
      assert(-1 == ccevents_timeval_compare(A, B));
      flag = true;
    }
    assert(true == flag);
  }
}


int
main (int argc CCEVENTS_UNUSED, const char *const argv[] CCEVENTS_UNUSED)
{
  ccevents_init();
  if (1) test_timeval_initialisation_from_values();
  if (1) test_timeval_initialisation_from_timeval();
  if (1) test_timeval_initialisation_invalid_from_values();
  if (1) test_timeval_initialisation_overflow_from_values();
  if (1) test_timeval_initialisation_invalid_from_timeval();
  if (1) test_timeval_initialisation_overflow_from_timeval();
  //
  if (1) test_timeval_addition();
  if (1) test_timeval_subtraction();
  //
  if (1) test_timeval_comparison();
  exit(EXIT_SUCCESS);
}

/* end of file */
