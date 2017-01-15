/*
  Part of: CCEvents
  Contents: basic tests for file descriptor event sources
  Date: Sun Jan 15, 2017

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
#include <unistd.h>

int
main (int argc CCEVENTS_UNUSED, const char *const argv[] CCEVENTS_UNUSED)
{
  /* Detecting  readability  in the  first  file  descriptor of  a  pipe
     created with "pipe(2)". */
  {
    int		X[2];
    pipe(X);
    {
      ccevents_fd_source_t	fds[1];
      ccevents_absolute_time_t	expiration_time = {0, 0};
      volatile bool		flag = false;

      void event_handler (cce_location_tag_t * there, ccevents_fd_source_t * fds)
      {
	flag = true;
      }
      void expiration_handler (ccevents_fd_source_t * fds)
      {
	flag = true;
      }

      /* fprintf(stderr, "X[0] = %d, X[1] = %d\n", X[0], X[1]); */
      ccevents_fd_event_source_init (fds, X[0],
				     ccevents_query_fd_readability, event_handler,
				     expiration_time, expiration_handler);

      /* No event is pending: try to serve one. */
      {
	cce_location_t	L;
	bool		event_served = false;

	if (cce_location(L)) {
	  cce_run_error_handlers(L);
	} else {
	  event_served = ccevents_fd_source_do_one_event(L, fds);
	  cce_run_cleanup_handlers(L);
	  assert(false == event_served);
	  assert(false == flag);
	}
      }

      /* Write to the pipe, serve the event. */
      {
	cce_location_t	L;
	bool		event_served = false;

	assert(1 == write(X[1], "Z", 1));

	if (cce_location(L)) {
	  cce_run_error_handlers(L);
	} else {
	  event_served = ccevents_fd_source_do_one_event(L, fds);
	  cce_run_cleanup_handlers(L);
	  {
	    char		buf[1] = { '0' };
	    read(X[0], buf, 1);
	    assert('Z' == buf[0]);
	  }
	  assert(true == event_served);
	  assert(true == flag);
	}
      }
    }
    close(X[0]);
    close(X[1]);
  }

  /* Detecting  writability in  the  second file  descriptor  of a  pipe
     created with "pipe(2)".  The file descriptor is always writable. */
  {
    int		X[2];
    pipe(X);
    {
      ccevents_fd_source_t	fds[1];
      ccevents_absolute_time_t	expiration_time = {0, 0};
      volatile bool		flag = false;

      void event_handler (cce_location_tag_t * there, ccevents_fd_source_t * fds)
      {
	flag = true;
      }
      void expiration_handler (ccevents_fd_source_t * fds)
      {
	flag = true;
      }

      /* fprintf(stderr, "X[0] = %d, X[1] = %d\n", X[0], X[1]); */
      ccevents_fd_event_source_init (fds, X[1],
				     ccevents_query_fd_writability, event_handler,
				     expiration_time, expiration_handler);

      {
	cce_location_t	L;
	bool		event_served = false;

	if (cce_location(L)) {
	  cce_run_error_handlers(L);
	} else {
	  event_served = ccevents_fd_source_do_one_event(L, fds);
	  cce_run_cleanup_handlers(L);
	  assert(true == event_served);
	  assert(true == flag);
	}
      }
    }
    close(X[0]);
    close(X[1]);
  }

  exit(EXIT_SUCCESS);
}


/* end of file */
