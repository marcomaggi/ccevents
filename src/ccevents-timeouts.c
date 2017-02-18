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

/** ------------------------------------------------------------
 ** Dynamic constants.
 ** ----------------------------------------------------------*/

/* A  constant, statically  allocated  instance of  "ccevents_timeout_t"
   representing a timeout that never expires. */
static const ccevents_timeout_t TIMEOUT_NEVER	= {
  .seconds	= LONG_MAX,
  .milliseconds	= 0,
  .microseconds	= 0
};
const ccevents_timeout_t * CCEVENTS_TIMEOUT_NEVER = &TIMEOUT_NEVER;

/* A  constant, statically  allocated  instance of  "ccevents_timeout_t"
   representing a timeout that never expires. */
static const ccevents_timeout_t TIMEOUT_NOW = {
  .seconds	= 0,
  .milliseconds	= 0,
  .microseconds	= 0
};
const ccevents_timeout_t * CCEVENTS_TIMEOUT_NOW = &TIMEOUT_NOW;

/** ------------------------------------------------------------
 ** Initialisation.
 ** ----------------------------------------------------------*/

static void
normalise_little_into_big (cce_location_t * there,
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
	cce_raise(there, ccevents_timeval_overflow_C());
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
      cce_raise(there, ccevents_timeout_overflow_C());
    }
    littles = mod;
  }
  /* Done. */
  *bigsp	= bigs;
  *littlesp	= littles;
}

ccevents_timeout_t
ccevents_timeout_init (cce_location_t * there,
		       long seconds, long milliseconds, long microseconds)
/* Initialise  an   already  allocated  timeout  structure.    Raise  an
   exception if  the time  span values  would cause  an overflow  in the
   internal time representation. */
{
  if ((seconds < 0) || (LONG_MAX < seconds) ||
      (milliseconds < LONG_MIN) || (LONG_MAX < milliseconds) ||
      (microseconds < LONG_MIN) || (LONG_MAX < microseconds)) {
    cce_raise(there, ccevents_timeout_invalid_C());
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
  {
    ccevents_timeout_t to = {
      .seconds		= seconds,
      .milliseconds	= milliseconds,
      .microseconds	= microseconds
    };
    return to;
  }
}

/** ------------------------------------------------------------
 ** Timeout comparisons.
 ** ----------------------------------------------------------*/

int
ccevents_timeout_compare (const ccevents_timeout_t toA, const ccevents_timeout_t toB)
{
  if (toA.seconds < toB.seconds) {
    return -1;
  } else if (toA.seconds > toB.seconds) {
    return +1;
  } else {
    /* (toA.seconds == toB.seconds) */
    if (toA.milliseconds < toB.milliseconds) {
      return -1;
    } else if (toA.milliseconds > toB.milliseconds) {
      return +1;
    } else {
      /* (toA.milliseconds == toB.milliseconds) */
      if (toA.microseconds < toB.microseconds) {
	return -1;
      } else if (toA.microseconds > toB.microseconds) {
	return +1;
      } else {
	/* (toA.microseconds == toB.microseconds) */
	return 0;
      }
    }
  }
}

/** ------------------------------------------------------------
 ** Operations: start and stop.
 ** ----------------------------------------------------------*/

ccevents_timeval_t
ccevents_timeout_start (cce_location_t * there, const ccevents_timeout_t to)
{
  if (ccevents_timeout_is_infinite(to)) {
    ccevents_timeval_t	R = { .tv_sec = LONG_MAX, .tv_usec = 0 };
    return R;
  } else {
    ccevents_timeval_t	now, span = {
      .tv_sec	= to.seconds,
      .tv_usec	= 1000L * to.milliseconds + to.microseconds
    };
    gettimeofday(&now, NULL);
    return ccevents_timeval_add(there, now, span);
  }
}

/* end of file */
