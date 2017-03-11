## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS		= -I meta/autotools
AUTOMAKE_OPTIONS	= foreign
EXTRA_DIST		= INSTALL configure.sh
dist_doc_DATA		= README COPYING
AM_CFLAGS		= -Wall -Wextra $(AX_CFLAGS)

## --------------------------------------------------------------------

EXTRA_DIST		+= meta/scripts/ccevents.pc.in
pkgconfigdir		= $(libdir)/pkgconfig
nodist_pkgconfig_DATA	= meta/scripts/ccevents.pc

## --------------------------------------------------------------------

AM_MAKEINFOFLAGS	= --no-split

info_TEXINFOS		= doc/ccevents.texi
doc_ccevents_TEXINFOS	= \
	doc/macros.texi		\
	doc/lgpl-3.0.texi	\
	doc/fdl-1.3.texi

#page
#### libraries

ccevents_CURRENT	= @ccevents_VERSION_INTERFACE_CURRENT@
ccevents_REVISION	= @ccevents_VERSION_INTERFACE_REVISION@
ccevents_AGE		= @ccevents_VERSION_INTERFACE_AGE@

include_HEADERS		= src/ccevents.h

lib_LTLIBRARIES		= libccevents.la
libccevents_la_CPPFLAGS	= $(CCEXCEPTIONS_CFLAGS)
libccevents_la_LDFLAGS	= -version-info $(ccevents_CURRENT):$(ccevents_REVISION):$(ccevents_AGE) \
			  $(CCEXCEPTIONS_LIBS)
libccevents_la_SOURCES= \
	src/ccevents.h			\
	src/ccevents-internals.h	\
	src/ccevents-debug.h		\
	src/ccevents-core.c		\
	src/ccevents-version.c		\
	src/ccevents-conditions.c	\
	src/ccevents-timevals.c		\
	src/ccevents-timeouts.c		\
	src/ccevents-queues.c		\
	src/ccevents-loops.c		\
	src/ccevents-groups.c		\
	src/ccevents-sources-base.c	\
	src/ccevents-sources-task.c	\
	src/ccevents-sources-timer.c	\
	src/ccevents-sources-signal.c	\
	src/ccevents-sources-fd.c

#page
#### interface to "make check"
#
# Read "Parallel Test Harness" in the documentation of GNU Automake to
# understand how to use this interface for "make check".
#

check_PROGRAMS	= \
	tests/version			\
	tests/timevals			\
	tests/timeouts			\
	tests/groups			\
	tests/base-sources		\
	tests/task-sources		\
	tests/timer-sources		\
	tests/standalone-fd-sources	\
	tests/signal-sources		\
	tests/main-loop

TESTS				= $(check_PROGRAMS)

ccevents_tests_cppflags		= -I$(srcdir)/src $(CCEXCEPTIONS_FLAGS)
ccevents_tests_ldadd		= libccevents.la

tests_version_CPPFLAGS			= $(ccevents_tests_cppflags)
tests_version_LDADD			= $(ccevents_tests_ldadd)

tests_groups_CPPFLAGS			= $(ccevents_tests_cppflags)
tests_groups_LDADD			= $(ccevents_tests_ldadd)

tests_timeouts_CPPFLAGS			= $(ccevents_tests_cppflags)
tests_timeouts_LDADD			= $(ccevents_tests_ldadd)

tests_timevals_CPPFLAGS			= $(ccevents_tests_cppflags)
tests_timevals_LDADD			= $(ccevents_tests_ldadd)

tests_base_sources_CPPFLAGS		= $(ccevents_tests_cppflags)
tests_base_sources_LDADD		= $(ccevents_tests_ldadd)

tests_task_sources_CPPFLAGS		= $(ccevents_tests_cppflags)
tests_task_sources_LDADD		= $(ccevents_tests_ldadd)

tests_timer_sources_CPPFLAGS		= $(ccevents_tests_cppflags)
tests_timer_sources_LDADD		= $(ccevents_tests_ldadd)

tests_standalone_fd_sources_CPPFLAGS	= $(ccevents_tests_cppflags)
tests_standalone_fd_sources_LDADD	= $(ccevents_tests_ldadd)

tests_signal_sources_CPPFLAGS		= $(ccevents_tests_cppflags)
tests_signal_sources_LDADD		= $(ccevents_tests_ldadd)

tests_main_loop_CPPFLAGS		= $(ccevents_tests_cppflags)
tests_main_loop_LDADD			= $(ccevents_tests_ldadd)

#page
#### interface to "make instcheck"

installcheck-local: $(TESTS)
	@for f in $(TESTS); do $(builddir)/$$f; done

#page
#### running test programs under gdb

GDB_ENV		= LD_LIBRARY_PATH=$(builddir)/.libs:$(LD_LIBRARY_PATH); export LD_LIBRARY_PATH;
GDB		= gdb
GDB_RUN		= $(GDB_ENV) $(GDB)

.PHONY: gdb-test

gdb-test:
	test -f $(builddir)/.gdbinit || ln -s $(top_srcdir)/.gdbinit $(builddir)/.gdbinit
	$(GDB_RUN) $(builddir)/tests/.libs/*$(file)*

#page
#### demo programs

EXTRA_DIST	+= \
	demos/processes-communicating-through-pipes.c

noinst_PROGRAMS	= \
	demos/processes-communicating-through-pipes

ccevents_demos_cppflags		= -I$(srcdir)/src $(CCEXCEPTIONS_FLAGS)
ccevents_demos_ldadd		= libccevents.la

demos_processes_communicating_through_pipes_CPPFLAGS	= $(ccevents_demos_cppflags)
demos_processes_communicating_through_pipes_LDADD	= $(ccevents_demos_ldadd)

### end of file
