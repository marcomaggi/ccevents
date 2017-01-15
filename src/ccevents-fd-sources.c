/*
  Part of: CCEvents
  Contents: file descriptor events sources
  Date: Sun Jan 15, 2017

  Abstract



  Copyright (C) 2017 Marco Maggi <marco.maggi-ipsu@poste.it>

  This is free software; you can  redistribute it and/or modify it under
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
#include <errno.h>
#include <sys/select.h>

void
ccevents_fd_event_source_init (ccevents_fd_source_t * fds,
			       int fd,
			       ccevents_fd_source_query_fun_t * query_fd_fun,
			       ccevents_fd_source_handler_fun_t * event_handler_fun,
			       ccevents_absolute_time_t expiration_time,
			       ccevents_fd_source_expiration_handler_fun_t * expiration_handler)
/* Initialise an already allocated fd source.

   SOURCES is  a pointer to the  events sources collector to  which this
   new event source will belong.

   FDS is a pointer to an already allocated fd events source struct.

   FD is the file descriptor.

   QUERY_FD_FUN is  a pointer  to the  function used  to query  the file
   descriptor  for  events.   This  function decides  which  events  are
   expected: readability, writability, exceptional condition.

   EVENT_HANDLER_FUN is a pointer to the  function used to handle a file
   descriptor event.

   EXPIRATION_TIME is the expiration time for the current event waiting.

   EXPIRATION_HANDLER  is  a pointer  to  the  function used  to  handle
   expired waiting for the next event. */
{
  fds->fd			= fd;
  fds->query			= query_fd_fun;
  fds->handler			= event_handler_fun;
  fds->expiration_time		= expiration_time;
  fds->expiration_handler	= expiration_handler;
}

void
ccevents_fd_event_source_register (ccevents_sources_t * sources, ccevents_fd_source_t * fds)
/* Push a new file descriptor events source on the list of fd sources in
   the group SOURCES. */
{
  if (sources->fds_head) {
    fds->prev			= NULL;
    fds->next			= sources->fds_head;
    sources->fds_head->prev	= fds;
  }
}

void
ccevents_fd_event_source_forget (ccevents_sources_t * sources, ccevents_fd_source_t * fds)
/* Remove a file  descriptor events source from the  group SOURCES.  The
   file descriptor source is *not* finalised in any way.

   SOURCES is a  pointer to the events sources collector  from which the
   event source will be removed.

   FDS is a pointer to the fd events source struct. */
{
  for (ccevents_fd_source_t * iter = sources->fds_head; iter; iter = iter->next) {
    if (iter == fds) {
      if (iter->prev) {
	iter->prev->next = iter->next;
      }
      if (iter->next) {
	iter->next->prev = iter->prev;
      }
    }
  }
  fds->prev	= NULL;
  fds->next	= NULL;
}

bool
ccevents_query_fd_readability (cce_location_tag_t * there, ccevents_fd_source_t * fds)
/* Query a file descriptor for readability. */
{
  static struct timeval	timeout = { 0, 0 };
  fd_set	set;
  int		rv;
  FD_ZERO(&set);
  FD_SET(fds->fd, &set);
  errno = 0;
  rv = select(1+(fds->fd), &set, NULL, NULL, &timeout);
  if (-1 == rv) {
    /* An error occurred. */
    cce_raise(there, cce_errno_condition(rv));
  } else {
    /* Success.  RV  contains the number  of file descriptors  ready for
       reading; in this case it can be only 1 or 0. */
    return (1 == rv)? true : false;
  }
}

bool
ccevents_query_fd_writability (cce_location_tag_t * there, ccevents_fd_source_t * fds)
/* Query a file descriptor for writability. */
{
  static struct timeval	timeout = { 0, 0 };
  fd_set	set;
  int		rv;
  FD_ZERO(&set);
  FD_SET(fds->fd, &set);
  errno = 0;
  rv = select(1+(fds->fd), NULL, &set, NULL, &timeout);
  if (-1 == rv) {
    /* An error occurred. */
    cce_raise(there, cce_errno_condition(rv));
  } else {
    /* Success.  RV  contains the number  of file descriptors  ready for
       writing; in this case it can be only 1 or 0. */
    return (1 == rv)? true : false;
  }
}

bool
ccevents_query_fd_exception (cce_location_tag_t * there, ccevents_fd_source_t * fds)
/* Query a file descriptor for exception. */
{
  static struct timeval	timeout = { 0, 0 };
  fd_set	set;
  int		rv;
  FD_ZERO(&set);
  FD_SET(fds->fd, &set);
  errno = 0;
  rv = select(1+(fds->fd), NULL, NULL, &set, &timeout);
  if (-1 == rv) {
    /* An error occurred. */
    cce_raise(there, cce_errno_condition(rv));
  } else {
    /* Success.   RV  contains  the  number of  file  descriptors  which
       received an exception; in this case it can be only 1 or 0. */
    return (1 == rv)? true : false;
  }
}

bool
ccevents_fd_source_do_one_event (cce_location_tag_t * there, ccevents_fd_source_t * fds)
/* Consume one event,  if any, and return.  Return a  boolean, "true" if
   one event was served; otherwise "false".

   Exceptions raised while querying an  event source or serving an event
   handler are catched and ignored. */
{
  bool		pending;
  bool		rv = false;

  /* Query the file descriptor for a pending event. */
  {
    cce_location_t	L;

    if (cce_location(L)) {
      /* Error while querying for events. */
      cce_run_error_handlers(L);
      cce_raise(there, cce_location_condition(L));
    } else {
      pending	= fds->query(L, fds);
      cce_run_cleanup_handlers(L);
    }
  }
  if (pending) {
    cce_location_t	L;
    if (cce_location(L)) {
      /* Error while handling event. */
      cce_run_error_handlers(L);
      cce_raise(there, cce_location_condition(L));
    } else {
      fds->handler(L, fds);
      cce_run_cleanup_handlers(L);
      rv = true;
    }
  }
  return rv;
}

/*
(define (do-one-fd-event)
  ;;Consume one event, if any, and  return.  Return a boolean, #t if one
  ;;event was served.
  ;;
  ;;Exceptions raised while querying an event source or serving an event
  ;;handler are catched and ignored.
  ;;
  (with-event-sources (SOURCES)
    (when (and (null? SOURCES.fds.tail)
	       (not (null? SOURCES.fds.rev-head)))
      (set! SOURCES.fds.tail     (reverse SOURCES.fds.rev-head))
      (set! SOURCES.fds.rev-head '()))
    (if (null? SOURCES.fds.tail)
	#f
      (if ($fx< SOURCES.fds.count SOURCES.fds.watermark)
	  (let ((E ($car SOURCES.fds.tail)))
	    (set! SOURCES.fds.tail ($cdr SOURCES.fds.tail))
	    (cond
	     ;;Check whether the event happened.   If it has: invoke the
	     ;;associated handler.
	     ;;
	     ((%catch (($fd-entry-query E)))
	      (guard (E (else
			 ;;(pretty-print E (current-error-port))
			 #f))
		(($fd-entry-handler E))
		($fxincr! SOURCES.fds.count)
		#t))

	     ;;If an expiration time has  been set: check it against the
	     ;;current  time.   If  this  even is  expired:  invoke  the
	     ;;associated handler.
	     ;;
	     ((let ((T ($fd-entry-expiration-time E)))
		(and T (time<=? T (current-time))))
	      ($fxincr! SOURCES.fds.count)
	      (($fd-entry-expiration-handler E))
	      #t)

	     ;;The event  did not happen;  re-enqueue the event  for the
	     ;;next loop.
	     ;;
	     (else
	      (set! SOURCES.fds.rev-head (cons E SOURCES.fds.rev-head))
	      (if (null? SOURCES.fds.tail)
		  (begin
		    (set! SOURCES.fds.count 0)
		    #f)
		(do-one-fd-event)))))
	(begin
	  (set! SOURCES.fds.count 0)
	  #f)))))

(define readable
  (case-lambda
   ((port/fd handler-thunk)
    (readable port/fd handler-thunk #f #f))
   ((port/fd handler-thunk expiration-time expiration-thunk)
    (define who 'readable)
    (with-arguments-validation (who)
	((port/file-descriptor	port/fd)
	 (procedure		handler-thunk)
	 (time/false		expiration-time)
	 (procedure/false	expiration-thunk))
      (let ((fd (if (port? port/fd)
		    (port-fd port/fd)
		  port/fd)))
	(%enqueue-fd-event-source fd
				  (lambda ()
				    (px.select-fd-readable? fd 0 0))
				  handler-thunk expiration-time expiration-thunk))))))

(define writable
  (case-lambda
   ((port/fd handler-thunk)
    (writable port/fd handler-thunk #f #f))
   ((port/fd handler-thunk expiration-time expiration-thunk)
    (define who 'writable)
    (with-arguments-validation (who)
	((port/file-descriptor	port/fd)
	 (procedure		handler-thunk)
	 (time/false		expiration-time)
	 (procedure/false	expiration-thunk))
      (let ((fd (if (port? port/fd)
		    (port-fd port/fd)
		  port/fd)))
	(%enqueue-fd-event-source fd (lambda ()
				       (px.select-fd-writable? fd 0 0))
				  handler-thunk expiration-time expiration-thunk))))))

(define exception
  (case-lambda
   ((port/fd handler-thunk)
    (exception port/fd handler-thunk #f #f))
   ((port/fd handler-thunk expiration-time expiration-thunk)
    (define who 'exception)
    (with-arguments-validation (who)
	((port/file-descriptor	port/fd)
	 (procedure		handler-thunk)
	 (time/false		expiration-time)
	 (procedure/false	expiration-thunk))
      (let ((fd (if (port? port/fd)
		    (port-fd port/fd)
		  port/fd)))
	(%enqueue-fd-event-source fd (lambda ()
				       (px.select-fd-exceptional? fd 0 0))
				  handler-thunk expiration-time expiration-thunk))))))

(define (forget-fd port/fd)
  (define who 'exception)
  (with-arguments-validation (who)
      ((port/file-descriptor	port/fd))
    (let ((fd (if (port? port/fd)
		  (port-fd port/fd)
		port/fd)))
      (with-event-sources (SOURCES)
	(set! SOURCES.fds.tail (remp (lambda (E)
				       ($fx= fd ($fd-entry-fd E)))
				 SOURCES.fds.tail))
	(set! SOURCES.fds.rev-head (remp (lambda (E)
					   ($fx= fd ($fd-entry-fd E)))
				     SOURCES.fds.rev-head))))))
*/

/* end of file */
