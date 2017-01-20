/*
  Part of: CCEvents
  Contents: operations on "struct timeval"
  Date: Thu Jan 19, 2017

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


/*** headers ***/

#include "ccevents-internals.h"

#define ASSERT_NORMALISED_TIMEVAL(TV)		\
  do {						\
    assert(0        <= (TV).tv_sec);		\
    assert(LONG_MAX >= (TV).tv_sec);		\
    assert(0        <= (TV).tv_usec);		\
    assert(999999   >= (TV).tv_usec);		\
  } while (0);


/** --------------------------------------------------------------------
 ** Exceptional condition descriptor: timeval invalid.
 ** ----------------------------------------------------------------- */

static void		condition_timeval_invalid_destructor     (cce_condition_t * C);
static const char *	condition_timeval_invalid_static_message (const cce_condition_t * C);

static ccevents_condition_descriptor_timeval_invalid_t condition_timeval_invalid_descriptor = {
  .parent		= &ccevents_base_condition_descriptor_stru,
  .free			= condition_timeval_invalid_destructor,
  .static_message	= condition_timeval_invalid_static_message
};

const ccevents_condition_descriptor_timeval_invalid_t * \
  ccevents_condition_timeval_invalid_descriptor = &condition_timeval_invalid_descriptor;

/* This struct type has no dynamic fields, so there is only one instance
   of this struct type.  We allocate it statically. */
static const ccevents_condition_timeval_invalid_t condition_timeval_invalid = {
  .descriptor = &condition_timeval_invalid_descriptor
};

const ccevents_condition_timeval_invalid_t *
ccevents_condition_timeval_invalid (void)
{
  return &condition_timeval_invalid;
}
void
condition_timeval_invalid_destructor (cce_condition_t * C CCEVENTS_UNUSED)
{
  return;
}
const char *
condition_timeval_invalid_static_message (const cce_condition_t * C CCEVENTS_UNUSED)
{
  return "timeval fields specification are invalid";
}


/** --------------------------------------------------------------------
 ** Exceptional condition descriptor: timeval overflow.
 ** ----------------------------------------------------------------- */

static void		condition_timeval_overflow_destructor     (cce_condition_t * C);
static const char *	condition_timeval_overflow_static_message (const cce_condition_t * C);

static ccevents_condition_descriptor_timeval_overflow_t condition_timeval_overflow_descriptor = {
  .parent		= &ccevents_base_condition_descriptor_stru,
  .free			= condition_timeval_overflow_destructor,
  .static_message	= condition_timeval_overflow_static_message
};

const ccevents_condition_descriptor_timeval_overflow_t * \
  ccevents_condition_timeval_overflow_descriptor = &condition_timeval_overflow_descriptor;

/* This struct type has no dynamic fields, so there is only one instance
   of this struct type.  We allocate it statically. */
static const ccevents_condition_timeval_overflow_t condition_timeval_overflow = {
  .descriptor = &condition_timeval_overflow_descriptor
};

const ccevents_condition_timeval_overflow_t *
ccevents_condition_timeval_overflow (void)
{
  return &condition_timeval_overflow;
}
void
condition_timeval_overflow_destructor (cce_condition_t * C CCEVENTS_UNUSED)
{
  return;
}
const char *
condition_timeval_overflow_static_message (const cce_condition_t * C CCEVENTS_UNUSED)
{
  return "timeval fields specification would cause overflow in time representation";
}


ccevents_timeval_t
ccevents_timeval_normalise (cce_location_tag_t * there, struct timeval T)
/* Normalise  the structure  "T" and  return the  result.  The  returned
 * structure is guaranteed to have:
 *
 *    0 <= R.tv_sec  <= LONG_MAX
 *    0 <= R.tv_usec <= 999999
 *
 * The field "tv_sec" of "T" is meant  to be in the range [0, LONG_MAX].
 * The field  "tv_usec" of "T"  is meant to  be in the  range [LONG_MIN,
 * LONG_MAX].
 */
{
  if ((T.tv_sec < 0) || (LONG_MAX < T.tv_sec) ||
      (T.tv_usec < LONG_MIN) || (LONG_MAX < T.tv_usec)) {
    cce_raise(there, ccevents_condition_timeval_invalid());
  }
  if (0 > T.tv_usec) {
    /* If "T.tv_usec" is negative: both "div" and "mod" are negative. */
    long	div = T.tv_usec / 1000000L;
    long	mod = T.tv_usec % 1000000L;
    if (0) fprintf(stderr, "div = %ld, mod = %ld, LONG_MIN = %ld\n", div, mod, LONG_MIN);
    T.tv_sec += div - 1;
    T.tv_usec = mod + 1000000L;
    if (0) fprintf(stderr, "tv_sec = %ld, tv_usec = %ld\n", T.tv_sec, T.tv_usec);
    /* It is possible that:  after distributing negative microseconds into
       seconds, the number of seconds is negative. */
    if (0 > T.tv_sec) {
      cce_raise(there, ccevents_condition_timeval_overflow());
    }
  }
  if (T.tv_usec > 999999) {
    long	div = T.tv_usec / 1000000L;
    long	mod = T.tv_usec % 1000000L;
    if ((LONG_MAX - T.tv_sec) > div) {
      T.tv_sec += div;
    } else {
      cce_raise(there, ccevents_condition_timeval_overflow());
    }
    T.tv_usec = mod;
  }
  {
    ccevents_timeval_t	R = { .tv_sec = T.tv_sec, .tv_usec = T.tv_usec };
    return R;
  }
}
ccevents_timeval_t
ccevents_timeval_init (cce_location_tag_t * there, long seconds, long microseconds)
{
  struct timeval	T = { .tv_sec = seconds, .tv_usec = microseconds };
  if (0 > seconds) {
    cce_raise(there, ccevents_condition_timeval_invalid());
  }
  return ccevents_timeval_normalise(there, T);
}


ccevents_timeval_t
ccevents_timeval_add (cce_location_tag_t * there, ccevents_timeval_t A, ccevents_timeval_t B)
/* Add two  normalised timeval  structures and  return the  result.  The
 * returned structure is guaranteed to have:
 *
 *    0 <= R.tv_sec  <= LONG_MAX
 *    0 <= R.tv_usec <= 999999
 */
{
  ccevents_timeval_t	R;
  ASSERT_NORMALISED_TIMEVAL(A);
  ASSERT_NORMALISED_TIMEVAL(B);

  if ((LONG_MAX - A.tv_sec) > B.tv_sec) {
    R.tv_sec = A.tv_sec + B.tv_sec;
  } else {
    cce_raise(there, ccevents_condition_timeval_overflow());
  }

  /* The sum between  two normalised "usec" fields is  guaranteed to fit
     into a "usec" field. */
  R.tv_usec = A.tv_usec + B.tv_usec;
  /* Redistribute the excess "usec" into the "sec" field. */
  {
    long	secs = R.tv_usec / 1000000L;
    R.tv_usec %= 1000000L;
    if ((LONG_MAX - R.tv_sec) > secs) {
      R.tv_sec += secs;
    } else {
      cce_raise(there, ccevents_condition_timeval_overflow());
    }
  }
  return R;
}


ccevents_timeval_t
ccevents_timeval_sub (cce_location_tag_t * there, ccevents_timeval_t A, ccevents_timeval_t B)
/* Subtract B from A  and return the result: R = A -  B.  Assume A and B
 * are normalised, that is:
 *
 *    0 <= A.tv_sec  <= LONG_MAX
 *    0 <= A.tv_usec <= 999999
 *
 *    0 <= B.tv_sec  <= LONG_MAX
 *    0 <= B.tv_usec <= 999999
 *
 * Return a structure for which:
 *
 *    0 <= R.tv_sec  <= LONG_MAX
 *    0 <= R.tv_usec <= 999999
 *
 */
{
  ASSERT_NORMALISED_TIMEVAL(A);
  ASSERT_NORMALISED_TIMEVAL(B);
  return ccevents_timeval_init(there, (A.tv_sec - B.tv_sec), (A.tv_usec - B.tv_usec));
}


int
ccevents_timeval_compare (ccevents_timeval_t A, ccevents_timeval_t B)
{
  if (A.tv_sec < B.tv_sec) {
    return -1;
  } else if (A.tv_sec > B.tv_sec) {
    return +1;
  } else {
    /* (A.tv_sec == B.tv_sec) */
    if (A.tv_usec < B.tv_usec) {
      return -1;
    } else if (A.tv_usec > B.tv_usec) {
      return +1;
    } else {
      return 0;
    }
  }
}

/* end of file */
