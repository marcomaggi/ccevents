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
#include <stddef.h>
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

/* Pointer cast macro helper. */
#define CCEVENTS_PC(TYPE,X,Y)		TYPE * X = (TYPE *) (Y)


/** --------------------------------------------------------------------
 ** Initialisation.
 ** ----------------------------------------------------------------- */

ccevents_decl void ccevents_init (void)
  __attribute__((constructor));


/** --------------------------------------------------------------------
 ** Forward declarations.
 ** ----------------------------------------------------------------- */

typedef struct ccevents_loop_t			ccevents_loop_t;
typedef struct ccevents_group_t			ccevents_group_t;
typedef struct ccevents_source_t		ccevents_source_t;
typedef struct ccevents_fd_source_t		ccevents_fd_source_t;
typedef struct ccevents_task_source_t		ccevents_task_source_t;
typedef struct ccevents_signal_bub_source_t	ccevents_signal_bub_source_t;
typedef struct ccevents_timer_source_t		ccevents_timer_source_t;

typedef struct ccevents_queue_node_t		ccevents_queue_node_t;
typedef struct ccevents_queue_t			ccevents_queue_t;


/** --------------------------------------------------------------------
 ** Version functions.
 ** ----------------------------------------------------------------- */

ccevents_decl const char *	ccevents_version_string			(void);
ccevents_decl int		ccevents_version_interface_current	(void);
ccevents_decl int		ccevents_version_interface_revision	(void);
ccevents_decl int		ccevents_version_interface_age		(void);


/** --------------------------------------------------------------------
 ** Queues.
 ** ----------------------------------------------------------------- */

/* This "queue"  is in truth a doubly linked circular list:
 *
 *  ------------------
 * | ccevents_queue_t |
 *  ------------------
 *          |
 *          v
 *  -----------------------      -----------------------
 * | ccevents_queue_node_t |<-->| ccevents_queue_node_t |
 *  -----------------------      -----------------------
 *          ^                              ^
 *          |                              |
 *          v                              v
 *  -----------------------      -----------------------
 * | ccevents_queue_node_t |<-->| ccevents_queue_node_t |
 *  -----------------------      -----------------------
 *
 */

struct ccevents_queue_node_t {
  /* Pointer to the base structure to which this node belongs. */
  ccevents_queue_t *		base;
  /* Pointers to the  next and previous node in the  doubly linked list.
     NULL if there is no prev/next. */
  ccevents_queue_node_t *	next;
  ccevents_queue_node_t *	prev;
};

struct ccevents_queue_t {
  /* Pointer to the current node in the list. */
  ccevents_queue_node_t *	current;
  size_t			number_of_nodes;
};

ccevents_decl void ccevents_queue_init (ccevents_queue_t * Q)
  __attribute__((nonnull(1)));

ccevents_decl void ccevents_queue_enqueue (ccevents_queue_t * Q, ccevents_queue_node_t * new_current)
  __attribute__((nonnull(1,2)));

ccevents_decl ccevents_queue_node_t * ccevents_queue_dequeue (ccevents_queue_t * Q)
  __attribute__((nonnull(1)));

ccevents_decl void ccevents_queue_advance_current (ccevents_queue_t * Q)
  __attribute__((nonnull(1)));

__attribute__((pure,nonnull(1),always_inline))
static inline ccevents_queue_node_t *
ccevents_queue_current_node (const ccevents_queue_t * Q)
{
  return Q->current;
}

__attribute__((pure,nonnull(1),always_inline))
static inline bool
ccevents_queue_is_not_empty (const ccevents_queue_t * Q)
{
  return (Q->current)? true : false;
}

__attribute__((pure,nonnull(1),always_inline))
static inline size_t
ccevents_queue_number_of_items (const ccevents_queue_t * Q)
{
  return Q->number_of_nodes;
}

__attribute__((pure,nonnull(1,2),always_inline))
static inline bool
ccevents_queue_contains_item (const ccevents_queue_t * Q, const ccevents_queue_node_t * N)
{
  return (N->base == Q)? true : false;
}

/* ------------------------------------------------------------------ */

ccevents_decl void ccevents_queue_node_init (ccevents_queue_node_t * N)
  __attribute__((nonnull(1)));

ccevents_decl void ccevents_queue_node_dequeue_itself (ccevents_queue_node_t * N)
  __attribute__((nonnull(1)));

__attribute__((nonnull(1),always_inline))
static inline bool
ccevents_queue_node_is_enqueued (const ccevents_queue_node_t * N)
{
  return (N->base)? true : false;
}


/** --------------------------------------------------------------------
 ** Exceptional conditions.
 ** ----------------------------------------------------------------- */

typedef struct ccevents_base_D_t {
  cce_condition_descriptor_t;
} ccevents_base_D_t;

ccevents_decl const ccevents_base_D_t * ccevents_base_D;

__attribute__((pure,nonnull(1),always_inline))
static inline bool
ccevents_is_a_base_C (const cce_condition_t * C)
{
  return cce_is_a_condition(C, ccevents_base_D);
}

/* ------------------------------------------------------------------ */

typedef struct ccevents_timeval_D_t {
  cce_condition_descriptor_t;
} ccevents_timeval_D_t;

typedef struct ccevents_timeval_C_t {
  cce_condition_t;
} ccevents_timeval_C_t;

ccevents_decl const ccevents_timeval_C_t * ccevents_timeval_C (void)
  __attribute__((leaf,pure));
ccevents_decl const ccevents_timeval_D_t * ccevents_timeval_D;

__attribute__((pure,nonnull(1),always_inline))
static inline bool
ccevents_is_a_timeval_C (const cce_condition_t * C)
{
  return cce_is_a_condition(C, ccevents_timeval_D);
}

/* ------------------------------------------------------------------ */

typedef struct ccevents_timeval_invalid_D_t {
  cce_condition_descriptor_t;
} ccevents_timeval_invalid_D_t;

typedef struct ccevents_timeval_invalid_C_t {
  cce_condition_t;
} ccevents_timeval_invalid_C_t;

ccevents_decl const ccevents_timeval_invalid_C_t * ccevents_timeval_invalid_C (void)
  __attribute__((leaf,pure));
ccevents_decl const ccevents_timeval_invalid_D_t * ccevents_timeval_invalid_D;

__attribute__((pure,nonnull(1),always_inline))
static inline bool
ccevents_is_a_timeval_invalid_C (const cce_condition_t * C)
{
  return cce_is_a_condition(C, ccevents_timeval_invalid_D);
}

/* ------------------------------------------------------------------ */

typedef struct ccevents_timeval_overflow_D_t {
  cce_condition_descriptor_t;
} ccevents_timeval_overflow_D_t;

typedef struct ccevents_timeval_overflow_C_t {
  cce_condition_t;
} ccevents_timeval_overflow_C_t;

ccevents_decl const ccevents_timeval_overflow_C_t * ccevents_timeval_overflow_C (void)
  __attribute__((leaf,pure));
ccevents_decl const ccevents_timeval_overflow_D_t * ccevents_timeval_overflow_D;

__attribute__((pure,nonnull(1),always_inline))
static inline bool
ccevents_is_a_timeval_overflow_C (const cce_condition_t * C)
{
  return cce_is_a_condition(C, ccevents_timeval_overflow_D);
}

/* ------------------------------------------------------------------ */

typedef struct ccevents_timeout_invalid_D_t {
  cce_condition_descriptor_t;
} ccevents_timeout_invalid_D_t;

typedef struct ccevents_timeout_invalid_C_t {
  cce_condition_t;
} ccevents_timeout_invalid_C_t;

ccevents_decl const ccevents_timeout_invalid_C_t * ccevents_timeout_invalid_C (void)
  __attribute__((leaf,pure));
ccevents_decl const ccevents_timeout_invalid_D_t * ccevents_timeout_invalid_D;

__attribute__((pure,nonnull(1),always_inline))
static inline bool
ccevents_is_a_timeout_invalid_C (const cce_condition_t * C)
{
  return cce_is_a_condition(C, ccevents_timeout_invalid_D);
}

/* ------------------------------------------------------------------ */

typedef struct ccevents_timeout_overflow_D_t {
  cce_condition_descriptor_t;
} ccevents_timeout_overflow_D_t;

typedef struct ccevents_timeout_overflow_C_t {
  cce_condition_t;
} ccevents_timeout_overflow_C_t;

ccevents_decl const ccevents_timeout_overflow_C_t * ccevents_timeout_overflow_C (void)
  __attribute__((leaf,pure));
ccevents_decl const ccevents_timeout_overflow_D_t * ccevents_timeout_overflow_D;

__attribute__((pure,nonnull(1),always_inline))
static inline bool
ccevents_is_a_timeout_overflow_C (const cce_condition_t * C)
{
  return cce_is_a_condition(C, ccevents_timeout_overflow_D);
}


/** --------------------------------------------------------------------
 ** Timeval handling.
 ** ----------------------------------------------------------------- */

typedef struct ccevents_timeval_t	ccevents_timeval_t;

/* This is like  "struct timeval" but it is guaranteed  to be normalised
 * such that:
 *
 *    0 <= tv_sec  <= LONG_MAX
 *    0 <= tv_usec <= 999999
 */
struct ccevents_timeval_t {
  struct timeval;
};

/* Pointer   to   a   constant,   statically   allocated   instance   of
   "ccevents_timeval_t"  representing  a  conventionally  infinite  time
   span. */
ccevents_decl const ccevents_timeval_t * CCEVENTS_TIMEVAL_NEVER;

ccevents_decl ccevents_timeval_t ccevents_timeval_init (cce_location_t * there, long seconds, long microseconds)
  __attribute__((nonnull(1)));

ccevents_decl ccevents_timeval_t ccevents_timeval_normalise (cce_location_t * there, struct timeval T)
  __attribute__((nonnull(1)));

ccevents_decl ccevents_timeval_t ccevents_timeval_add (cce_location_t * there,
						       const ccevents_timeval_t A, const ccevents_timeval_t B)
  __attribute__((nonnull(1)));

ccevents_decl ccevents_timeval_t ccevents_timeval_sub (cce_location_t * there,
						       const ccevents_timeval_t A, const ccevents_timeval_t B)
  __attribute__((nonnull(1)));

ccevents_decl int ccevents_timeval_compare (const ccevents_timeval_t A, const ccevents_timeval_t B)
  __attribute__((leaf,const));

ccevents_decl bool ccevents_timeval_is_expired_timeout (const ccevents_timeval_t expiration_time)
  __attribute__((leaf,pure));

ccevents_decl bool ccevents_timeval_is_infinite_timeout  (const ccevents_timeval_t T)
  __attribute__((leaf,pure));


/** --------------------------------------------------------------------
 ** Timeouts representation.
 ** ----------------------------------------------------------------- */

typedef struct ccevents_timeout_t		ccevents_timeout_t;

struct ccevents_timeout_t {
  /* Seconds count,  a non-negative number.   When set to  LONG_MAX: the
     timeout never expires. */
  long int		seconds;
  /* Milliseconds  count.  After  normalisation:  this value  is in  the
     range [0, 999]. */
  long int		milliseconds;
  /* Microseconds  count.  After  normalisation:  this value  is in  the
     range [0, 999]. */
  long int		microseconds;
};

/* Pointer   to   a   constant,   statically   allocated   instance   of
   "ccevents_timeout_t" representing a timeout that never expires. */
ccevents_decl const ccevents_timeout_t * CCEVENTS_TIMEOUT_NEVER;
/* Pointer   to   a   constant,   statically   allocated   instance   of
   "ccevents_timeout_t"    representing   a    timeout   that    expires
   immediately. */
ccevents_decl const ccevents_timeout_t * CCEVENTS_TIMEOUT_NOW;

ccevents_decl ccevents_timeout_t ccevents_timeout_init (cce_location_t * there,
							long seconds, long milliseconds, long microseconds)
  __attribute__((nonnull(1)));

ccevents_decl ccevents_timeval_t ccevents_timeout_start (cce_location_t * there, const ccevents_timeout_t to)
  __attribute__((nonnull(1)));

ccevents_decl int ccevents_timeout_compare (const ccevents_timeout_t toA, const ccevents_timeout_t toB)
  __attribute__((leaf,const));

__attribute__((const,always_inline))
static inline long int
ccevents_timeout_seconds (const ccevents_timeout_t to)
{
  return to.seconds;
}

__attribute__((const,always_inline))
static inline long int
ccevents_timeout_milliseconds (const ccevents_timeout_t to)
{
  return to.milliseconds;
}

__attribute__((const,always_inline))
static inline long int
ccevents_timeout_microseconds (const ccevents_timeout_t to)
{
  return to.microseconds;
}

__attribute__((const,always_inline))
static inline bool
ccevents_timeout_is_infinite (const ccevents_timeout_t to)
{
  return (LONG_MAX == to.seconds);
}


/** --------------------------------------------------------------------
 ** Base events sources.
 ** ----------------------------------------------------------------- */

typedef void ccevents_source_final_t	(ccevents_source_t * src)
  __attribute__((nonnull(1)));

typedef struct ccevents_source_otable_t		ccevents_source_otable_t;

struct ccevents_source_otable_t {
  ccevents_source_final_t *		final;
};

ccevents_decl const ccevents_source_otable_t * const	ccevents_source_otable_default;

/* ------------------------------------------------------------------ */

typedef bool ccevents_event_inquirer_t	(cce_location_t * L, ccevents_source_t * src)
  __attribute__((nonnull(1,2)));

typedef void ccevents_event_handler_t	(cce_location_t * L, ccevents_source_t * src)
  __attribute__((nonnull(1,2)));

typedef void ccevents_timeout_handler_t	(cce_location_t * L, ccevents_source_t * src)
  __attribute__((nonnull(1,2)));

typedef struct ccevents_source_vtable_t		 ccevents_source_vtable_t;

struct ccevents_source_vtable_t {
  ccevents_event_inquirer_t *	event_inquirer;
  ccevents_event_handler_t *	event_handler;
};

/* ------------------------------------------------------------------ */

struct ccevents_source_t {
  /* This struct is a node in the  list of event sources registered in a
     group. */
  ccevents_queue_node_t;

  const ccevents_source_otable_t *	otable;
  const ccevents_source_vtable_t *	vtable;

  /* The expiration time for the next event in this event source. */
  ccevents_timeval_t			expiration_time;
  /* The expiration handler for this  event source.  Pointer to function
     to be called whenever this event expires. */
  ccevents_timeout_handler_t *		expiration_handler;

  /* Nested  anonymous struct  defined as  a bitfield.   Everybody hates
     bitfields, but I do not care.  I'm bad. */
  struct {
    int		enabled: 1;
  };
};

ccevents_decl ccevents_event_inquirer_t		ccevents_source_query;
ccevents_decl ccevents_event_handler_t		ccevents_source_handle_event;
ccevents_decl ccevents_timeout_handler_t	ccevents_source_handle_expiration;

ccevents_decl void ccevents_source_init (ccevents_source_t * src, const ccevents_source_vtable_t * vtable)
  __attribute__((nonnull(1)));

ccevents_decl void ccevents_source_final (ccevents_source_t * src);

ccevents_decl void ccevents_source_set_otable (ccevents_source_t * src, const ccevents_source_otable_t * otable)
  __attribute__((nonnull(1,2)));

ccevents_decl void ccevents_source_set_timeout (ccevents_source_t * src,
						ccevents_timeval_t expiration_time,
						ccevents_timeout_handler_t * expiration_handler)
  __attribute__((leaf,nonnull(1,3)));

ccevents_decl bool ccevents_source_do_one_event (cce_location_t * there, ccevents_source_t * src)
  __attribute__((nonnull(1,2)));

/* ------------------------------------------------------------------ */

__attribute__((nonnull(1),always_inline))
static inline void
ccevents_source_dequeue_itself (ccevents_source_t * src)
{
  ccevents_queue_node_dequeue_itself(src);
}

/* ------------------------------------------------------------------ */

__attribute__((pure,nonnull(1),always_inline))
static inline bool
ccevents_source_is_enqueued (const ccevents_source_t * src)
{
  return ccevents_queue_node_is_enqueued(src);
}

/* ------------------------------------------------------------------ */

__attribute__((pure,nonnull(1),always_inline))
static inline bool
ccevents_source_servicing_is_enabled (const ccevents_source_t * src)
{
  return (bool)(src->enabled);
}

__attribute__((nonnull(1),always_inline))
static inline void
ccevents_source_enable_servicing (ccevents_source_t * src)
{
  src->enabled = 1;
}

__attribute__((nonnull(1),always_inline))
static inline void
ccevents_source_disable_servicing (ccevents_source_t * src)
{
  src->enabled = 0;
}

/* ------------------------------------------------------------------ */

ccevents_decl ccevents_event_inquirer_t		ccevents_dummy_event_inquirer_true;
ccevents_decl ccevents_event_inquirer_t		ccevents_dummy_event_inquirer_false;
ccevents_decl ccevents_event_handler_t		ccevents_dummy_event_handler;
ccevents_decl ccevents_timeout_handler_t	ccevents_dummy_timeout_handler;


/** --------------------------------------------------------------------
 ** File descriptor events sources.
 ** ----------------------------------------------------------------- */

struct ccevents_fd_source_t {
  ccevents_source_t;

  /* The file descriptor. */
  int				fd;

  /* Pointer to function  to be called to query the  file descriptor for
     the expected event. */
  ccevents_event_inquirer_t *	event_inquirer;

  /* Pointer  to  function to  be  called  whenever the  expected  event
     happens. */
  ccevents_event_handler_t *		event_handler;
};

ccevents_decl void ccevents_fd_source_init (ccevents_fd_source_t * fdsrc, int fd)
  __attribute__((leaf,nonnull(1)));

ccevents_decl void ccevents_fd_source_set (ccevents_fd_source_t * fdsrc,
					   ccevents_event_inquirer_t * event_inquirer,
					   ccevents_event_handler_t * event_handler)
  __attribute__((nonnull(1,2,3)));

ccevents_decl ccevents_event_inquirer_t ccevents_query_fd_readability;
ccevents_decl ccevents_event_inquirer_t ccevents_query_fd_writability;
ccevents_decl ccevents_event_inquirer_t ccevents_query_fd_exception;

/* Output of: (my-c-insert-cast-function "ccevents" "source" "fd_source") */
__attribute__((const,always_inline))
static inline ccevents_fd_source_t *
ccevents_cast_to_fd_source_from_source (ccevents_source_t * src)
{
  return (ccevents_fd_source_t *)src;
}
#define ccevents_cast_to_fd_source(SRC)		\
  _Generic((SRC), ccevents_source_t *: ccevents_cast_to_fd_source_from_source)(SRC)
/* End of output. */


/** --------------------------------------------------------------------
 ** Task fragment event sources.
 ** ----------------------------------------------------------------- */

struct ccevents_task_source_t {
  ccevents_source_t;
  ccevents_event_inquirer_t *	event_inquirer;
  ccevents_event_handler_t *		event_handler;
};

ccevents_decl void ccevents_task_source_init (ccevents_task_source_t * tksrc)
  __attribute__((nonnull(1)));

ccevents_decl void ccevents_task_source_set (ccevents_task_source_t    * tksrc,
					     ccevents_event_inquirer_t * event_inquirer,
					     ccevents_event_handler_t  * event_handler)
  __attribute__((nonnull(1,2,3)));

/* Output of: (my-c-insert-cast-function "ccevents" "source" "task_source") */
__attribute__((const,always_inline))
static inline ccevents_task_source_t *
ccevents_cast_to_task_source_from_source (ccevents_source_t * src)
{
  return (ccevents_task_source_t *)src;
}
#define ccevents_cast_to_task_source(SRC)		\
  _Generic((SRC), ccevents_source_t *: ccevents_cast_to_task_source_from_source)(SRC)
/* End of output. */


/** --------------------------------------------------------------------
 ** Interprocess signal events sources.
 ** ----------------------------------------------------------------- */

struct ccevents_signal_bub_source_t {
  ccevents_source_t;

  /* The signal number to which this event source must react. */
  int	signum;

  /* Pointer  to  function to  be  called  whenever the  expected  event
     happens. */
  ccevents_event_handler_t *		event_handler;
};

ccevents_decl void ccevents_signal_bub_init (void)
  __attribute__((leaf));
ccevents_decl void ccevents_signal_bub_final (void)
  __attribute__((leaf));
ccevents_decl void ccevents_signal_bub_acquire (void)
  __attribute__((leaf));
ccevents_decl bool ccevents_signal_bub_delivered (int signum)
  __attribute__((leaf));

/* ------------------------------------------------------------------ */

ccevents_decl void ccevents_signal_bub_source_init (ccevents_signal_bub_source_t * sigsrc, int signum)
  __attribute__((leaf,nonnull(1)));

ccevents_decl void ccevents_signal_bub_source_set (ccevents_signal_bub_source_t * sigsrc,
						   ccevents_event_handler_t  * event_handler)
  __attribute__((nonnull(1,2)));

/* Output of: (my-c-insert-cast-function "ccevents" "source" "signal_bub_source") */
__attribute__((const,always_inline))
static inline ccevents_signal_bub_source_t *
ccevents_cast_to_signal_bub_source_from_source (ccevents_source_t * src)
{
  return (ccevents_signal_bub_source_t *)src;
}
#define ccevents_cast_to_signal_bub_source(SRC)		\
  _Generic((SRC), ccevents_source_t *: ccevents_cast_to_signal_bub_source_from_source)(SRC)
/* End of output. */


/** --------------------------------------------------------------------
 ** Timer events sources.
 ** ----------------------------------------------------------------- */

struct ccevents_timer_source_t {
  ccevents_source_t;
};

ccevents_decl void ccevents_timer_source_init (ccevents_timer_source_t * timsrc)
  __attribute__((leaf,nonnull(1)));

/* Output of: (my-c-insert-cast-function "ccevents" "source" "timer_source") */
__attribute__((const,always_inline))
static inline ccevents_timer_source_t *
ccevents_cast_to_timer_source_from_source (ccevents_source_t * src)
{
  return (ccevents_timer_source_t *)src;
}
#define ccevents_cast_to_timer_source(SRC)		\
  _Generic((SRC), ccevents_source_t *: ccevents_cast_to_timer_source_from_source)(SRC)
/* End of output. */


/** --------------------------------------------------------------------
 ** Groups of event sources.
 ** ----------------------------------------------------------------- */

/* Groups  are collection  of event  sources  that must  be queried  for
   pending events with the same priority.
*/
struct ccevents_group_t {
  ccevents_queue_node_t;

  /* Queue of event sources. */
  ccevents_queue_t	sources[1];

  /* True  if a  request  to leave  the  loop as  soon  as possible  was
     posted. */
  bool		request_to_leave;

  /* Maximum  number of  consecutive event  servicing attempts  for this
     group.   When  the count  reaches  the  watermark level:  the  loop
     abandons this group and moves on to the next. */
  size_t	servicing_attempts_watermark;
};

ccevents_decl void ccevents_group_init (ccevents_group_t * grp, size_t servicing_attempts_watermark)
  __attribute__((leaf,nonnull(1)));

ccevents_decl void ccevents_group_final (ccevents_group_t * grp)
  __attribute__((nonnull(1)));

ccevents_decl void ccevents_group_enter (ccevents_group_t * grp)
  __attribute__((nonnull(1)));

__attribute__((nonnull(1),always_inline))
static inline void
ccevents_group_post_request_to_leave (ccevents_group_t * grp)
{
  grp->request_to_leave = true;
}

/* ------------------------------------------------------------------ */

__attribute__((nonnull(1),always_inline))
static inline bool
ccevents_group_queue_is_not_empty (const ccevents_group_t * grp)
{
  return ccevents_queue_is_not_empty(grp->sources);
}

__attribute__((nonnull(1),always_inline))
static inline size_t
ccevents_group_number_of_sources (const ccevents_group_t * grp)
{
  return ccevents_queue_number_of_items(grp->sources);
}

__attribute__((nonnull(1),always_inline))
static inline bool
ccevents_group_contains_source (const ccevents_group_t * grp, const ccevents_source_t * src)
{
  return ccevents_queue_contains_item (grp->sources, src);
}

/* ------------------------------------------------------------------ */

__attribute__((nonnull(1),always_inline))
static inline void
ccevents_group_dequeue_itself (ccevents_group_t * grp)
{
  ccevents_queue_node_dequeue_itself(grp);
}

__attribute__((nonnull(1),always_inline))
static inline void
ccevents_group_enqueue_source (ccevents_group_t * grp, ccevents_source_t * new_source)
{
  ccevents_queue_enqueue(grp->sources, new_source);
}

__attribute__((nonnull(1),always_inline))
static inline ccevents_source_t *
ccevents_group_dequeue_source (ccevents_group_t * grp)
{
  CCEVENTS_PC(ccevents_source_t, src, ccevents_queue_current_node(grp->sources));
  ccevents_source_dequeue_itself(src);
  return src;
}


/** --------------------------------------------------------------------
 ** Main loop.
 ** ----------------------------------------------------------------- */

struct ccevents_loop_t {
  /* Queue of event groups. */
  ccevents_queue_t	groups[1];

  /* True  if a  request  to leave  the  loop as  soon  as possible  was
     posted. */
  bool		request_to_leave;
};

ccevents_decl void ccevents_loop_init (ccevents_loop_t * loop)
  __attribute__((leaf,nonnull(1)));

ccevents_decl void ccevents_loop_final (ccevents_loop_t * loop)
  __attribute__((nonnull(1)));

ccevents_decl void ccevents_loop_enter (ccevents_loop_t * loop)
  __attribute__((nonnull(1)));

__attribute__((pure,nonnull(1),always_inline))
static inline void
ccevents_loop_post_request_to_leave (ccevents_loop_t * loop)
/* Post a request to exit ASAP the loop for this loop. */
{
  loop->request_to_leave = true;
}

/* ------------------------------------------------------------------ */

__attribute__((nonnull(1,2),always_inline))
static inline void
ccevents_loop_enqueue_group (ccevents_loop_t * loop, ccevents_group_t * grp)
{
  ccevents_queue_enqueue(loop->groups, grp);
}

__attribute__((nonnull(1),always_inline))
static inline ccevents_group_t *
ccevents_loop_dequeue_group (ccevents_loop_t * loop)
{
  return (ccevents_group_t *)ccevents_queue_dequeue(loop->groups);
}

/* ------------------------------------------------------------------ */

__attribute__((pure,nonnull(1),always_inline))
static inline bool
ccevents_loop_queue_is_not_empty (const ccevents_loop_t * loop)
{
  return ccevents_queue_is_not_empty(loop->groups);
}

__attribute__((pure,nonnull(1),always_inline))
static inline size_t
ccevents_loop_number_of_groups (const ccevents_loop_t * loop)
{
  return ccevents_queue_number_of_items(loop->groups);
}

__attribute__((pure,nonnull(1,2),always_inline))
static inline bool
ccevents_loop_contains_group (const ccevents_loop_t * loop, const ccevents_group_t * grp)
{
  return ccevents_queue_contains_item(loop->groups, grp);
}


/** --------------------------------------------------------------------
 ** Late functions.
 ** ----------------------------------------------------------------- */

__attribute__((pure,nonnull(1),always_inline))
static inline ccevents_group_t *
ccevents_source_get_group (const ccevents_source_t * src)
/* Given a  pointer to  source: if  the source is  enqueued in  a group,
   return  the  pointer  to  the group;  otherwise  return  NULL.   This
   function is defined here because it  needs the full definition of the
   structure "ccevents_group_t". */
{
  return (src->base)? \
    (ccevents_group_t *)((uint8_t*)(src->base) - (ptrdiff_t)offsetof(struct ccevents_group_t, sources)) :
    NULL;
}

__attribute__((pure,nonnull(1),always_inline))
static inline ccevents_loop_t *
ccevents_group_get_loop (const ccevents_group_t * grp)
/* Given a pointer to group: if the  group is enqueued in a loop, return
   the pointer  to the  loop; otherwise return  NULL.  This  function is
   defined here  because it needs  the full definition of  the structure
   "ccevents_loop_t". */
{
  return (grp->base)?
    (ccevents_loop_t *)((uint8_t*)(grp->base) - (ptrdiff_t)offsetof(struct ccevents_loop_t, groups)) :
    NULL;
}


/** --------------------------------------------------------------------
 ** Done.
 ** ----------------------------------------------------------------- */

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CCEVENTS_H */

/* end of file */
