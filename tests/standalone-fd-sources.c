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
#include <ccexceptions-system.h>
#include <ccexceptions-networking.h>
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


static volatile bool test_1_0_event_flag   = false;
static volatile bool test_1_0_timeout_flag = false;

static void
test_1_0_event_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * fdsrc CCEVENTS_UNUSED)
{
  test_1_0_event_flag = true;
}

static void
test_1_0_expiration_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * fdsrc CCEVENTS_UNUSED)
{
  test_1_0_timeout_flag = true;
}

static void
test_1_0_standalone_readability (void)
/* Detecting readability in the first  file descriptor of a pipe created
   with "pipe(2)". */
{
  int			X[2];
  volatile bool		error_flag   = false;
  cce_location_t	L[1];

  /* X[0] is the readable end.  X[1] is the writable end. */
  pipe(X);

  if (cce_location(L)) {
    cce_run_error_handlers_final(L);
    error_flag = true;
  } else {
    ccevents_group_t		grp[1];
    ccevents_fd_source_t	fdsrc[1];

    /* fprintf(stderr, "X[0] = %d, X[1] = %d\n", X[0], X[1]); */
    ccevents_fd_source_init(fdsrc, X[0]);
    ccevents_fd_source_set(fdsrc, ccevents_query_fd_readability, test_1_0_event_handler);
    ccevents_source_set_timeout(ccevents_source(fdsrc), *CCEVENTS_TIMEVAL_NEVER, test_1_0_expiration_handler);

    ccevents_group_init(grp, INT_MAX);

    /* No event is pending: try to serve one. */
    {
      assert(false == ccevents_source_do_one_event(L, ccevents_source(fdsrc)));
      assert(false == test_1_0_event_flag);
      assert(false == test_1_0_timeout_flag);
    }

    /* Write to the pipe, serve the event. */
    {
      assert(1 == write(X[1], "Z", 1));
      assert(true == ccevents_source_do_one_event(L, ccevents_source(fdsrc)));
      cce_run_cleanup_handlers(L);
      {
	char	buf[1] = { '0' };
	read(X[0], buf, 1);
	assert('Z' == buf[0]);
      }
      assert(true  == test_1_0_event_flag);
      assert(false == test_1_0_timeout_flag);
    }

    cce_run_cleanup_handlers(L);
  }

  close(X[0]);
  close(X[1]);
  assert(false == error_flag);
}


static volatile bool test_2_0_event_flag   = false;
static volatile bool test_2_0_timeout_flag = false;

static void
test_2_0_event_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * fdsrc CCEVENTS_UNUSED)
{
  test_2_0_event_flag = true;
}
static void
test_2_0_expiration_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * fdsrc CCEVENTS_UNUSED)
{
  test_2_0_timeout_flag = true;
}

static void
test_2_0_standalone_writability (void)
/* Detecting writability in the second file descriptor of a pipe created
   with "pipe(2)".  The file descriptor is always writable. */
{
  int			X[2];
  volatile bool		error_flag = false;
  cce_location_t	L[1];

  /* X[0] is the readable end.  X[1] is the writable end. */
  pipe(X);

  if (cce_location(L)) {
    cce_run_error_handlers_final(L);
    error_flag = true;
  } else {
    ccevents_group_t		grp[1];
    ccevents_fd_source_t	fdsrc[1];

    /* fprintf(stderr, "X[0] = %d, X[1] = %d\n", X[0], X[1]); */
    ccevents_fd_source_init(fdsrc, X[1]);
    ccevents_fd_source_set(fdsrc, ccevents_query_fd_writability, test_2_0_event_handler);
    ccevents_source_set_timeout(ccevents_source(fdsrc), *CCEVENTS_TIMEVAL_NEVER, test_2_0_expiration_handler);

    ccevents_group_init(grp, INT_MAX);

    /* The pipe is always writable. */
    {
      assert(true == ccevents_source_do_one_event(L, ccevents_source(fdsrc)));
      cce_run_cleanup_handlers(L);
      assert(true  == test_2_0_event_flag);
      assert(false == test_2_0_timeout_flag);
    }

    cce_run_cleanup_handlers(L);
  }

  close(X[0]);
  close(X[1]);
  assert(false == error_flag);
}


typedef struct test_3_0_readable_t {
  ccevents_fd_source_t	src;
  char			read_buf[11];
  int			read_len;
  volatile bool		readable_flag;
} test_3_0_readable_t;

typedef struct test_3_0_exception_t {
  ccevents_fd_source_t	src;
  char			except_buf[2];
  int			except_len;
  volatile bool		exception_flag;
} test_3_0_exception_t;

static void
test_3_0_readable_event_handler (cce_location_t * there, ccevents_source_t * _src)
{
  CCEVENTS_PC(test_3_0_readable_t, state, _src);
  int	len;
  len = cce_sys_recv(there, state->src.fd, state->read_buf + state->read_len, 10, 0);
  if (0) { fprintf(stderr, "%s: read %d bytes\n", __func__, len); }
  state->read_len      += len;
  state->readable_flag  = true;
}

static void
test_3_0_exception_event_handler (cce_location_t * there, ccevents_source_t * _src)
{
  CCEVENTS_PC(test_3_0_exception_t, state, _src);
  int	len;
  len = cce_sys_recv(there, state->src.fd, state->except_buf + state->except_len, 1, MSG_OOB);
  if (0) { fprintf(stderr, "%s: read %d bytes\n", __func__, len); }
  state->except_len += len;
  state->exception_flag = true;
}

static void
test_3_0_standalone_exception (void)
/* Detecting exception in  the server socket of a INET  server.  To test
   sending  OOB  data  we  really  need  to  use  a  client/server  INET
   connection, a socketpair will not work. */
{
  cce_location_t	L[1];
  cce_handler_t		master_sock_H[1];
  cce_handler_t		server_sock_H[1];
  cce_handler_t		client_sock_H[1];
  volatile bool		error_flag = false;

  if (cce_location(L)) {
    fprintf(stderr, "%s: error, %s\n", __func__, cce_condition_static_message(cce_condition(L)));
    cce_run_error_handlers_final(L);
    error_flag = true;
  } else {
    volatile int	master_sock, server_sock, client_sock;

    /* Prepare the sockets pair. */
    {
      struct linger		optval = { .l_onoff  = 1, .l_linger = 1 };
      socklen_t			optlen = sizeof(struct linger);
      struct sockaddr_in	master_addr;
      master_addr.sin_family		= AF_INET;
      /* This becomes the address "127.0.0.1" in host byte order. */
      master_addr.sin_addr.s_addr	= htonl(INADDR_ANY);
      master_addr.sin_port		= htons(8080);
      struct sockaddr_in	server_addr;
      socklen_t			server_addr_len;
      struct sockaddr_in	client_addr;
      client_addr.sin_family		= AF_INET;
      client_addr.sin_addr.s_addr	= htonl(INADDR_ANY);
      client_addr.sin_port		= htons(8080);

      master_sock = cce_sys_socket(L, PF_INET, SOCK_STREAM, 0);
      assert(0 <= master_sock);
      cce_cleanup_handler_filedes_init(L, master_sock_H, master_sock);
      /* We need  this SO_REUSEADDR option:  the system will let  us run
	 this program multiple times in a short time span.  If we do not
	 set  it:  the  program  will  fail  if  the  address  is  still
	 registered as bound by a previou program run. */
      {
	int	reuse = 1;
	cce_sys_setsockopt(L, master_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
      }
      cce_sys_bind(L, master_sock, (struct sockaddr *)&master_addr, sizeof(master_addr));
      cce_sys_listen(L, master_sock, 2);

      client_sock = cce_sys_socket(L, PF_INET, SOCK_STREAM, 0);
      assert(0 <= client_sock);
      cce_cleanup_handler_filedes_init(L, client_sock_H, client_sock);
      setsockopt(client_sock, SOL_SOCKET, SO_LINGER, &optval, optlen);
      cce_sys_connect(L, client_sock, (struct sockaddr *)&client_addr, sizeof(client_addr));

      server_sock = cce_sys_accept(L, master_sock, (struct sockaddr *)&server_addr, &server_addr_len);
      //assert(0 <= server_sock);
      cce_cleanup_handler_filedes_init(L, server_sock_H, server_sock);
      setsockopt(master_sock, SOL_SOCKET, SO_LINGER, &optval, optlen);
    }

    {
      ccevents_group_t		grp[1];
      test_3_0_readable_t	readable_state = {
	.read_len	= 0,
	.readable_flag	= false
      };
      memset(readable_state.read_buf, '\0', sizeof(readable_state.read_buf[11]));
      test_3_0_exception_t	exception_state = {
	.except_len	= 0,
	.exception_flag	= false
      };
      memset(exception_state.except_buf, '\0', sizeof(exception_state.except_buf[2]));

      if (0) { fprintf(stderr, "socketpair: server_sock = %d, client_sock = %d\n", server_sock, client_sock); }
      ccevents_fd_source_init(&readable_state.src, server_sock);
      ccevents_fd_source_set(&readable_state.src, ccevents_query_fd_readability, test_3_0_readable_event_handler);

      ccevents_fd_source_init(&exception_state.src, server_sock);
      ccevents_fd_source_set(&exception_state.src, ccevents_query_fd_exception, test_3_0_exception_event_handler);

      ccevents_group_init(grp, INT_MAX);

      /* Send data. */
      {
	uint8_t		buf1[3] = { '1', '2', '3' };
	uint8_t		buf2[3] = { '4', '5', '6' };
	uint8_t		buf3[3] = { '7', '8', '9' };
	assert(3 == cce_sys_send(L, client_sock, buf1, 3, 0));
	assert(3 == cce_sys_send(L, client_sock, buf2, 3, 0));
	/* The byte "X" is the OOB data. */
	assert(1 == cce_sys_send(L, client_sock, "X", 1, MSG_OOB));
	assert(3 == cce_sys_send(L, client_sock, buf3, 3, 0));
      }

      /* Serve the readable event. */
      {
	assert(true  == ccevents_source_do_one_event(L, ccevents_source(&readable_state.src)));
	assert(true  == readable_state.readable_flag);
	assert(false == exception_state.exception_flag);
	if (0) { fprintf(stderr, "read_len = %d, read_buf = %s\n", (int)(readable_state.read_len), readable_state.read_buf); }
	assert(0 == strncmp(readable_state.read_buf, "123456", 6));
      }

      /* Serve the readable event. */
      {
	assert(true == ccevents_source_do_one_event(L, ccevents_source(&exception_state.src)));
	assert(true == readable_state.readable_flag);
	assert(true == exception_state.exception_flag);
	if (1) { fprintf(stderr, "except_len = %d, except_buf = %s\n", (int)(exception_state.except_len), exception_state.except_buf); }
	assert(0 == strncmp(exception_state.except_buf, "X", 1));
      }

      /* Serve the readable event. */
      {
	assert(true == ccevents_source_do_one_event(L, ccevents_source(&readable_state.src)));
	assert(true == readable_state.readable_flag);
	if (1) { fprintf(stderr, "read_len = %d, read_buf = %s\n", (int)(readable_state.read_len), readable_state.read_buf); }
	assert(0 == strncmp(readable_state.read_buf, "123456789", 9));
      }
    }
    fprintf(stderr, "%s: done, no error\n", __func__);
    cce_run_cleanup_handlers(L);
  }
  assert(false == error_flag);
}


typedef struct test_4_0_master_state_t	test_4_0_master_state_t;
typedef struct test_4_0_slave_state_t	test_4_0_slave_state_t;

struct test_4_0_master_state_t {
  ccevents_fd_source_t	read_source;
  ccevents_fd_source_t	write_source;
  volatile int		state;
};

struct test_4_0_slave_state_t {
  ccevents_fd_source_t	read_source;
  ccevents_fd_source_t	write_source;
  volatile int		state;
};

static void test_4_0_master_process (cce_location_t * there, int forwards_pipe[2], int backwards_pipe[2]);
static void test_4_0_slave_process  (int forwards_pipe[2], int backwards_pipe[2]);

static ccevents_event_handler_t test_4_0_master_read_event_handler;
static ccevents_event_handler_t test_4_0_master_write_event_handler;
static ccevents_event_handler_t test_4_0_slave_read_event_handler;
static ccevents_event_handler_t test_4_0_slave_write_event_handler;

static void
test_4_0_talking_processes_with_groups (void)
/* Two processes  talk to each  others through  pipes.  This is  just an
   example,  so we  use an  event source  to wait  for writability  even
   though a pipe writable end is always writable. */
{
  cce_location_t	L[1];
  int			forwards_pipe[2];
  cce_handler_t		forwards_pipe_H[1];
  int			backwards_pipe[2];
  cce_handler_t		backwards_pipe_H[1];
  pid_t			pid;

  if (cce_location(L)) {
    cce_run_error_handlers_final(L);
  } else {
    /* forwards_pipe[0] is  the readable  end.  forwards_pipe[1]  is the
       writable end. */
    cce_sys_pipe(L, forwards_pipe);
    cce_cleanup_handler_pipedes_init(L, forwards_pipe_H, forwards_pipe);

    /* backwards_pipe[0] is the readable  end.  backwards_pipe[1] is the
       writable end. */
    cce_sys_pipe(L, backwards_pipe);
    cce_cleanup_handler_pipedes_init(L, backwards_pipe_H, backwards_pipe);

    /* Create the slave  process and run the thing.   The parent process
       is the master.  The slave process is the child. */
    pid = cce_sys_fork(L);
    if (0 == pid) {
      /* This is the child process. */
      test_4_0_slave_process(forwards_pipe, backwards_pipe);
    } else {
      /* This is the parent process. */
      test_4_0_master_process(L, forwards_pipe, backwards_pipe);
    }

    /* Wait for the child process. */
    {
      int	wstatus = 0;

      cce_sys_waitpid(L, pid, &wstatus, 0);
      assert(WIFEXITED(wstatus));
      assert(0 == WEXITSTATUS(wstatus));
    }

    cce_run_cleanup_handlers(L);
  }
}

/* ------------------------------------------------------------------ */

static void
test_4_0_master_process (cce_location_t * there, int forwards_pipe[2], int backwards_pipe[2])
{
  test_4_0_master_state_t	state = {
    .state		= 0,
  };
  cce_location_t	L[1];
  ccevents_group_t	grp[1];
  int			read_fd  = backwards_pipe[0];
  int			write_fd = forwards_pipe[1];

  /* Do the talking. */
  if (cce_location(L)) {
    cce_run_error_handlers_raise(L, there);
  } else {
    ccevents_fd_source_init(&state.read_source,  read_fd);
    ccevents_fd_source_init(&state.write_source, write_fd);
    ccevents_fd_source_set(&state.read_source,  ccevents_query_fd_readability, test_4_0_master_read_event_handler);
    ccevents_fd_source_set(&state.write_source, ccevents_query_fd_writability, test_4_0_master_write_event_handler);
    ccevents_group_init(grp, INT_MAX);
    {
      ccevents_group_enqueue_source(grp, ccevents_source(&state.read_source));
      ccevents_group_enqueue_source(grp, ccevents_source(&state.write_source));
      ccevents_source_disable_servicing(ccevents_source(&state.read_source));
    }
    ccevents_group_enter(grp);
    cce_run_cleanup_handlers(L);
  }
}

static void
test_4_0_master_write_event_handler (cce_location_t * there, ccevents_source_t * src)
/* Master's write event handler. */
{
  CCEVENTS_SF(test_4_0_master_state_t, master, write_source, src);
  fprintf(stderr, "%s: enter, master state %d\n", __func__, master->state);
  ccevents_source_disable_servicing(ccevents_source(&(master->write_source)));
  {
    char const *	buf;
    switch (master->state) {
    case 0: /* Send greetings. */
      buf = "hello\n";
      master->state = 1;
      break;
    case 2: /* Send quitting. */
      buf = "quit\n";
      master->state = 3;
      break;
    default:
      assert(0);
      break;
    }
    fprintf(stderr, "master: send '%s'\n", buf);
    if (strlen(buf) != cce_sys_write(there, master->write_source.fd, buf, strlen(buf))) {
      cce_raise(there, cce_condition_new_errno_clear());
    }
  }
  ccevents_source_enable_servicing(ccevents_source(&(master->read_source)));
  fprintf(stderr, "%s: leave, master state %d\n", __func__, master->state);
  sleep_awhile();
}

static void
test_4_0_master_read_event_handler (cce_location_t * there, ccevents_source_t * src)
/* Master's read event handler. */
{
  CCEVENTS_SF(test_4_0_master_state_t, master, read_source, src);
  char	buf[10];
  int	count;

  fprintf(stderr, "%s: master state %d\n", __func__, master->state);
  ccevents_source_disable_servicing(ccevents_source(&(master->read_source)));
  count = cce_sys_read(there, master->read_source.fd, buf, 10);
  buf[count] = '\0';
  switch (master->state) {
  case 1: /* Read greetings. */
    {
      assert(0 == strncmp("hello\n", buf, strlen("hello\n")));
      fprintf(stderr, "master: recv '%s'\n", buf);
      master->state = 2;
      ccevents_source_enable_servicing(ccevents_source(&(master->write_source)));
      sleep_awhile();
    }
    break;
  case 3: /* Read quitting. */
    {
      assert(0 == strncmp("quit\n", buf, strlen("quit\n")));
      fprintf(stderr, "master: recv '%s'\n", buf);
      master->state = 0;
      ccevents_source_dequeue_itself(ccevents_source(&(master->read_source)));
      ccevents_source_dequeue_itself(ccevents_source(&(master->write_source)));
      /* end */
    }
    break;
  default:
    assert(0);
    break;
  }
}

/* ------------------------------------------------------------------ */

static void
test_4_0_slave_process (int forwards_pipe[2], int backwards_pipe[2])
{
  test_4_0_slave_state_t	state = { .state = 0 };
  cce_location_t	L[1];
  ccevents_group_t	grp[1];
  int			read_fd  = forwards_pipe[0];
  int			write_fd = backwards_pipe[1];
  bool			error_flag = false;

  /* Do the talking. */
  if (cce_location(L)) {
    cce_run_error_handlers_final(L);
    error_flag = true;
  } else {
    ccevents_fd_source_init(&state.read_source,  read_fd);
    ccevents_fd_source_init(&state.write_source, write_fd);
    ccevents_fd_source_set(&state.read_source,  ccevents_query_fd_readability, test_4_0_slave_read_event_handler);
    ccevents_fd_source_set(&state.write_source, ccevents_query_fd_writability, test_4_0_slave_write_event_handler);

    ccevents_group_init(grp, INT_MAX);
    {
      ccevents_group_enqueue_source(grp, ccevents_source(&state.read_source));
      ccevents_group_enqueue_source(grp, ccevents_source(&state.write_source));
      ccevents_source_disable_servicing(ccevents_source(&state.write_source));
    }
    fprintf(stderr, "%s: enter group\n", __func__);
    ccevents_group_enter(grp);
    cce_run_cleanup_handlers(L);
  }

  exit((error_flag)? EXIT_FAILURE : EXIT_SUCCESS);
}

static void
test_4_0_slave_read_event_handler (cce_location_t * there, ccevents_source_t * src)
/* Slave's read event handler. */
{
  CCEVENTS_SF(test_4_0_slave_state_t, slave, read_source, src);
  char	buf[10];
  int	count;

  fprintf(stderr, "%s: slave state %d\n", __func__, slave->state);
  ccevents_source_disable_servicing(ccevents_source(&(slave->read_source)));
  count = cce_sys_read(there, slave->read_source.fd, buf, 10);
  buf[count] = '\0';

  switch (slave->state) {
  case 0: /* Read greetings. */
    assert(0 == strncmp("hello\n", buf, strlen("hello\n")));
    slave->state = 1;
    break;
  case 2: /* Read quitting. */
    assert(0 == strncmp("quit\n", buf, strlen("quit\n")));
    slave->state = 3;
    break;
  default:
    assert(0);
    break;
  }
  fprintf(stderr, "slave: recv '%s'\n", buf);
  ccevents_source_enable_servicing(ccevents_source(&(slave->write_source)));
  sleep_awhile();
}

static void
test_4_0_slave_write_event_handler (cce_location_t * there, ccevents_source_t * src)
/* Slave's write event handler. */
{
  CCEVENTS_SF(test_4_0_slave_state_t, slave, write_source, src);
  char const *	buf;
  ccevents_source_disable_servicing(ccevents_source(&(slave->write_source)));
  fprintf(stderr, "%s: slave state %d\n", __func__, slave->state);

  switch (slave->state) {
  case 1: /* Send greetings. */
    buf = "hello\n";
    break;
  case 3: /* Send quitting. */
    buf = "quit\n";
    break;
  default:
    assert(0);
    break;
  }

  fprintf(stderr, "slave: send '%s'\n", buf);
  if (strlen(buf) != cce_sys_write(there, slave->write_source.fd, buf, strlen(buf))) {
    cce_raise(there, cce_condition_new_errno_clear());
  }
  fprintf(stderr, "slave: sent '%s'\n", buf);

  switch (slave->state) {
  case 1: /* Done greetings. */
    slave->state = 2;
    ccevents_source_enable_servicing(ccevents_source(&(slave->read_source)));
    sleep_awhile();
    break;
  case 3: /* Done quitting.  End. */
    slave->state = 0;
    ccevents_source_dequeue_itself(ccevents_source(&(slave->read_source)));
    ccevents_source_dequeue_itself(ccevents_source(&(slave->write_source)));
    break;
  }
}


int
main (int argc CCEVENTS_UNUSED, const char *const argv[] CCEVENTS_UNUSED)
{
  ccevents_init();
  //
  if (1) test_1_0_standalone_readability();
  if (1) test_2_0_standalone_writability();
  if (1) test_3_0_standalone_exception();
  //
  if (1) test_4_0_talking_processes_with_groups();
  //
  exit(EXIT_SUCCESS);
}

/* end of file */
