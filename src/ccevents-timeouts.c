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


/** --------------------------------------------------------------------
 ** Exceptional condition descriptor: timeout invalid.
 ** ----------------------------------------------------------------- */

static void             timeout_invalid_condition_destructor  (cce_condition_t * C);
static const char *     timeout_invalid_condition_static_message (const cce_condition_t * C);

static ccevents_condition_descriptor_timeout_invalid_t timeout_invalid_condition_descriptor = {
  .parent		= &ccevents_base_condition_descriptor_stru,
  .free			= timeout_invalid_condition_destructor,
  .static_message	= timeout_invalid_condition_static_message
};

const ccevents_condition_descriptor_timeout_invalid_t * \
  ccevents_condition_timeout_invalid_descriptor = &timeout_invalid_condition_descriptor;

/* There  are  no  dynamic  fields  in  this  condition  object,  so  we
   statically allocate it. */
static const ccevents_condition_timeout_invalid_t condition_timeout_invalid = {
  .descriptor = &timeout_invalid_condition_descriptor
};

const ccevents_condition_timeout_invalid_t *
ccevents_condition_timeout_invalid (void)
{
  return &condition_timeout_invalid;
}
void
timeout_invalid_condition_destructor (cce_condition_t * C CCEVENTS_UNUSED)
{
  return;
}
const char *
timeout_invalid_condition_static_message (const cce_condition_t * C CCEVENTS_UNUSED)
{
  return "timeout fields specification would cause invalid time";
}


/** --------------------------------------------------------------------
 ** Exceptional condition descriptor: timeout overflow.
 ** ----------------------------------------------------------------- */

static void             timeout_overflow_condition_destructor	(cce_condition_t * C);
static const char *     timeout_overflow_condition_static_message (const cce_condition_t * C);

static ccevents_condition_descriptor_timeout_overflow_t timeout_overflow_condition_descriptor = {
  .parent		= &ccevents_base_condition_descriptor_stru,
  .free			= timeout_overflow_condition_destructor,
  .static_message	= timeout_overflow_condition_static_message
};

const ccevents_condition_descriptor_timeout_overflow_t * \
  ccevents_condition_timeout_overflow_descriptor = &timeout_overflow_condition_descriptor;

/* There  are  no  dynamic  fields  in  this  condition  object,  so  we
   statically allocate it. */
static const ccevents_condition_timeout_overflow_t condition_timeout_overflow = {
  .descriptor = &timeout_overflow_condition_descriptor
};

const ccevents_condition_timeout_overflow_t *
ccevents_condition_timeout_overflow (void)
{
  return &condition_timeout_overflow;
}
void
timeout_overflow_condition_destructor (cce_condition_t * C CCEVENTS_UNUSED)
{
  return;
}
const char *
timeout_overflow_condition_static_message (const cce_condition_t * C CCEVENTS_UNUSED)
{
  return "timeout fields specification would cause overflow in time representation";
}


/** ------------------------------------------------------------
 ** Initialisation.
 ** ----------------------------------------------------------*/

/* A  constant, statically  allocated  instance of  "ccevents_timeout_t"
   representing a timeout that never expires. */
const ccevents_timeout_t CCEVENTS_TIMEOUT_NEVER	= {
  .tv_sec	= LONG_MAX,
  .tv_usec	= 0,
  .seconds	= LONG_MAX,
  .milliseconds	= 0,
  .microseconds	= 0
};

void
ccevents_timeout_init (cce_location_tag_t * there, ccevents_timeout_t * to,
		       const long seconds, const long milliseconds, const long microseconds)
/* Initialise  an   already  allocated  timeout  structure.    Raise  an
   exception if  the time  span values  would cause  an overflow  in the
   internal time representation. */
{
  /* Let's preserver the  operands so that we can use  them when raising
     an exception. */
  long		secs	= seconds;
  long		msecs	= milliseconds;
  long		usecs	= microseconds;

  if (seconds < 0) {
    cce_raise(there, ccevents_condition_timeout_invalid());
  }
  ccevents_debug("before normalisation: secs=%ld, msecs=%ld, usecs=%ld", secs, msecs, usecs);
  /* Overflow the milliseconds into the seconds. */
  if (msecs > 999) {
    long	div = msecs / 1000;
    long	mod = msecs % 1000;
    if ((LONG_MAX - seconds) > div) {
      secs += div;
    } else {
      cce_raise(there, ccevents_condition_timeout_overflow());
    }
    msecs = mod;
  }
  ccevents_debug("after msecs normalisation: secs=%ld, msecs=%ld, usecs=%ld", secs, msecs, usecs);
  /* Overflow the microseconds into the msecs. */
  if (usecs > 999) {
    long	div = usecs / 1000;
    long	mod = usecs % 1000;
    if ((LONG_MAX - msecs) > div) {
      msecs += div;
    } else {
      cce_raise(there, ccevents_condition_timeout_overflow());
    }
    usecs = mod;
  }
  ccevents_debug("after usecs normalisation: secs=%ld, msecs=%ld, usecs=%ld", secs, msecs, usecs);
  /* Overflow  the msecs  into  the seconds.   We do  this  twice as  an
     attempt to avoid overflowing the milliseconds. */
  if (msecs > 999) {
    long	div = msecs / 1000;
    long	mod = msecs % 1000;
    ccevents_debug("delta=%ld, div=%ld", LONG_MAX - seconds, div);
    if ((LONG_MAX - seconds) > div) {
      secs += div;
    } else {
      cce_raise(there, ccevents_condition_timeout_overflow());
    }
    msecs = mod;
  }
  ccevents_debug("LONG_MAX=                   %ld", LONG_MAX);
  ccevents_debug("after full normalisation: secs=%ld, msecs=%ld, usecs=%ld", secs, msecs, usecs);
  /* These fields represent the timeout's time span. */
  to->seconds		= secs;
  to->milliseconds	= msecs;
  to->microseconds	= usecs;
  /* These  fields  represent the  timeout's  absolute  time.  They  are
     properly set by the start function. */
  to->tv_sec		= LONG_MAX;
  to->tv_usec		= 0;
}
void
ccevents_timeout_copy (ccevents_timeout_t * dst, const ccevents_timeout_t * src)
{
  *dst = *src;
}


/** ------------------------------------------------------------
 ** Getters.
 ** ----------------------------------------------------------*/

long int
ccevents_timeout_seconds (const ccevents_timeout_t * to)
{
  return to->seconds;
}
long int
ccevents_timeout_milliseconds (const ccevents_timeout_t * to)
{
  return to->milliseconds;
}
long int
ccevents_timeout_microseconds (const ccevents_timeout_t * to)
{
  return to->microseconds;
}
ccevents_timeval_t
ccevents_timeout_time_span (const ccevents_timeout_t * to)
{
  ccevents_timeval_t	span = {
    .tv_sec  = to->seconds,
    /* With  a  normalised  'to'  the  'timeval'  representation  cannot
       overflow. */
    .tv_usec = 1000 * to->milliseconds + to->microseconds
  };
  return span;
}
ccevents_timeval_t
ccevents_timeout_time (ccevents_timeout_t * to)
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
ccevents_timeout_infinite_time_span (ccevents_timeout_t * to)
{
  return (LONG_MAX == to->seconds);
}
bool
ccevents_timeout_timed_out (ccevents_timeout_t * to)
{
  bool	rv;
  if (ccevents_timeout_infinite_time_span(to)) {
    rv = true;
  } else {
    ccevents_timeval_t	now;
    ccevents_timeval_t	trigger_time	= ccevents_timeout_time(to);
    gettimeofday(&now, NULL);
    if (now.tv_sec < trigger_time.tv_sec) {
      rv = true;
    } else if ((now.tv_sec = trigger_time.tv_sec) || (now.tv_usec <= trigger_time.tv_usec)) {
      rv = true;
    } else {
      rv = false;
    }
  }
  return rv;
}


/** ------------------------------------------------------------
 ** Timeout span comparison.
 ** ----------------------------------------------------------*/

int
ccevents_timeout_cmp (ccevents_timeout_t * toA, ccevents_timeout_t * toB)
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
bool
ccevents_timeout_less (ccevents_timeout_t * toA, ccevents_timeout_t * toB)
{
  return (-1 == ccevents_timeout_cmp(toA, toB));
}
bool
ccevents_timeout_equal (ccevents_timeout_t * toA, ccevents_timeout_t * toB)
{
  return (0  == ccevents_timeout_cmp(toA, toB));
}
bool
ccevents_timeout_greater (ccevents_timeout_t * toA, ccevents_timeout_t * toB)
{
  return (+1 == ccevents_timeout_cmp(toA, toB));
}


/** ------------------------------------------------------------
 ** Timeout trigger time comparison.
 ** ----------------------------------------------------------*/

int
ccevents_timeout_time_cmp (ccevents_timeout_t * A, ccevents_timeout_t * B)
{
  ccevents_timeval_t span_a = ccevents_timeout_time(A);
  ccevents_timeval_t span_b = ccevents_timeout_time(B);
  return ccevents_timeval_compare(span_a, span_b);
}
bool
ccevents_timeout_first (ccevents_timeout_t * A, ccevents_timeout_t * B)
{
  return (-1 == ccevents_timeout_time_cmp(A, B));
}
bool
ccevents_timeout_last (ccevents_timeout_t * A, ccevents_timeout_t * B)
{
  return (+1 == ccevents_timeout_time_cmp(A, B));
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
    ccevents_timeval_t	now;
    ccevents_timeval_t	span;

    gettimeofday(&now, NULL);
    span = ccevents_timeout_time_span(to);
    /* Compute the expiration time and store it in "to".  The expiration
       time  is the  sum  between the  current time  and  the time  span
       represented by "to". */
    if ((LONG_MAX - span.tv_sec) < now.tv_sec) {
      cce_raise(there, ccevents_condition_timeout_overflow());
    }
    to->tv_sec  = now.tv_sec + span.tv_sec;
    to->tv_usec = now.tv_usec + span.tv_usec;
    /* Normalise the strucutre. */
    to->tv_sec  += to->tv_usec / 1000000;
    to->tv_usec = to->tv_usec % 1000000;
  }
}
void
ccevents_timeout_reset (ccevents_timeout_t * to)
{
  to->tv_sec	= LONG_MAX; /* means infinite time span */
  to->tv_usec	= 0;
}

/* end of file */
