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

/** --------------------------------------------------------------------
 ** Default functions.
 ** ----------------------------------------------------------------- */

static bool
default_event_query (cce_location_t *	L	CCEVENTS_UNUSED,
		     ccevents_group_t *		grp	CCEVENTS_UNUSED,
		     ccevents_source_t *	fdsrc	CCEVENTS_UNUSED)
{
  return false;
}
static void
default_event_handler (cce_location_t *	L	CCEVENTS_UNUSED,
		       ccevents_group_t *	grp	CCEVENTS_UNUSED,
		       ccevents_source_t *	fdsrc	CCEVENTS_UNUSED)
{
  return;
}

/** --------------------------------------------------------------------
 ** Method functions.
 ** ----------------------------------------------------------------- */

static bool
method_event_inquirer (cce_location_t * L, ccevents_group_t * grp, ccevents_source_t * src)
{
  CCEVENTS_PC(ccevents_fd_source_t, fdsrc, src);
  return fdsrc->event_inquirer(L, grp, fdsrc);
}
static void
method_event_handler (cce_location_t * L, ccevents_group_t * grp, ccevents_source_t * src)
{
  CCEVENTS_PC(ccevents_fd_source_t, fdsrc, src);
  return fdsrc->event_handler(L, grp, fdsrc);
}
static const ccevents_source_vtable_t methods_table = {
  .event_inquirer	= method_event_inquirer,
  .event_handler	= method_event_handler,
};

/** --------------------------------------------------------------------
 ** Initialisation and setup.
 ** ----------------------------------------------------------------- */

void
ccevents_fd_source_init (ccevents_fd_source_t * fdsrc, int fd)
/* Initialise an  already allocated fd source.   FDSRC is a pointer  to an
   already  allocated  fd   events  source  struct.   FD   is  the  file
   descriptor. */
{
  ccevents_source_init(fdsrc, &methods_table);
  fdsrc->fd			= fd;
  fdsrc->event_inquirer		= default_event_query;
  fdsrc->event_handler		= default_event_handler;
}
void
ccevents_fd_source_set (ccevents_fd_source_t * fdsrc,
			ccevents_source_event_inquirer_fun_t * event_inquirer,
			ccevents_source_event_handler_fun_t * event_handler)
/* Set up an already initialised fd  source to wait for an event.  Start
   the expiration timer.

   FDSRC is a pointer to an already allocated fd events source struct.

   EVENT_INQUIRER is  a pointer to the  function used to query  the file
   descriptor  for  events.   This  function decides  which  events  are
   expected: readability, writability, exceptional condition.

   EVENT_HANDLER is  a pointer  to the  function used  to handle  a file
   descriptor event.
*/
{
  fdsrc->event_inquirer		= event_inquirer;
  fdsrc->event_handler		= event_handler;
}

/** --------------------------------------------------------------------
 ** Predefined query functions.
 ** ----------------------------------------------------------------- */

bool
ccevents_query_fd_readability (cce_location_t * there, ccevents_group_t * grp, ccevents_source_t * src)
/* Query a file descriptor for readability. */
{
  /* Remember that "select()" might mutate this struct. */
  struct timeval	timeout = { 0, 0 };
  CCEVENTS_PC(ccevents_fd_source_t, fdsrc, src);
  fd_set		set;
  int			rv;
  FD_ZERO(&set);
  FD_SET(fdsrc->fd, &set);
  errno = 0;
  rv = select(1+(fdsrc->fd), &set, NULL, NULL, &timeout);
  if (-1 == rv) {
    /* An error occurred. */
    cce_raise(there, cce_errno_condition(rv));
  } else {
    /* Success.  RV  contains the number  of file descriptors  ready for
       reading; in this case it can be only 1 or 0. */
    //fprintf(stderr, "%s: rv=%d\n", __func__, rv);
    if (1 == rv) {
      return true;
    } else {
      ccevents_group_enqueue_source(grp, src);
      return false;
    }
  }
}

bool
ccevents_query_fd_writability (cce_location_t * there, ccevents_group_t * grp, ccevents_source_t * src)
/* Query a file descriptor for writability. */
{
  /* Remember that "select()" might mutate this struct. */
  struct timeval	timeout = { 0, 0 };
  CCEVENTS_PC(ccevents_fd_source_t, fdsrc, src);
  fd_set	set;
  int		rv;
  FD_ZERO(&set);
  FD_SET(fdsrc->fd, &set);
  errno = 0;
  rv = select(1+(fdsrc->fd), NULL, &set, NULL, &timeout);
  //fprintf(stderr, "%s: fd=%d, rv=%d\n", __func__, fdsrc->fd, rv);
  if (-1 == rv) {
    /* An error occurred. */
    cce_raise(there, cce_errno_condition(rv));
  } else {
    /* Success.  RV  contains the number  of file descriptors  ready for
       writing; in this case it can be only 1 or 0. */
    //fprintf(stderr, "%s: fd=%d, rv=%d\n", __func__, fdsrc->fd, rv);
    if (1 == rv) {
      return true;
    } else {
      ccevents_group_enqueue_source(grp, src);
      return false;
    }
  }
}

bool
ccevents_query_fd_exception (cce_location_t * there, ccevents_group_t * grp, ccevents_source_t * src)
/* Query a file descriptor for exception. */
{
  /* Remember that "select()" might mutate this struct. */
  struct timeval	timeout = { 0, 0 };
  CCEVENTS_PC(ccevents_fd_source_t, fdsrc, src);
  fd_set		set;
  int			rv;
  FD_ZERO(&set);
  FD_SET(fdsrc->fd, &set);
  errno = 0;
  rv = select(1+(fdsrc->fd), NULL, NULL, &set, &timeout);
  if (-1 == rv) {
    /* An error occurred. */
    cce_raise(there, cce_errno_condition(rv));
  } else {
    /* Success.   RV  contains  the  number of  file  descriptors  which
       received an exception; in this case it can be only 1 or 0. */
    if (1 == rv) {
      return true;
    } else {
      ccevents_group_enqueue_source(grp, src);
      return false;
    }
  }
}

/* end of file */
