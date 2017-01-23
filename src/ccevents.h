/*
  Part of: CCEvents
  Contents: public header file
  Date: Jan 11, 2017

  Abstract



  Copyright (C) 2017 Marco Maggi <marco.maggi-ipsu@poste.it>

  This program is  free software: you can redistribute  it and/or modify
  it  under the  terms  of  the GNU  Lesser  General  Public License  as
  published by  the Free  Software Foundation, either  version 3  of the
  License, or (at your option) any later version.

  This program  is distributed in the  hope that it will  be useful, but
  WITHOUT   ANY  WARRANTY;   without  even   the  implied   warranty  of
  MERCHANTABILITY  or FITNESS  FOR A  PARTICULAR PURPOSE.   See  the GNU
  General Public License for more details.

  You  should have received  a copy  of the  GNU General  Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CCEVENTS_H
#define CCEVENTS_H 1


/** --------------------------------------------------------------------
 ** Headers.
 ** ----------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

#include <ccexceptions.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>

/* The  macro  CCEVENTS_UNUSED  indicates  that a  function,  function
   argument or variable may potentially be unused. Usage examples:

   static int unused_function (char arg) CCEVENTS_UNUSED;
   int foo (char unused_argument CCEVENTS_UNUSED);
   int unused_variable CCEVENTS_UNUSED;
*/
#ifdef __GNUC__
#  define CCEVENTS_UNUSED		__attribute__((unused))
#else
#  define CCEVENTS_UNUSED		/* empty */
#endif

#ifndef __GNUC__
#  define __attribute__(...)		/* empty */
#endif

/* I found  the following chunk on  the Net.  (Marco Maggi;  Sun Feb 26,
   2012) */
#if defined _WIN32 || defined __CYGWIN__
#  ifdef BUILDING_DLL
#    ifdef __GNUC__
#      define ccevents_decl		__attribute__((dllexport))
#    else
#      define ccevents_decl		__declspec(dllexport)
#    endif
#  else
#    ifdef __GNUC__
#      define ccevents_decl		__attribute__((dllimport))
#    else
#      define ccevents_decl		__declspec(dllimport)
#    endif
#  endif
#  define ccevents_private_decl	extern
#else
#  if __GNUC__ >= 4
#    define ccevents_decl		__attribute__((visibility ("default")))
#    define ccevents_private_decl	__attribute__((visibility ("hidden")))
#  else
#    define ccevents_decl		extern
#    define ccevents_private_decl	extern
#  endif
#endif


/** --------------------------------------------------------------------
 ** Initialisation.
 ** ----------------------------------------------------------------- */

ccevents_decl void ccevents_init (void);


/** --------------------------------------------------------------------
 ** Forward declarations.
 ** ----------------------------------------------------------------- */

typedef struct ccevents_sources_tag_t		ccevents_sources_t;
typedef struct ccevents_fd_source_tag_t		ccevents_fd_source_t;
typedef struct ccevents_task_source_tag_t	ccevents_task_source_t;


/** --------------------------------------------------------------------
 ** Constants.
 ** ----------------------------------------------------------------- */

#define MAX_CONSECUTIVE_FD_EVENTS	5


/** --------------------------------------------------------------------
 ** Version functions.
 ** ----------------------------------------------------------------- */

ccevents_decl const char *	cct_version_string		(void);
ccevents_decl int		cct_version_interface_current	(void);
ccevents_decl int		cct_version_interface_revision	(void);
ccevents_decl int		cct_version_interface_age	(void);


/** --------------------------------------------------------------------
 ** Exceptional conditions.
 ** ----------------------------------------------------------------- */

typedef struct ccevents_condition_base_descriptor_t {
  cce_condition_descriptor_t;
} ccevents_condition_base_descriptor_t;

ccevents_decl const ccevents_condition_base_descriptor_t * ccevents_condition_base_descriptor;

/* ------------------------------------------------------------------ */

typedef struct ccevents_condition_timeval_descriptor_t {
  cce_condition_descriptor_t;
} ccevents_condition_timeval_descriptor_t;

typedef struct ccevents_condition_timeval_t {
  cce_condition_t;
} ccevents_condition_timeval_t;

ccevents_decl const ccevents_condition_timeval_t * ccevents_condition_timeval (void)
  __attribute__((pure));
ccevents_decl const ccevents_condition_timeval_descriptor_t * ccevents_condition_timeval_descriptor;
ccevents_decl bool ccevents_condition_is_a_timeval (const cce_condition_t * C)
  __attribute__((pure,nonnull(1)));

/* ------------------------------------------------------------------ */

typedef struct ccevents_condition_timeval_invalid_descriptor_t {
  cce_condition_descriptor_t;
} ccevents_condition_timeval_invalid_descriptor_t;

typedef struct ccevents_condition_timeval_invalid_t {
  cce_condition_t;
} ccevents_condition_timeval_invalid_t;

ccevents_decl const ccevents_condition_timeval_invalid_t * ccevents_condition_timeval_invalid (void)
  __attribute__((pure));
ccevents_decl const ccevents_condition_timeval_invalid_descriptor_t * ccevents_condition_timeval_invalid_descriptor;
ccevents_decl bool ccevents_condition_is_a_timeval_invalid (const cce_condition_t * C)
  __attribute__((pure,nonnull(1)));

/* ------------------------------------------------------------------ */

typedef struct ccevents_condition_timeval_overflow_descriptor_t {
  cce_condition_descriptor_t;
} ccevents_condition_timeval_overflow_descriptor_t;

typedef struct ccevents_condition_timeval_overflow_t {
  cce_condition_t;
} ccevents_condition_timeval_overflow_t;

ccevents_decl const ccevents_condition_timeval_overflow_t * ccevents_condition_timeval_overflow (void)
  __attribute__((pure));
ccevents_decl const ccevents_condition_timeval_overflow_descriptor_t * ccevents_condition_timeval_overflow_descriptor;
ccevents_decl bool ccevents_condition_is_a_timeval_overflow (const cce_condition_t * C)
  __attribute__((pure,nonnull(1)));

/* ------------------------------------------------------------------ */

typedef struct ccevents_condition_descriptor_timeout_invalid_t {
  cce_condition_descriptor_t;
} ccevents_condition_descriptor_timeout_invalid_t;

typedef struct ccevents_condition_timeout_invalid_t {
  cce_condition_t;
} ccevents_condition_timeout_invalid_t;

ccevents_decl const ccevents_condition_timeout_invalid_t *	ccevents_condition_timeout_invalid (void)
  __attribute__((pure));
ccevents_decl const ccevents_condition_descriptor_timeout_invalid_t * ccevents_condition_timeout_invalid_descriptor;

/* ------------------------------------------------------------------ */

typedef struct ccevents_condition_descriptor_timeout_overflow_t {
  cce_condition_descriptor_t;
} ccevents_condition_descriptor_timeout_overflow_t;

typedef struct ccevents_condition_timeout_overflow_t {
  cce_condition_t;
} ccevents_condition_timeout_overflow_t;

ccevents_decl const ccevents_condition_timeout_overflow_t * ccevents_condition_timeout_overflow (void)
  __attribute__((pure));
ccevents_decl const ccevents_condition_descriptor_timeout_overflow_t * ccevents_condition_timeout_overflow_descriptor;


/** --------------------------------------------------------------------
 ** Timeval handling.
 ** ----------------------------------------------------------------- */

/* This is like  "struct timeval" but it is guaranteed  to be normalised
 * such that:
 *
 *    0 <= tv_sec  <= LONG_MAX
 *    0 <= tv_usec <= 999999
 */
typedef struct ccevents_timeval_t {
  struct timeval;
} ccevents_timeval_t;

ccevents_decl ccevents_timeval_t ccevents_timeval_init (cce_location_tag_t * there, long seconds, long microseconds)
  /* Not "pure" or "const" because it might perform a non-local exit. */
  __attribute__((nonnull(1)));

ccevents_decl ccevents_timeval_t ccevents_timeval_normalise (cce_location_tag_t * there, struct timeval T)
  /* Not "pure" or "const" because it might perform a non-local exit. */
  __attribute__((nonnull(1)));

ccevents_decl ccevents_timeval_t ccevents_timeval_add (cce_location_tag_t * there,
						       ccevents_timeval_t A, ccevents_timeval_t B)
  /* Not "pure" or "const" because it might perform a non-local exit. */
  __attribute__((nonnull(1)));

ccevents_decl ccevents_timeval_t ccevents_timeval_sub (cce_location_tag_t * there,
						       ccevents_timeval_t A, ccevents_timeval_t B)
  /* Not "pure" or "const" because it might perform a non-local exit. */
  __attribute__((nonnull(1)));

ccevents_decl int ccevents_timeval_compare (ccevents_timeval_t A, ccevents_timeval_t B)
  __attribute__((const));


/** --------------------------------------------------------------------
 ** Timeouts representation.
 ** ----------------------------------------------------------------- */

typedef struct ccevents_timeout_tag_t {
  /* When this timeout is started the  expiration time is stored in this
     structure.   When  the "tv_sec"  field  is  set to  LONG_MAX:  this
     timeout never expires. */
  ccevents_timeval_t;
  /* Seconds count, a  non-negative number.  When set  to LONG_MAX: this
     timeout never expires. */
  long int		seconds;
  /* Milliseconds  count.  After  normalisation:  this value  is in  the
     range [0, 999]. */
  long int		milliseconds;
  /* Microseconds  count.  After  normalisation:  this value  is in  the
     range [0, 999]. */
  long int		microseconds;
} ccevents_timeout_t;

/* Pointer   to   a   constant,   statically   allocated   instance   of
   "ccevents_timeout_t" representing a timeout that never expires. */
ccevents_decl const ccevents_timeout_t * CCEVENTS_TIMEOUT_NEVER;
/* Pointer   to   a   constant,   statically   allocated   instance   of
   "ccevents_timeout_t"    representing   a    timeout   that    expires
   immediately. */
ccevents_decl const ccevents_timeout_t * CCEVENTS_TIMEOUT_NOW;

ccevents_decl void ccevents_timeout_init (cce_location_tag_t * there, ccevents_timeout_t * to,
					  long seconds, long milliseconds, long microseconds)
  /* Not "pure" or "const" because:  it mutates the structure referenced
     by "to"; it might perfor a non-local exit. */
  __attribute__((nonnull(1,2)));

ccevents_decl ccevents_timeval_t ccevents_timeout_time_span (const ccevents_timeout_t * to)
  __attribute__((pure,nonnull(1)));

ccevents_decl ccevents_timeval_t ccevents_timeout_time (const ccevents_timeout_t * to)
  __attribute__((pure,nonnull(1)));

ccevents_decl bool ccevents_timeout_infinite_time_span (const ccevents_timeout_t * to)
  __attribute__((pure,nonnull(1)));

ccevents_decl bool ccevents_timeout_expired (const ccevents_timeout_t * to)
  __attribute__((nonnull(1)));

ccevents_decl int ccevents_timeout_compare_time_span (const ccevents_timeout_t * toA, const ccevents_timeout_t * toB)
  __attribute__((pure,nonnull(1,2)));

ccevents_decl int ccevents_timeout_compare_expiration_time (const ccevents_timeout_t * A, const ccevents_timeout_t * B)
  __attribute__((pure,nonnull(1,2)));

ccevents_decl void ccevents_timeout_start (cce_location_tag_t * there, ccevents_timeout_t * to)
  __attribute__((nonnull(1,2)));

ccevents_decl void ccevents_timeout_reset (ccevents_timeout_t * to)
  __attribute__((nonnull(1)));

static inline long int __attribute__((pure,nonnull(1)))
ccevents_timeout_seconds (const ccevents_timeout_t * to)
{
  return to->seconds;
}
static inline long int __attribute__((pure,nonnull(1)))
ccevents_timeout_milliseconds (const ccevents_timeout_t * to)
{
  return to->milliseconds;
}
static inline int __attribute__((pure,nonnull(1)))
ccevents_timeout_microseconds (const ccevents_timeout_t * to)
{
  return to->microseconds;
}


/** --------------------------------------------------------------------
 ** File descriptor events sources.
 ** ----------------------------------------------------------------- */

typedef bool ccevents_fd_source_query_fun_t		(cce_location_tag_t * L,
							 ccevents_fd_source_t * fds);
typedef void ccevents_fd_source_handler_fun_t		(cce_location_tag_t * L,
							 ccevents_fd_source_t * fds);
typedef void ccevents_fd_source_expiration_handler_fun_t(ccevents_fd_source_t * fds);

struct ccevents_fd_source_tag_t {
  /* The file descriptor. */
  int				fd;

  /* A thunk to be called to  query the file descriptor for the expected
     event. */
  ccevents_fd_source_query_fun_t *	query;

  /* A thunk to be called whenever the expected event happens. */
  ccevents_fd_source_handler_fun_t *	handler;

  /* False or  a TIME struct  representing the expiration time  for this
     event. */
  ccevents_timeout_t			expiration_time;

  /* False  or a  thunk to be called whenever this event expires. */
  ccevents_fd_source_expiration_handler_fun_t *	expiration_handler;

  ccevents_fd_source_t *	next;
  ccevents_fd_source_t *	prev;
};

ccevents_decl void ccevents_fd_event_source_init (ccevents_fd_source_t * fds,
						  int fd,
						  ccevents_fd_source_query_fun_t * query_fd_fun,
						  ccevents_fd_source_handler_fun_t * event_handler_fun,
						  ccevents_timeout_t expiration_time,
						  ccevents_fd_source_expiration_handler_fun_t * expiration_handler)
  __attribute__((nonnull(1, 3, 4, 6)));

ccevents_decl void ccevents_fd_event_source_register (ccevents_sources_t * sources, ccevents_fd_source_t * fds)
  __attribute__((nonnull(1,2)));

ccevents_decl void ccevents_fd_event_source_forget (ccevents_sources_t * sources, ccevents_fd_source_t * fds)
  __attribute__((nonnull(1,2)));

ccevents_decl bool ccevents_query_fd_readability (cce_location_tag_t * there, ccevents_fd_source_t * fds)
  __attribute__((nonnull(1,2)));
ccevents_decl bool ccevents_query_fd_writability (cce_location_tag_t * there, ccevents_fd_source_t * fds)
  __attribute__((nonnull(1,2)));
ccevents_decl bool ccevents_query_fd_exception   (cce_location_tag_t * there, ccevents_fd_source_t * fds)
  __attribute__((nonnull(1,2)));

ccevents_decl bool ccevents_fd_source_do_one_event (cce_location_tag_t * there, ccevents_fd_source_t * fds)
  __attribute__((nonnull(1,2)));


/** --------------------------------------------------------------------
 ** Task fragment event sources.
 ** ----------------------------------------------------------------- */

struct ccevents_task_source_tag_t {
  ccevents_task_source_t *	prev;
  ccevents_task_source_t *	next;
};


/** --------------------------------------------------------------------
 ** Interprocess signal event sources.
 ** ----------------------------------------------------------------- */

typedef void ccevents_signal_handler_t (void);


/** --------------------------------------------------------------------
 ** Events sources.
 ** ----------------------------------------------------------------- */

struct ccevents_sources_tag_t {
  /* Boolean.  True if  a request to leave the loop  as soon as possible
     was posted. */
  bool		break_flag;

  ccevents_signal_handler_t *	signal_handlers;
  /* Vector of  null or lists.   Each list contains  interprocess signal
     handlers in the form of thunks to be run once. */

  /* Non-negative fixnum.  Count of consecutive fd events served. */
  size_t	fds_count;

  /* Non-negative fixnum.   Maximum number  of consecutive fd  events to
     serve.  When the count reaches the watermark level: the loop avoids
     servicing  fd events  and  tries  to serve  an  event from  another
     source. */
  size_t	fds_watermark;

  /* Reverse list of fd entries already queried for the current run over
     fd event sources. */
  ccevents_fd_source_t *	fds_head;

  /* List of fd entries still to query  in the current run over fd event
     sources. */
  ccevents_fd_source_t *	fds_tail;

  /* Reverse list  of task entries  already queried for the  current run
     over task event sources. */
  ccevents_task_source_t *	tasks_rev_head;

  /* List of  task entries still to  query in the current  run over task
     event sources. */
  ccevents_task_source_t *	tasks_tail;
};


/** --------------------------------------------------------------------
 ** Done.
 ** ----------------------------------------------------------------- */

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CCEVENTS_H */

/* end of file */
