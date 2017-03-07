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
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <time.h>

static void
sleep_awhile (void)
{
  struct timespec	sleep_span = { .tv_sec = 0, .tv_nsec = 100000 };
  struct timespec	remaining_time;
  nanosleep(&sleep_span, &remaining_time);
}


static void
test_standalone_readability (void)
/* Detecting readability in the first  file descriptor of a pipe created
   with "pipe(2)". */
{
  int			X[2];
  volatile bool		error_flag   = false;
  volatile bool		event_flag   = false;
  volatile bool		timeout_flag = false;
  cce_location_t	L[1];

  /* X[0] is the readable end.  X[1] is the writable end. */
  pipe(X);

  if (cce_location(L)) {
    cce_run_error_handlers(L);
    cce_condition_free(cce_condition(L));
    error_flag = true;
  } else {
    ccevents_group_t		grp[1];
    ccevents_fd_source_t	fdsrc[1];

    void event_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * fdsrc CCEVENTS_UNUSED)
    {
      event_flag = true;
    }
    void expiration_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * fdsrc CCEVENTS_UNUSED)
    {
      timeout_flag = true;
    }

    /* fprintf(stderr, "X[0] = %d, X[1] = %d\n", X[0], X[1]); */
    ccevents_fd_source_init(fdsrc, X[0]);
    ccevents_fd_source_set(fdsrc, ccevents_query_fd_readability, event_handler);
    ccevents_source_set_timeout(fdsrc, *CCEVENTS_TIMEVAL_NEVER, expiration_handler);

    ccevents_group_init(grp, INT_MAX);

    /* No event is pending: try to serve one. */
    {
      assert(false == ccevents_source_do_one_event(L, fdsrc));
      assert(false == event_flag);
      assert(false == timeout_flag);
    }

    /* Write to the pipe, serve the event. */
    {
      assert(1 == write(X[1], "Z", 1));
      assert(true == ccevents_source_do_one_event(L, fdsrc));
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
  cce_location_t	L[1];

  /* X[0] is the readable end.  X[1] is the writable end. */
  pipe(X);

  if (cce_location(L)) {
    cce_run_error_handlers(L);
    cce_condition_free(cce_condition(L));
    error_flag = true;
  } else {
    ccevents_group_t		grp[1];
    ccevents_fd_source_t	fdsrc[1];

    void event_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * fdsrc CCEVENTS_UNUSED)
    {
      event_flag = true;
    }
    void expiration_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * fdsrc CCEVENTS_UNUSED)
    {
      timeout_flag = true;
    }

    /* fprintf(stderr, "X[0] = %d, X[1] = %d\n", X[0], X[1]); */
    ccevents_fd_source_init(fdsrc, X[1]);
    ccevents_source_set_timeout(fdsrc, *CCEVENTS_TIMEVAL_NEVER, expiration_handler);
    ccevents_fd_source_set(fdsrc, ccevents_query_fd_writability, event_handler);

    ccevents_group_init(grp, INT_MAX);

    /* The pipe is always writable. */
    {
      assert(true == ccevents_source_do_one_event(L, fdsrc));
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
  volatile int		master_sock, server_sock, client_sock;
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
  cce_location_t	L[1];
  volatile bool		error_flag = false;

  if (cce_location(L)) {
    cce_run_error_handlers(L);
    cce_condition_free(cce_condition(L));
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
      ccevents_group_t		grp[1];
      ccevents_fd_source_t	readable_fd_source;
      ccevents_fd_source_t	exception_fd_source;
      volatile bool		readable_flag = false;
      volatile bool		exception_flag = false;
      volatile bool		timeout_flag = false;
      char			read_buf[11] = {
	'\0', '\0', '\0',  '\0', '\0', '\0',  '\0', '\0', '\0',  '\0', '\0'
      };
      int			read_len = 0;
      char			except_buf[2] = { '\0', '\0' };
      int			except_len = 0;

      void readable_event_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src)
      {
	ccevents_fd_source_t * fdsrc = ccevents_cast_to_fd_source(src);
	int	len;
	readable_flag = true;
	len = recv(fdsrc->fd, read_buf + read_len, 10, 0);
	//fprintf(stderr, "%s: read %d bytes\n", __func__, len);
	read_len += len;
      }
      void exception_event_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src)
      {
	ccevents_fd_source_t * fdsrc = ccevents_cast_to_fd_source(src);
	int	len;
	exception_flag = true;
	len = recv(fdsrc->fd, except_buf + except_len, 1, MSG_OOB);
	//fprintf(stderr, "%s: read %d bytes\n", __func__, len);
	except_len += len;
      }
      void expiration_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src CCEVENTS_UNUSED)
      {
	timeout_flag = true;
      }

      //fprintf(stderr, "socketpair: server_sock = %d, client_sock = %d\n", server_sock, client_sock);
      ccevents_fd_source_init(&readable_fd_source, server_sock);
      ccevents_source_set_timeout(&readable_fd_source, *CCEVENTS_TIMEVAL_NEVER, expiration_handler);
      ccevents_fd_source_set(&readable_fd_source, ccevents_query_fd_readability, readable_event_handler);

      ccevents_fd_source_init(&exception_fd_source, server_sock);
      ccevents_source_set_timeout(&exception_fd_source, *CCEVENTS_TIMEVAL_NEVER, expiration_handler);
      ccevents_fd_source_set(&exception_fd_source, ccevents_query_fd_exception, exception_event_handler);

      ccevents_group_init(grp, INT_MAX);

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
	assert(true == ccevents_source_do_one_event(L, &readable_fd_source));
	assert(true == readable_flag);
	assert(false == exception_flag);
	assert(false == timeout_flag);
	//fprintf(stderr, "read_len = %d, read_buf = %s\n", (int)read_len, read_buf);
	assert(0 == strncmp(read_buf, "123456", 6));
      }

      /* Serve the readable event. */
      {
	assert(true == ccevents_source_do_one_event(L, &exception_fd_source));
	assert(true == readable_flag);
	assert(true == exception_flag);
	assert(false == timeout_flag);
	//fprintf(stderr, "except_len = %d, except_buf = %s\n", (int)except_len, except_buf);
	assert(0 == strncmp(except_buf, "X", 1));
      }

      /* Serve the readable event. */
      {
	assert(true == ccevents_source_do_one_event(L, &readable_fd_source));
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


static void
test_talking_processes_with_groups (void)
/* Two processes  talk to each  others through  pipes.  This is  just an
   example,  so we  use an  event source  to wait  for writability  even
   though a pipe writable end is always writable. */
{
  int	forwards_pipe[2];
  int	backwards_pipe[2];

  /* forwards_pipe[0]  is the  readable  end.   forwards_pipe[1] is  the
     writable end. */
  pipe(forwards_pipe);
  /* backwards_pipe[0]  is the  readable  end.   backwards_pipe[1] is  the
     writable end. */
  pipe(backwards_pipe);

  void master_process (void)
  {
    int				read_fd		= backwards_pipe[0];
    int				write_fd	= forwards_pipe[1];
    cce_location_t		L[1];
    ccevents_group_t		grp[1];
    ccevents_fd_source_t	read_source[1];
    ccevents_fd_source_t	write_source[1];
    volatile int		state = 0;

    /* Location handler to close the file descriptors. */
    void close_read_fd_handler (const cce_condition_t * C CCEVENTS_UNUSED, cce_handler_t * H CCEVENTS_UNUSED)
    {
      close(read_fd);
    }
    void close_write_fd_handler (const cce_condition_t * C CCEVENTS_UNUSED, cce_handler_t * H CCEVENTS_UNUSED)
    {
      close(write_fd);
    }
    cce_handler_t	H1 = { .handler_function = close_read_fd_handler  };
    cce_handler_t	H2 = { .handler_function = close_write_fd_handler };
    cce_register_cleanup_handler(L, &H1);
    cce_register_cleanup_handler(L, &H2);

    /* Master's write event handler. */
    void master_write_event_handler (cce_location_t * there, ccevents_source_t * src)
    {
      ccevents_fd_source_t * fdsrc = ccevents_cast_to_fd_source(src);
      ccevents_source_dequeue_itself(write_source);
      fprintf(stderr, "%s: enter, master state %d\n", __func__, state);
      switch (state) {
      case 0: /* Send greetings. */
	{
	  fprintf(stderr, "master: send 'hello'\n");
	  errno = 0;
	  if (strlen("hello\n") != write(fdsrc->fd, "hello\n", strlen("hello\n"))) {
	    cce_raise(there, cce_errno_C(errno));
	  }
	  state = 1;
	  ccevents_group_enqueue_source(grp, read_source);
	}
	break;
      case 2: /* Send quitting. */
	{
	  fprintf(stderr, "master: send 'quit'\n");
	  errno = 0;
	  if (strlen("quit\n") != write(write_fd, "quit\n", strlen("quit\n"))) {
	    cce_raise(there, cce_errno_C(errno));
	  }
	  state = 3;
	  ccevents_group_enqueue_source(grp, read_source);
	}
	break;
      default:
	assert(0);
	break;
      }
      fprintf(stderr, "%s: leave, master state %d\n", __func__, state);
      sleep_awhile();
    }

    /* Master's read event handler. */
    void master_read_event_handler (cce_location_t * there, ccevents_source_t * src)
    {
      ccevents_fd_source_t * fdsrc = ccevents_cast_to_fd_source(src);
      ccevents_source_dequeue_itself(read_source);
      fprintf(stderr, "%s: master state %d\n", __func__, state);
      switch (state) {
      case 1: /* Read greetings. */
	{
	  char	buf[10];
	  int	count;
	  errno = 0;
	  count = read(fdsrc->fd, buf, 10);
	  if (-1 == count) {
	    cce_raise(there, cce_errno_C(errno));
	  }
	  buf[count] = '\0';
	  assert(0 == strncmp("hello\n", buf, strlen("hello\n")));
	  fprintf(stderr, "master: recv '%s'\n", buf);
	  state = 2;
	  ccevents_group_enqueue_source(grp, write_source);
	  sleep_awhile();
	}
	break;
      case 3: /* Read quitting. */
	{
	  char	buf[10];
	  int	count;
	  errno = 0;
	  count = read(fdsrc->fd, buf, 10);
	  if (-1 == count) {
	    cce_raise(there, cce_errno_C(errno));
	  }
	  buf[count] = '\0';
	  assert(0 == strncmp("quit\n", buf, strlen("quit\n")));
	  fprintf(stderr, "master: recv '%s'\n", buf);
	  state = 0;
	  /* end */
	}
	break;
      default:
	assert(0);
	break;
      }
    }

    /* Do the talking. */
    if (cce_location(L)) {
      cce_run_error_handlers(L);
      cce_condition_free(cce_condition(L));
    } else {
      ccevents_fd_source_init(read_source, read_fd);
      ccevents_fd_source_init(write_source, write_fd);
      ccevents_fd_source_set(read_source,  ccevents_query_fd_readability, master_read_event_handler);
      ccevents_fd_source_set(write_source, ccevents_query_fd_writability, master_write_event_handler);
      ccevents_source_set_timeout(read_source,  *CCEVENTS_TIMEVAL_NEVER, ccevents_dummy_timeout_handler);
      ccevents_source_set_timeout(write_source, *CCEVENTS_TIMEVAL_NEVER, ccevents_dummy_timeout_handler);
      ccevents_group_init(grp, INT_MAX);
      {
	ccevents_fd_source_set(write_source, ccevents_query_fd_writability, master_write_event_handler);
	ccevents_group_enqueue_source(grp, write_source);
      }
      ccevents_group_enter(grp);
      cce_run_cleanup_handlers(L);
    }
  }

  void slave_process (void)
  {
    int				read_fd		= forwards_pipe[0];
    int				write_fd	= backwards_pipe[1];
    cce_location_t		L[1];
    ccevents_group_t		grp[1];
    ccevents_fd_source_t	read_source[1];
    ccevents_fd_source_t	write_source[1];
    int				state = 0;
    bool			error_flag = false;

    /* Location handler to close the file descriptors. */
    void close_read_fd_handler (const cce_condition_t * C CCEVENTS_UNUSED, cce_handler_t * H CCEVENTS_UNUSED)
    {
      close(read_fd);
    }
    void close_write_fd_handler (const cce_condition_t * C CCEVENTS_UNUSED, cce_handler_t * H CCEVENTS_UNUSED)
    {
      close(write_fd);
    }
    cce_handler_t	H1 = { .handler_function = close_read_fd_handler  };
    cce_handler_t	H2 = { .handler_function = close_write_fd_handler };
    cce_register_cleanup_handler(L, &H1);
    cce_register_cleanup_handler(L, &H2);

    /* Slave's read event handler. */
    void slave_read_event_handler (cce_location_t * there, ccevents_source_t * src)
    {
      ccevents_fd_source_t * fdsrc = ccevents_cast_to_fd_source(src);
      fprintf(stderr, "%s: slave state %d\n", __func__, state);
      switch (state) {
      case 0: /* Read greetings. */
	{
	  char	buf[10];
	  int	count;
	  errno = 0;
	  count = read(fdsrc->fd, buf, 10);
	  if (-1 == count) {
	    cce_raise(there, cce_errno_C(errno));
	  }
	  buf[count] = '\0';
	  assert(0 == strncmp("hello\n", buf, strlen("hello\n")));
	  fprintf(stderr, "slave: recv '%s'\n", buf);
	  state = 1;
	  ccevents_source_dequeue_itself(read_source);
	  ccevents_group_enqueue_source(grp, write_source);
	  sleep_awhile();
	}
	break;
      case 2: /* Read quitting. */
	{
	  char	buf[10];
	  int	count;
	  errno = 0;
	  count = read(fdsrc->fd, buf, 10);
	  if (-1 == count) {
	    cce_raise(there, cce_errno_C(errno));
	  }
	  buf[count] = '\0';
	  assert(0 == strncmp("quit\n", buf, strlen("quit\n")));
	  fprintf(stderr, "slave: recv '%s'\n", buf);
	  state = 3;
	  ccevents_source_dequeue_itself(read_source);
	  ccevents_group_enqueue_source(grp, write_source);
	  sleep_awhile();
	}
	break;
      }
    }

    /* Slave's write event handler. */
    void slave_write_event_handler (cce_location_t * there, ccevents_source_t * src)
    {
      ccevents_fd_source_t * fdsrc = ccevents_cast_to_fd_source(src);
      fprintf(stderr, "%s: slave state %d\n", __func__, state);
      switch (state) {
      case 1: /* Send greetings. */
	{
	  fprintf(stderr, "slave: send 'hello'\n");
	  errno = 0;
	  if (strlen("hello\n") != write(fdsrc->fd, "hello\n", strlen("hello\n"))) {
	    cce_raise(there, cce_errno_C(errno));
	  }
	  fprintf(stderr, "slave: sent 'hello'\n");
	  state = 2;
	  ccevents_source_dequeue_itself(write_source);
	  ccevents_group_enqueue_source(grp, read_source);
	  sleep_awhile();
	}
	break;
      case 3: /* Send quitting. */
	{
	  fprintf(stderr, "slave: send 'quit'\n");
	  errno = 0;
	  if (strlen("quit\n") != write(fdsrc->fd, "quit\n", strlen("quit\n"))) {
	    cce_raise(there, cce_errno_C(errno));
	  }
	  state = 0;
	  ccevents_source_dequeue_itself(src);
	}
	break;
      }
    }

    /* Do the talking. */
    if (cce_location(L)) {
      cce_run_error_handlers(L);
      cce_condition_free(cce_condition(L));
      error_flag = true;
    } else {
      ccevents_fd_source_init(read_source, read_fd);
      ccevents_fd_source_init(write_source, write_fd);
      ccevents_fd_source_set(read_source,  ccevents_query_fd_readability, slave_read_event_handler);
      ccevents_fd_source_set(write_source, ccevents_query_fd_writability, slave_write_event_handler);
      ccevents_source_set_timeout( read_source, *CCEVENTS_TIMEVAL_NEVER, ccevents_dummy_timeout_handler);
      ccevents_source_set_timeout(write_source, *CCEVENTS_TIMEVAL_NEVER, ccevents_dummy_timeout_handler);

      ccevents_group_init(grp, INT_MAX);
      {
	ccevents_group_enqueue_source(grp, read_source);
      }
      ccevents_group_enter(grp);
      cce_run_cleanup_handlers(L);
    }

    exit((error_flag)? EXIT_FAILURE : EXIT_SUCCESS);
  }

  /* Create the slave process and run  the thing.  The parent process is
     the master.  The slave process is the child. */
  {
    cce_location_t	L[1];
    pid_t		pid;

    if (cce_location(L)) {
      cce_run_error_handlers(L);
      cce_condition_free(cce_condition(L));
    } else {
      errno = 0;
      pid   = fork();
      if (-1 == pid) {
	cce_raise(L, cce_errno_C(errno));
      } else if (0 == pid) {
	/* This is the child process. */
	slave_process();
      } else {
	/* This is the parent process. */
	master_process();
      }

      /* Wait for the child process. */
      {
	int	rv, wstatus = 0;

	errno = 0;
	rv    = waitpid(pid, &wstatus, 0);
	if (-1 == rv) {
	  cce_raise(L, cce_errno_C(errno));
	}
	assert(WIFEXITED(wstatus));
	assert(0 == WEXITSTATUS(wstatus));
      }

      cce_run_cleanup_handlers(L);
    }
  }
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
  if (1) test_talking_processes_with_groups();
  //
  exit(EXIT_SUCCESS);
}

/* end of file */
