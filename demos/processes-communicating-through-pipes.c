/*
  Part of: CCEvents
  Contents: demo program of two processes communicating through Unix pipes
  Date: Thu Mar  9, 2017

  Abstract



  Copyright (C) 2017 Marco Maggi <marco.maggi-ipsu@poste.it>

  The author hereby  grant permission to use,  copy, modify, distribute,
  and  license this  software  and its  documentation  for any  purpose,
  provided that  existing copyright notices  are retained in  all copies
  and that  this notice  is included verbatim  in any  distributions. No
  written agreement, license, or royalty fee  is required for any of the
  authorized uses.  Modifications to this software may be copyrighted by
  their authors and need not  follow the licensing terms described here,
  provided that the new terms are clearly indicated on the first page of
  each file where they apply.

  IN NO  EVENT SHALL THE AUTHOR  OR DISTRIBUTORS BE LIABLE  TO ANY PARTY
  FOR  DIRECT, INDIRECT, SPECIAL,  INCIDENTAL, OR  CONSEQUENTIAL DAMAGES
  ARISING OUT  OF THE  USE OF THIS  SOFTWARE, ITS DOCUMENTATION,  OR ANY
  DERIVATIVES  THEREOF, EVEN  IF THE  AUTHOR  HAVE BEEN  ADVISED OF  THE
  POSSIBILITY OF SUCH DAMAGE.

  THE  AUTHOR AND  DISTRIBUTORS  SPECIFICALLY  DISCLAIM ANY  WARRANTIES,
  INCLUDING,   BUT   NOT  LIMITED   TO,   THE   IMPLIED  WARRANTIES   OF
  MERCHANTABILITY,    FITNESS   FOR    A    PARTICULAR   PURPOSE,    AND
  NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED  ON AN "AS IS" BASIS, AND
  THE AUTHOR AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE MAINTENANCE,
  SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/


/** --------------------------------------------------------------------
 ** Headers.
 ** ----------------------------------------------------------------- */

#include "ccevents.h"
#include <ccexceptions-system.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* ------------------------------------------------------------------ */

static const char *	progname = "processes-communicating-through-pipes";

/* ------------------------------------------------------------------ */

typedef struct all_filedes_t		all_filedes_t;

struct all_filedes_t {
  int	parent_write_child_read;
  int	child_write_parent_read;
  int	parent_read_child_write;
  int	child_read_parent_write;
};

/* ------------------------------------------------------------------ */

static all_filedes_t allocate_all_filedes (cce_location_t * there);
static void parent_process (int parent_read_child_write_fd, int parent_write_child_read_fd);
static void child_process  (int child_read_parent_write_fd, int child_write_parent_read_fd);

static void master_read_event_handler  (cce_location_t * there, ccevents_source_t * src);
static void master_write_event_handler (cce_location_t * there, ccevents_source_t * src);

static void slave_read_event_handler  (cce_location_t * there, ccevents_source_t * src);
static void slave_write_event_handler (cce_location_t * there, ccevents_source_t * src);


/** --------------------------------------------------------------------
 ** Handling "all_filedes_t" as asynchronous resources with CCExceptions.
 ** ----------------------------------------------------------------- */

typedef struct all_filedes_H_t	all_filedes_H_t;

struct all_filedes_H_t {
  cce_handler_t		handler;
  all_filedes_t *	fds;
};

/* ------------------------------------------------------------------ */

__attribute__((const,always_inline))
static inline all_filedes_H_t *
cce_cast_to_all_filedes_handler_from_handler (cce_handler_t * src)
{
  return (all_filedes_H_t *)src;
}
#define cce_cast_to_handler_all_filedes(SRC)	\
  _Generic((SRC), cce_handler_t *: cce_cast_to_all_filedes_handler_from_handler)(SRC)

cce_decl void cce_cleanup_handler_all_filedes_init (cce_location_t * there, all_filedes_H_t * H, all_filedes_t * fds)
  __attribute__((nonnull(1,2,3)));

cce_decl void cce_error_handler_all_filedes_init (cce_location_t * there, all_filedes_H_t * H, all_filedes_t * fds)
  __attribute__((nonnull(1,2,3)));

/* ------------------------------------------------------------------ */

static void
cce_handler_all_filedes_close (const cce_condition_t * C CCE_UNUSED, cce_handler_t * _H)
{
  all_filedes_H_t *	H = cce_cast_to_handler_all_filedes(_H);
  close(H->fds->parent_write_child_read);
  close(H->fds->child_write_parent_read);
  close(H->fds->parent_read_child_write);
  close(H->fds->child_read_parent_write);
}

/* ------------------------------------------------------------------ */

void
cce_cleanup_handler_all_filedes_init (cce_location_t * there, all_filedes_H_t * H, all_filedes_t * fds)
{
  H->handler.function	= cce_handler_all_filedes_close;
  H->fds		= fds;
  cce_register_cleanup_handler(there, &(H->handler));
}
void
cce_error_handler_all_filedes_init (cce_location_t * there, all_filedes_H_t * H, all_filedes_t * fds)
{
  H->handler.function	= cce_handler_all_filedes_close;
  H->fds		= fds;
  cce_register_error_handler(there, &(H->handler));
}


int
main (int argc CCEVENTS_UNUSED, const char *const argv[] CCEVENTS_UNUSED)
{
  all_filedes_t	fds;
  int		pid;

  ccevents_init();

  {
    cce_location_t	L[1];
    all_filedes_H_t	fds_H[1];

    if (cce_location(L)) {
      cce_run_error_handlers(L);
      fprintf(stderr, "%s: error: %s\n", progname,
	      cce_condition_static_message(cce_condition(L)));
      cce_condition_delete(cce_condition(L));
      exit(EXIT_FAILURE);
    } else {
      fds = allocate_all_filedes(L);
      cce_error_handler_all_filedes_init(L, fds_H, &fds);
      pid = cce_sys_fork(L);
      cce_run_cleanup_handlers(L);
    }
  }

  /* If  we  are here:  forking  succeeded,  and  we  can use  the  file
     descriptors in FDS. */
  if (0 == pid) {
    /* This is the child process. */
    child_process(fds.child_read_parent_write, fds.child_write_parent_read);
  } else {
    /* This is the parent process. */
    cce_location_t	L[1];

    if (cce_location(L)) {
      cce_run_error_handlers(L);
      fprintf(stderr, "%s: error: %s\n", progname,
	      cce_condition_static_message(cce_condition(L)));
      cce_condition_delete(cce_condition(L));
      exit(EXIT_FAILURE);
    } else {
      parent_process(fds.parent_read_child_write, fds.parent_write_child_read);

      /* Wait for the child process. */
      {
	int	wstatus = 0;
	cce_sys_waitpid(L, pid, &wstatus, 0);
	assert(WIFEXITED(wstatus));
	assert(0 == WEXITSTATUS(wstatus));
      }
      cce_run_cleanup_handlers(L);
      exit(EXIT_SUCCESS);
    }
  }
}


all_filedes_t
allocate_all_filedes (cce_location_t * there)
/* Allocate the pipes needed for  communication between parent and child
   processes.  Return the  whole structure as a  value; inefficient, but
   we do not care here. */
{
  cce_location_t	L[1];
  cce_handler_t		forwards_pipe_H[1];
  cce_handler_t		backwards_pipe_H[1];
  int			forwards_pipe[2];
  int			backwards_pipe[2];
  all_filedes_t		fds;

  if (cce_location(L)) {
    cce_run_error_handlers(L);
    cce_raise(there, cce_condition(L));
  } else {
    /* "forwards_pipe[0]" is the readable end; "forwards_pipe[1]" is the
       writable end. */
    cce_sys_pipe(L, forwards_pipe);
    cce_error_handler_pipedes_init(L, forwards_pipe_H, forwards_pipe);

    /* "backwards_pipe[0]" is  the readable end;  "backwards_pipe[1]" is
       the writable end. */
    cce_sys_pipe(L, backwards_pipe);
    cce_error_handler_pipedes_init(L, backwards_pipe_H, backwards_pipe);

    fds.parent_read_child_write = forwards_pipe[0];
    fds.child_write_parent_read = forwards_pipe[1];
    fds.child_read_parent_write = backwards_pipe[0];
    fds.parent_write_child_read = backwards_pipe[1];

    if (0) {
      fprintf(stderr, "%s: parent read fd=%d, child write fd=%d, child read fd=%d, parent write fd=%d\n", __func__,
	      fds.parent_read_child_write, fds.child_write_parent_read,
	      fds.child_read_parent_write, fds.parent_write_child_read);
    }

    /* Run this to test the reaction to pipe creation errors. */
    if (0) { cce_raise(there, cce_condition_new_unknown()); }

    cce_run_cleanup_handlers(L);
    return fds;
  }
}


/** --------------------------------------------------------------------
 ** Master process state.
 ** ----------------------------------------------------------------- */

typedef enum master_state_code_t {
  MASTER_READ_GREETINGS,
  MASTER_SEND_GREETINGS,
  MASTER_READ_FAREWELL,
  MASTER_SEND_FAREWELL,
  MASTER_DONE,
} master_state_code_t;

/* ------------------------------------------------------------------ */

typedef struct master_state_t		master_state_t;

/* This structure represents the state of the master process. */
struct master_state_t {
  ccevents_loop_t	loop[1];
  ccevents_group_t	grp[1];
  ccevents_fd_source_t	read_source[1];
  ccevents_fd_source_t	write_source[1];
  int			state;
  cce_condition_t *	exceptional_condition;
};

static master_state_t *
master_state_pointer_from_read_source_pointer (ccevents_source_t * src)
/* Given  a  pointer to  the  field  READ_SOURCE in  a  "master_state_t"
   structure:  return  a  pointer  to  the first  machine  word  in  the
   structure. */
{
  return (master_state_t *)(((uint8_t *)src) - offsetof(struct master_state_t, read_source));
}

static master_state_t *
master_state_pointer_from_write_source_pointer (ccevents_source_t * src)
/* Given  a pointer  to  the field  WROTE_SOURCE  in a  "master_state_t"
   structure:  return  a  pointer  to  the first  machine  word  in  the
   structure. */
{
  return (master_state_t *)(((uint8_t *)src) - offsetof(struct master_state_t, write_source));
}

/* ------------------------------------------------------------------ */

void
master_state_init (master_state_t * S, int parent_read_fd, int parent_write_fd)
/* Initialise  an  already  allocated "master_state_t"  structure.   Set
   everything that is required to enter the events loop. */
{
  ccevents_fd_source_init(S->read_source,  parent_read_fd);
  ccevents_fd_source_init(S->write_source, parent_write_fd);
  ccevents_fd_source_set(S->read_source,  ccevents_query_fd_readability, master_read_event_handler);
  ccevents_fd_source_set(S->write_source, ccevents_query_fd_writability, master_write_event_handler);
  ccevents_source_set_timeout(ccevents_source(S->read_source),  *CCEVENTS_TIMEVAL_NEVER, ccevents_dummy_timeout_handler);
  ccevents_source_set_timeout(ccevents_source(S->write_source), *CCEVENTS_TIMEVAL_NEVER, ccevents_dummy_timeout_handler);
  ccevents_group_init(S->grp, 2);
  ccevents_loop_init(S->loop);
  ccevents_group_enqueue_source(S->grp, ccevents_source(S->read_source));
  ccevents_group_enqueue_source(S->grp, ccevents_source(S->write_source));
  ccevents_loop_enqueue_group(S->loop, S->grp);
  /* The parent  process is  the master  and it  starts the  dialogue by
     writing. */
  ccevents_source_disable_servicing(ccevents_source(S->read_source));
  S->state			= MASTER_SEND_GREETINGS;
  S->exceptional_condition	= NULL;
}

void
master_state_final (master_state_t * S)
/* Finalise a  "master_state_t" structure.   Close the  file descriptors
   used  to  communicate with  the  child  process; ignore  the  closing
   errors, if any. */
{
  ccevents_loop_final(S->loop);
  close(ccevents_fd(S->read_source));
  close(ccevents_fd(S->write_source));
  if (0) { fprintf(stderr, "%s: done\n", __func__); }
}

/* ------------------------------------------------------------------ */

void
master_task_final (master_state_t * S)
/* Call  this function  to  remove from  the event  loop  all the  event
   sources associated to the master task  and post a request to exit the
   loop.

   We are meant  to use this function when the  master task is finished,
   or  if an  unrecoverable  error occurs  while  performing the  task's
   operations. */
{
  S->state = MASTER_DONE;
  ccevents_source_dequeue_itself(ccevents_source(S->read_source));
  ccevents_source_dequeue_itself(ccevents_source(S->write_source));
  ccevents_loop_post_request_to_leave(S->loop);
}


/** --------------------------------------------------------------------
 ** Handling master process state as CCExceptions resources.
 ** ----------------------------------------------------------------- */

typedef struct master_state_H_t	master_state_H_t;

struct master_state_H_t {
  cce_handler_t		handler;
  master_state_t *	state;
};

/* ------------------------------------------------------------------ */

static void
cleanup_handler_master_state (const cce_condition_t * C CCEVENTS_UNUSED, cce_handler_t * _H)
{
  master_state_H_t *	H = (master_state_H_t *) _H;
  master_state_final(H->state);
}

/* ------------------------------------------------------------------ */

void
cleanup_handler_master_state_init (cce_location_t * there, master_state_H_t * S_handler, master_state_t * S)
{
  S_handler->state		= S;
  S_handler->handler.function	= cleanup_handler_master_state;
  cce_register_cleanup_handler(there, &(S_handler->handler));
}

void
error_handler_master_state_init (cce_location_t * there, master_state_H_t * S_handler, master_state_t * S)
{
  S_handler->state		= S;
  S_handler->handler.function	= cleanup_handler_master_state;
  cce_register_error_handler(there, &(S_handler->handler));
}


void
parent_process (int parent_read_child_write_fd, int parent_write_child_read_fd)
/* Parent process  main function.  Initialise  the event loop  and enter
   it.  When communication  is finished: check for  errors; finalise the
   structures; finally return. */
{
  cce_location_t	L[1];
  cce_handler_t		read_fd_H[1];
  cce_handler_t		write_fd_H[1];
  master_state_t	S;
  master_state_H_t	state_H[1];

  if (0) { fprintf(stderr, "%s: enter\n", __func__); }

  if (cce_location(L)) {
    cce_run_error_handlers(L);
    fprintf(stderr, "%s error: %s\n", __func__, cce_condition_static_message(cce_condition(L)));
    cce_condition_delete(cce_condition(L));
  } else {
    cce_cleanup_handler_filedes_init(L, read_fd_H,  parent_read_child_write_fd);
    cce_cleanup_handler_filedes_init(L, write_fd_H, parent_write_child_read_fd);

    master_state_init(&S, parent_read_child_write_fd, parent_write_child_read_fd);
    cleanup_handler_master_state_init(L, state_H, &S);
    ccevents_loop_enter(S.loop);

    /* Are we out of the loop because of an exceptional condition? */
    if (S.exceptional_condition) {
      fprintf(stderr, "%s: master task error: %s\n", progname,
	      cce_condition_static_message(S.exceptional_condition));
      cce_condition_delete(S.exceptional_condition);
    }

    cce_run_cleanup_handlers(L);
  }
}


void
master_read_event_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src)
/* Read event handler.   This function is called to read  bytes from the
   file descriptor source "S->read_source".

   The  argument SRC  is  a  pointer to  the  field  READ_SOURCE in  the
   structure "master_state_t". */
{
  static const char	expected_greetings[] = "hello\n";
  static const char	expected_farewell[]  = "quit\n";
  cce_location_t	L[1];
  master_state_t *	S = master_state_pointer_from_read_source_pointer(src);

  if (0) { fprintf(stderr, "%s: enter\n", __func__); }

  if (cce_location(L)) {
    cce_run_error_handlers(L);
    S->exceptional_condition = cce_condition(L);
    master_task_final(S);
  } else {
    char	buf[10];
    size_t	count = cce_sys_read(L, S->read_source->fd, buf, 10);

    buf[count] = '\0';
    fprintf(stderr, "master: recv '%s'\n", buf);
    switch (S->state) {
    case MASTER_READ_GREETINGS:
      {
	assert(0 == strncmp(expected_greetings, buf, strlen(expected_greetings)));
	S->state = MASTER_SEND_FAREWELL;
	ccevents_source_disable_servicing(ccevents_source(S->read_source));
	ccevents_source_enable_servicing(ccevents_source(S->write_source));
      }
      break;

    case MASTER_READ_FAREWELL:
      {
	assert(0 == strncmp(expected_farewell, buf, strlen(expected_farewell)));
	master_task_final(S);
      }
      break;

    default:
      assert(0);
      break;
    }

    cce_run_cleanup_handlers(L);
  }
}


void
master_write_event_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src)
/* Write event handler.   This function is called to write  bytes to the
   file descriptor source "S->write_source".

   The  argument SRC  is  a pointer  to the  field  WRITE_SOURCE in  the
   structure "master_state_t". */
{
  static const char	greetings[] = "hello\n";
  static const char	farewell[]  = "quit\n";
  cce_location_t	L[1];
  master_state_t *	S = master_state_pointer_from_write_source_pointer(src);

  if (0) { fprintf(stderr, "%s: enter\n", __func__); }

  if (cce_location(L)) {
    cce_run_error_handlers(L);
    S->exceptional_condition = cce_condition(L);
    master_task_final(S);
  } else {
    const char *	message;

    ccevents_source_disable_servicing(ccevents_source(S->write_source));
    switch (S->state) {
    case MASTER_SEND_GREETINGS:
      message = greetings;
      break;
    case MASTER_SEND_FAREWELL:
      message = farewell;
      break;
    default:
      assert(0);
      break;
    }
    {
      size_t	count = 0;

      fprintf(stderr, "master: send '%s'\n", message);
      do {
	count = cce_sys_write(L, S->write_source->fd, message+count, strlen(message)-count);
      } while (strlen(message) != count);
    }
    switch (S->state) {
    case MASTER_SEND_GREETINGS:
      S->state = MASTER_READ_GREETINGS;
      ccevents_source_enable_servicing(ccevents_source(S->read_source));
      break;
    case MASTER_SEND_FAREWELL:
      S->state = MASTER_READ_FAREWELL;
      ccevents_source_enable_servicing(ccevents_source(S->read_source));
      break;
    default:
      assert(0);
      break;
    }
    cce_run_cleanup_handlers(L);
  }
}


/** --------------------------------------------------------------------
 ** Slave process state.
 ** ----------------------------------------------------------------- */

typedef enum slave_state_code_t {
  SLAVE_READ_GREETINGS,
  SLAVE_SEND_GREETINGS,
  SLAVE_READ_FAREWELL,
  SLAVE_SEND_FAREWELL,
  SLAVE_DONE,
} slave_state_code_t;

/* ------------------------------------------------------------------ */

typedef struct slave_state_t		slave_state_t;

/* This structure represents the state of the slave process. */
struct slave_state_t {
  ccevents_loop_t	loop[1];
  ccevents_group_t	grp[1];
  ccevents_fd_source_t	read_source[1];
  ccevents_fd_source_t	write_source[1];
  slave_state_code_t	state;
  cce_condition_t *	exceptional_condition;
};

/* ------------------------------------------------------------------ */

static slave_state_t *
/* Given  a  pointer  to  the field  READ_SOURCE  in  a  "slave_state_t"
   structure:  return  a  pointer  to  the first  machine  word  in  the
   structure. */
slave_state_pointer_from_read_source_pointer (ccevents_source_t * src)
{
  return (slave_state_t *)(((uint8_t *)src) - offsetof(struct slave_state_t, read_source));
}

static slave_state_t *
slave_state_pointer_from_write_source_pointer (ccevents_source_t * src)
/* Given  a  pointer to  the  field  WROTE_SOURCE in  a  "slave_state_t"
   structure:  return  a  pointer  to  the first  machine  word  in  the
   structure. */
{
  return (slave_state_t *)(((uint8_t *)src) - offsetof(struct slave_state_t, write_source));
}

/* ------------------------------------------------------------------ */

void
slave_state_init (slave_state_t * S, int child_read_fd, int child_write_fd)
/* Initialise  an  already  allocated  "slave_state_t"  structure.   Set
   everything that is required to enter the events loop. */
{
  ccevents_fd_source_init(S->read_source,  child_read_fd);
  ccevents_fd_source_init(S->write_source, child_write_fd);
  ccevents_fd_source_set(S->read_source,  ccevents_query_fd_readability, slave_read_event_handler);
  ccevents_fd_source_set(S->write_source, ccevents_query_fd_writability, slave_write_event_handler);
  ccevents_source_set_timeout(ccevents_source(S->read_source),  *CCEVENTS_TIMEVAL_NEVER, ccevents_dummy_timeout_handler);
  ccevents_source_set_timeout(ccevents_source(S->write_source), *CCEVENTS_TIMEVAL_NEVER, ccevents_dummy_timeout_handler);
  ccevents_group_init(S->grp, 2);
  ccevents_loop_init(S->loop);
  S->state			= SLAVE_READ_GREETINGS;
  S->exceptional_condition	= NULL;

  ccevents_group_enqueue_source(S->grp, ccevents_source(S->read_source));
  ccevents_group_enqueue_source(S->grp, ccevents_source(S->write_source));
  ccevents_loop_enqueue_group(S->loop, S->grp);
  /* The  child process  is  the slave  and it  starts  the dialogue  by
     reading. */
  ccevents_source_disable_servicing(ccevents_source(S->write_source));
}

void
slave_state_final (slave_state_t * S)
/* Finalise  a "slave_state_t"  structure.  Close  the file  descriptors
   used  to  communicate with  the  child  process; ignore  the  closing
   errors, if any. */
{
  ccevents_source_dequeue_itself(ccevents_source(S->read_source));
  ccevents_source_dequeue_itself(ccevents_source(S->write_source));
  ccevents_loop_final(S->loop);
  close(ccevents_fd(S->read_source));
  close(ccevents_fd(S->write_source));
  if (0) { fprintf(stderr, "%s: done\n", __func__); }
}

/* ------------------------------------------------------------------ */

void
slave_task_final (slave_state_t * S)
/* Call  this function  to  remove from  the event  loop  all the  event
   sources associated to  the slave task and post a  request to exit the
   loop.

   We are meant to use this function  when the slave task is finished or
   if  an  unrecoverable  error   occurs  while  performing  the  task's
   operations. */
{
  ccevents_source_dequeue_itself(ccevents_source(S->read_source));
  ccevents_source_dequeue_itself(ccevents_source(S->write_source));
  ccevents_loop_post_request_to_leave(S->loop);
  S->state = SLAVE_DONE;
}


/** --------------------------------------------------------------------
 ** Handling slave process state as CCExceptions resources.
 ** ----------------------------------------------------------------- */

typedef struct slave_state_H_t	slave_state_H_t;

struct slave_state_H_t {
  cce_handler_t		handler;
  slave_state_t *	state;
};

/* ------------------------------------------------------------------ */

static void
cleanup_handler_slave_state (const cce_condition_t * C CCEVENTS_UNUSED, cce_handler_t * _H)
{
  slave_state_H_t *	H = (slave_state_H_t *) _H;
  slave_state_final(H->state);
}

/* ------------------------------------------------------------------ */

void
cleanup_handler_slave_state_init (cce_location_t * there, slave_state_H_t * S_handler, slave_state_t * S)
{
  S_handler->state		= S;
  S_handler->handler.function	= cleanup_handler_slave_state;
  cce_register_cleanup_handler(there, &(S_handler->handler));
}

void
error_handler_slave_state_init (cce_location_t * there, slave_state_H_t * S_handler, slave_state_t * S)
{
  S_handler->state		= S;
  S_handler->handler.function	= cleanup_handler_slave_state;
  cce_register_error_handler(there, &(S_handler->handler));
}


void
child_process (int child_read_parent_write_fd, int child_write_parent_read_fd)
/* Child process main function.  Initialise the event loop and enter it.
   When  communication  is  finished:  check for  errors;  finalise  the
   structures; finally return. */
{
  cce_location_t	L[1];
  cce_handler_t		handler_write_fd[1];
  cce_handler_t		handler_read_fd[1];
  slave_state_t		S;
  slave_state_H_t	S_handler[1];

  if (0) { fprintf(stderr, "%s: enter\n", __func__); }

  if (cce_location(L)) {
    cce_run_error_handlers(L);
    fprintf(stderr, "%s error: %s\n", __func__, cce_condition_static_message(cce_condition(L)));
    cce_condition_delete(cce_condition(L));
    exit(EXIT_FAILURE);
  } else {
    cce_cleanup_handler_filedes_init(L, handler_read_fd,  child_read_parent_write_fd);
    cce_cleanup_handler_filedes_init(L, handler_write_fd, child_write_parent_read_fd);

    slave_state_init(&S, child_read_parent_write_fd, child_write_parent_read_fd);
    cleanup_handler_slave_state_init(L, S_handler, &S);
    ccevents_loop_enter(S.loop);

    /* Are we out of the loop because of an exceptional condition? */
    if (S.exceptional_condition) {
      fprintf(stderr, "slave task error: %s\n", cce_condition_static_message(S.exceptional_condition));
      cce_condition_delete(S.exceptional_condition);
    }

    cce_run_cleanup_handlers(L);
    exit(EXIT_SUCCESS);
  }
}


void
slave_read_event_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src)
/* Read event handler.   This function is called to read  bytes from the
   file descriptor source "S->read_source".

   The  argument SRC  is  a  pointer to  the  field  READ_SOURCE in  the
   structure "slave_state_t". */
{
  static const char	expected_greetings[] = "hello\n";
  static const char	expected_farewell[]  = "quit\n";
  cce_location_t	L[1];
  slave_state_t *	S = slave_state_pointer_from_read_source_pointer(src);

  if (0) { fprintf(stderr, "%s: enter\n", __func__); }

  if (cce_location(L)) {
    cce_run_error_handlers(L);
    S->exceptional_condition = cce_condition(L);
    slave_task_final(S);
  } else {
    char	buf[10];
    size_t	count;

    ccevents_source_disable_servicing(ccevents_source(S->read_source));
    count = cce_sys_read(L, S->read_source->fd, buf, 10);
    buf[count] = '\0';

    switch (S->state) {
    case SLAVE_READ_GREETINGS:
      {
	fprintf(stderr, "slave: recv '%s'\n", buf);
	assert(0 == strncmp(expected_greetings, buf, strlen(expected_greetings)));
	S->state = SLAVE_SEND_GREETINGS;
	ccevents_source_enable_servicing(ccevents_source(S->write_source));
      }
      break;

    case SLAVE_READ_FAREWELL:
      {
	fprintf(stderr, "slave: recv '%s'\n", buf);
	assert(0 == strncmp(expected_farewell, buf, strlen(expected_farewell)));
	S->state = SLAVE_SEND_FAREWELL;
	ccevents_source_enable_servicing(ccevents_source(S->write_source));
      }
      break;

    default:
      assert(0);
    }
    cce_run_cleanup_handlers(L);
  }
}


void
slave_write_event_handler (cce_location_t * there CCEVENTS_UNUSED, ccevents_source_t * src)
/* Write event handler.   This function is called to write  bytes to the
   file descriptor source "S->write_source".

   The  argument SRC  is  a pointer  to the  field  WRITE_SOURCE in  the
   structure "slave_state_t". */
{
  static const char	greetings[] = "hello\n";
  static const char	farewell[]  = "quit\n";
  cce_location_t	L[1];
  slave_state_t *	S = slave_state_pointer_from_write_source_pointer(src);

  if (0) { fprintf(stderr, "%s: enter\n", __func__); }

  if (cce_location(L)) {
    cce_run_error_handlers(L);
    S->exceptional_condition = cce_condition(L);
    slave_task_final(S);
  } else {
    const char *	message;

    ccevents_source_disable_servicing(ccevents_source(S->write_source));

    switch (S->state) {
    case SLAVE_SEND_GREETINGS:
      message = greetings;
      break;
    case SLAVE_SEND_FAREWELL:
      message = farewell;
      break;
    default:
      assert(0);
    }
    {
      size_t	count = 0;

      fprintf(stderr, "slave: send '%s'\n", message);
      do {
	count = cce_sys_write(L, S->write_source->fd, message + count, strlen(message) - count);
      } while (strlen(message) != count);
    }
    switch (S->state) {
    case SLAVE_SEND_GREETINGS:
      S->state = SLAVE_READ_FAREWELL;
      ccevents_source_enable_servicing(ccevents_source(S->read_source));
      break;
    case SLAVE_SEND_FAREWELL:
      S->state = SLAVE_DONE;
      ccevents_source_dequeue_itself(ccevents_source(S->read_source));
      ccevents_source_dequeue_itself(ccevents_source(S->write_source));
      ccevents_loop_post_request_to_leave(S->loop);
      break;
    default:
      assert(0);
    }
    cce_run_cleanup_handlers(L);
  }
}

/* end of file */
