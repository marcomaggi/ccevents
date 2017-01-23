/*
  Part of: CCEvents
  Contents: timeout facilities
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

#include "ccevents-internals.h"
#define CCEVENTS_DEBUGGING	0
#include "ccevents-debug.h"
#include <limits.h>
#include <sys/time.h>

#define MAX_USEC		1000000


/** ------------------------------------------------------------
 ** Dynamic constants.
 ** ----------------------------------------------------------*/

/* A  constant, statically  allocated  instance of  "ccevents_timeout_t"
   representing a timeout that never expires. */
static const ccevents_timeout_t TIMEOUT_NEVER	= {
  .tv_sec	= LONG_MAX,
  .tv_usec	= 0,
  .seconds	= LONG_MAX,
  .milliseconds	= 0,
  .microseconds	= 0
};
const ccevents_timeout_t * CCEVENTS_TIMEOUT_NEVER = &TIMEOUT_NEVER;

/* A  constant, statically  allocated  instance of  "ccevents_timeout_t"
   representing a timeout that never expires. */
static const ccevents_timeout_t TIMEOUT_NOW = {
  .tv_sec	= 0,
  .tv_usec	= 0,
  .seconds	= 0,
  .milliseconds	= 0,
  .microseconds	= 0
};
const ccevents_timeout_t * CCEVENTS_TIMEOUT_NOW = &TIMEOUT_NOW;


/** ------------------------------------------------------------
 ** Initialisation.
 ** ----------------------------------------------------------*/

static void
normalise_little_into_big (cce_location_tag_t * there,
			   long * hugep, long * bigsp, long * littlesp)
/* To make it clear, let's assume:
 *
 * - "*hugep" is a number of seconds;
 *
 * - "*bigp" is a number of milliseconds;
 *
 * - "*littlesp" is a number of microseconds;
 *
 * then: this function  takes the positive or  negative microseconds and
 * normalises them  into the range  [0, 999], by distributing  the value
 * into the milliseconds count.
 *
 * When  "hugep"  is  not  NULL:  it  may  happen  that  the  number  of
 * milliseconds is distributed on the seconds.  When "hugep" is NULL: if
 * a normalisation of  milliseconds is required, an  exception is raised
 * with a non-local exit to "there".
 */
{
  long	bigs	= *bigsp;
  long	littles	= *littlesp;

  if (littles < 0) {
    /* The littles  are negative: normalise  them by reducing  the bigs.
     * When "littles" is negative: both "div" and "mod" are negative:
     *
     * - When  "long"  is   a  64-bit  word:  "div"  is   in  the  range
     *   [-9223372036854775, 0].
     *
     * - "mod" is in the range [-999, 0].
     */
    long	div = littles / 1000L;
    long	mod = littles % 1000L;
    bigs    += div - 1;
    littles  = mod + 1000L;
    /* Here "littles" is in the range [0, 1000L]. */
    if (0 > bigs) {
      /* After distributing  negative littles  into bigs, the  number of
	 bigs is  negative.  If possible:  distribute the bigs  into the
	 huges, otherwise raise an exception. */
      if (NULL != hugep) {
	normalise_little_into_big(there, NULL, hugep, &bigs);
      } else {
	cce_raise(there, ccevents_condition_timeval_overflow());
      }
    }
  }
  if (littles > 999) {
    /* Overflow the littles into the bigs. */
    long	div = littles / 1000L;
    long	mod = littles % 1000L;
    if ((LONG_MAX - bigs) > div) {
      bigs += div;
    } else {
      cce_raise(there, ccevents_condition_timeout_overflow());
    }
    littles = mod;
  }
  /* Done. */
  *bigsp	= bigs;
  *littlesp	= littles;
}

void
ccevents_timeout_init (cce_location_tag_t * there, ccevents_timeout_t * to,
		       long seconds, long milliseconds, long microseconds)
/* Initialise  an   already  allocated  timeout  structure.    Raise  an
   exception if  the time  span values  would cause  an overflow  in the
   internal time representation. */
{
  if ((seconds < 0) || (LONG_MAX < seconds) ||
      (milliseconds < LONG_MIN) || (LONG_MAX < milliseconds) ||
      (microseconds < LONG_MIN) || (LONG_MAX < microseconds)) {
    cce_raise(there, ccevents_condition_timeout_invalid());
  }
  ccevents_debug("before normalisation: seconds=%ld, milliseconds=%ld, microseconds=%ld", seconds, milliseconds, microseconds);

  /* Pre-normalisation of the milliseconds. */
  normalise_little_into_big(there, NULL, &seconds, &milliseconds);
  ccevents_debug("after milliseconds pre-normalisation: seconds=%ld, milliseconds=%ld, microseconds=%ld", seconds, milliseconds, microseconds);

  /* Normalisation of the microseconds. */
  normalise_little_into_big(there, &seconds, &milliseconds, &microseconds);
  ccevents_debug("after microseconds normalisation: seconds=%ld, milliseconds=%ld, microseconds=%ld", seconds, milliseconds, microseconds);

  /* Normalisation of the milliseconds.  */
  normalise_little_into_big(there, NULL, &seconds, &milliseconds);
  ccevents_debug("after milliseconds normalisation: seconds=%ld, milliseconds=%ld, microseconds=%ld", seconds, milliseconds, microseconds);

  /* These fields represent the timeout's time span. */
  to->seconds		= seconds;
  to->milliseconds	= milliseconds;
  to->microseconds	= microseconds;
  /* The "tv"  fields represent the timeout's  absolute expiration time;
     they are properly set by the start function. */
  to->tv_sec		= LONG_MAX;
  to->tv_usec		= 0;
}


/** ------------------------------------------------------------
 ** Getters.
 ** ----------------------------------------------------------*/

ccevents_timeval_t
ccevents_timeout_time_span (const ccevents_timeout_t * to)
{
  ccevents_timeval_t	span = {
    .tv_sec  = to->seconds,
    /* With  a  normalised  "to"  the  "timeval"  representation  cannot
       overflow. */
    .tv_usec = 1000L * to->milliseconds + to->microseconds
  };
  return span;
}
ccevents_timeval_t
ccevents_timeout_time (const ccevents_timeout_t * to)
{
  ccevents_timeval_t	absolute_time = {
    .tv_sec  = to->tv_sec,
    .tv_usec = to->tv_usec
  };
  return absolute_time;
}


/** ------------------------------------------------------------
 ** Predicates.
 ** ----------------------------------------------------------*/

bool
ccevents_timeout_infinite_time_span (const ccevents_timeout_t * to)
{
  return (LONG_MAX == to->seconds);
}
bool
ccevents_timeout_expired (const ccevents_timeout_t * to)
{
  bool	rv;
  if (ccevents_timeout_infinite_time_span(to)) {
    /* A timeout with infinite time span never expires. */
    rv = false;
  } else {
    ccevents_timeval_t	now;
    ccevents_timeval_t	expiration_time	= ccevents_timeout_time(to);
    gettimeofday(&now, NULL);
    //fprintf(stderr, "now.tv_sec = %ld, now.tv_usec = %ld\n", now.tv_sec, now.tv_usec);
    //fprintf(stderr, "expiration_time.tv_sec = %ld, expiration_time.tv_usec = %ld\n", expiration_time.tv_sec, expiration_time.tv_usec);
    if (now.tv_sec < expiration_time.tv_sec) {
      rv = false;
    } else if ((now.tv_sec == expiration_time.tv_sec) && (now.tv_usec <= expiration_time.tv_usec)) {
      rv = false;
    } else {
      rv = true;
    }
  }
  return rv;
}


/** ------------------------------------------------------------
 ** Timeout comparisons.
 ** ----------------------------------------------------------*/

int
ccevents_timeout_compare_time_span (const ccevents_timeout_t * toA, const ccevents_timeout_t * toB)
{
  if (toA->seconds < toB->seconds) {
    return -1;
  } else if (toA->seconds > toB->seconds) {
    return +1;
  } else {
    /* (toA->seconds == toB->seconds) */
    if (toA->milliseconds < toB->milliseconds) {
      return -1;
    } else if (toA->milliseconds > toB->milliseconds) {
      return +1;
    } else {
      /* (toA->milliseconds == toB->milliseconds) */
      if (toA->microseconds < toB->microseconds) {
	return -1;
      } else if (toA->microseconds > toB->microseconds) {
	return +1;
      } else {
	/* (toA->microseconds == toB->microseconds) */
	return 0;
      }
    }
  }
}
int
ccevents_timeout_compare_expiration_time (const ccevents_timeout_t * A, const ccevents_timeout_t * B)
{
  ccevents_timeval_t span_a = ccevents_timeout_time(A);
  ccevents_timeval_t span_b = ccevents_timeout_time(B);
  return ccevents_timeval_compare(span_a, span_b);
}


/** ------------------------------------------------------------
 ** Operations: start and stop.
 ** ----------------------------------------------------------*/

void
ccevents_timeout_start (cce_location_tag_t * there, ccevents_timeout_t * to)
{
  if (ccevents_timeout_infinite_time_span(to)) {
    to->tv_sec  = LONG_MAX;
    to->tv_usec	= 0;
  } else {
    ccevents_timeval_t	now, span, R;

    gettimeofday(&now, NULL);
    span = ccevents_timeout_time_span(to);
    R = ccevents_timeval_add(there, now, span);
    to->tv_sec	= R.tv_sec;
    to->tv_usec	= R.tv_usec;
  }
}
void
ccevents_timeout_reset (ccevents_timeout_t * to)
{
  to->tv_sec	= LONG_MAX; /* means infinite time span */
  to->tv_usec	= 0;
}

/* end of file */
