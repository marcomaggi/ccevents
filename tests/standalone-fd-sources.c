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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


static void
test_standalone_readability (void)
/* Detecting readability in the first  file descriptor of a pipe created
   with "pipe(2)". */
{
  int			X[2];
  volatile bool		error_flag = false;
  volatile bool		event_flag = false;
  volatile bool		timeout_flag = false;
  cce_location_t	L;

  /* X[0] is the readable end.  X[1] is the writable end. */
  pipe(X);

  if (cce_location(L)) {
    cce_run_error_handlers(L);
    error_flag = true;
  } else {
    ccevents_fd_source_t	fds[1];
    ccevents_timeout_t		expiration_time = *CCEVENTS_TIMEOUT_NEVER;

    void event_handler (cce_location_tag_t * there, ccevents_fd_source_t * fds)
    {
      event_flag = true;
    }
    void expiration_handler (cce_location_tag_t * there, ccevents_fd_source_t * fds)
    {
      timeout_flag = true;
    }

    /* fprintf(stderr, "X[0] = %d, X[1] = %d\n", X[0], X[1]); */
    ccevents_fd_event_source_init(fds, X[0]);
    ccevents_fd_event_source_set(L, fds,
				 ccevents_query_fd_readability, event_handler,
				 expiration_time, expiration_handler);

    /* No event is pending: try to serve one. */
    {
      assert(false == ccevents_fd_source_do_one_event(L, fds));
      assert(false == event_flag);
      assert(false == timeout_flag);
    }

    /* Write to the pipe, serve the event. */
    {
      assert(1 == write(X[1], "Z", 1));
      assert(true == ccevents_fd_source_do_one_event(L, fds));
      cce_run_cleanup_handlers(L);
      {
	char	buf[1] = { '0' };
	read(X[0], buf, 1);
	assert('Z' == buf[0]);
      }
      assert(true  == event_flag);
      assert(false == timeout_flag);
    }

    cce_run_cleanup_handlers(L);
  }

  close(X[0]);
  close(X[1]);
  assert(false == error_flag);
}


static void
test_standalone_writability (void)
/* Detecting writability in the second file descriptor of a pipe created
   with "pipe(2)".  The file descriptor is always writable. */
{
  int			X[2];
  volatile bool		error_flag = false;
  volatile bool		event_flag = false;
  volatile bool		timeout_flag = false;
  cce_location_t	L;

  /* X[0] is the readable end.  X[1] is the writable end. */
  pipe(X);

  if (cce_location(L)) {
    cce_run_error_handlers(L);
    error_flag = true;
  } else {
    ccevents_fd_source_t	fds[1];
    ccevents_timeout_t		expiration_time = *CCEVENTS_TIMEOUT_NEVER;

    void event_handler (cce_location_tag_t * there, ccevents_fd_source_t * fds)
    {
      event_flag = true;
    }
    void expiration_handler (cce_location_tag_t * there, ccevents_fd_source_t * fds)
    {
      timeout_flag = true;
    }

    /* fprintf(stderr, "X[0] = %d, X[1] = %d\n", X[0], X[1]); */
    ccevents_fd_event_source_init(fds, X[1]);
    ccevents_fd_event_source_set(L, fds,
				 ccevents_query_fd_writability, event_handler,
				 expiration_time, expiration_handler);

    /* The pipe is always writable. */
    {
      assert(true == ccevents_fd_source_do_one_event(L, fds));
      cce_run_cleanup_handlers(L);
      assert(true  == event_flag);
      assert(false == timeout_flag);
    }

    cce_run_cleanup_handlers(L);
  }

  close(X[0]);
  close(X[1]);
  assert(false == error_flag);
}


static void
test_standalone_exception (void)
/* Detecting exception in  the server socket of a INET  server.  To test
   sending  OOB  data  we  really  need  to  use  a  client/server  INET
   connection, a socketpair will not work. */
{
  int			master_sock, server_sock, client_sock;
  struct linger		optval = { .l_onoff  = 1, .l_linger = 1 };
  socklen_t		optlen = sizeof(struct linger);
  struct sockaddr_in	master_addr = {
    .sin_family		= AF_INET,
    /* This becomes the address "127.0.0.1" in host byte order. */
    .sin_addr.s_addr	= htonl(INADDR_ANY),
    .sin_port		= htons(8080)
  };
  struct sockaddr_in	server_addr;
  socklen_t		server_addr_len;
  struct sockaddr_in	client_addr = {
    .sin_family	= AF_INET,
    .sin_addr.s_addr	= htonl(INADDR_ANY),
    .sin_port		= htons(8080)
  };
  cce_location_t	L;
  bool			error_flag = false;

  if (cce_location(L)) {
    cce_run_error_handlers(L);
    error_flag = true;
  } else {
    master_sock = socket(PF_INET, SOCK_STREAM, 0);
    /* We need this SO_REUSEADDR option: the system will let us run this
       program multiple  times in a short  time span.  If we  do not set
       it: the program  will fail if the address is  still registered as
       bound by a previou program run. */
    {
      int	reuse = 1;
      setsockopt(master_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
    }
    assert(0 <= master_sock);
    assert(0 == bind(master_sock, (struct sockaddr *)&master_addr, sizeof(master_addr)));
    assert(0 == listen(master_sock, 2));

    client_sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(0 <= client_sock);
    setsockopt(client_sock, SOL_SOCKET, SO_LINGER, &optval, optlen);
    assert(0 == connect(client_sock, (struct sockaddr *)&client_addr, sizeof(client_addr)));

    server_sock = accept(master_sock, (struct sockaddr *)&server_addr, &server_addr_len);
    setsockopt(master_sock, SOL_SOCKET, SO_LINGER, &optval, optlen);

    {
      ccevents_fd_source_t	readable_fd_source;
      ccevents_fd_source_t	exception_fd_source;
      ccevents_timeout_t	expiration_time = *CCEVENTS_TIMEOUT_NEVER;
      volatile bool		readable_flag = false;
      volatile bool		exception_flag = false;
      volatile bool		timeout_flag = false;
      char			read_buf[11] = {
	'\0', '\0', '\0',  '\0', '\0', '\0',  '\0', '\0', '\0',  '\0', '\0'
      };
      int			read_len = 0;
      char			except_buf[2] = { '\0', '\0' };
      int			except_len = 0;

      void readable_event_handler (cce_location_tag_t * there, ccevents_fd_source_t * fds)
      {
	int	len;
	readable_flag = true;
	len = recv(fds->fd, read_buf + read_len, 10, 0);
	//fprintf(stderr, "%s: read %d bytes\n", __func__, len);
	read_len += len;
      }
      void exception_event_handler (cce_location_tag_t * there, ccevents_fd_source_t * fds)
      {
	int	len;
	exception_flag = true;
	len = recv(fds->fd, except_buf + except_len, 1, MSG_OOB);
	//fprintf(stderr, "%s: read %d bytes\n", __func__, len);
	except_len += len;
      }
      void expiration_handler (cce_location_tag_t * there, ccevents_fd_source_t * fds)
      {
	timeout_flag = true;
      }

      //fprintf(stderr, "socketpair: server_sock = %d, client_sock = %d\n", server_sock, client_sock);
      ccevents_fd_event_source_init(&readable_fd_source, server_sock);
      ccevents_fd_event_source_set(L, &readable_fd_source,
				   ccevents_query_fd_readability, readable_event_handler,
				   expiration_time, expiration_handler);

      ccevents_fd_event_source_init(&exception_fd_source, server_sock);
      ccevents_fd_event_source_set(L, &exception_fd_source,
				   ccevents_query_fd_exception, exception_event_handler,
				   expiration_time, expiration_handler);

      /* Send data. */
      {
	uint8_t		buf1[3] = { '1', '2', '3' };
	uint8_t		buf2[3] = { '4', '5', '6' };
	uint8_t		buf3[3] = { '7', '8', '9' };
	assert(3 == send(client_sock, buf1, 3, 0));
	assert(3 == send(client_sock, buf2, 3, 0));
	/* The byte "X" is the OOB data. */
	assert(1 == send(client_sock, "X", 1, MSG_OOB));
	assert(3 == send(client_sock, buf3, 3, 0));
      }

      /* Serve the readable event. */
      {
	assert(true == ccevents_fd_source_do_one_event(L, &readable_fd_source));
	assert(true == readable_flag);
	assert(false == exception_flag);
	assert(false == timeout_flag);
	//fprintf(stderr, "read_len = %d, read_buf = %s\n", (int)read_len, read_buf);
	assert(0 == strncmp(read_buf, "123456", 6));
      }

      /* Serve the readable event. */
      {
	assert(true == ccevents_fd_source_do_one_event(L, &exception_fd_source));
	assert(true == readable_flag);
	assert(true == exception_flag);
	assert(false == timeout_flag);
	//fprintf(stderr, "except_len = %d, except_buf = %s\n", (int)except_len, except_buf);
	assert(0 == strncmp(except_buf, "X", 1));
      }

      /* Serve the readable event. */
      {
	assert(true == ccevents_fd_source_do_one_event(L, &readable_fd_source));
	assert(true == readable_flag);
	//fprintf(stderr, "read_len = %d, read_buf = %s\n", (int)read_len, read_buf);
	assert(0 == strncmp(read_buf, "123456789", 9));
      }
    }
    cce_run_cleanup_handlers(L);
  }
  close(server_sock);
  close(client_sock);
  close(master_sock);
  assert(false == error_flag);
}


int
main (int argc CCEVENTS_UNUSED, const char *const argv[] CCEVENTS_UNUSED)
{
  ccevents_init();
  //
  if (1) test_standalone_readability();
  if (1) test_standalone_writability();
  if (1) test_standalone_exception();
  //
  exit(EXIT_SUCCESS);
}

/* end of file */
