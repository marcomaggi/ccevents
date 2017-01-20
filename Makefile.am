## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS		= -I meta/autotools
AUTOMAKE_OPTIONS	= foreign
EXTRA_DIST		= INSTALL configure.sh
dist_doc_DATA		= README COPYING
AM_CFLAGS		= -Wall $(AX_CFLAGS)

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
	src/ccevents-fd-sources.c

#page
#### tests

check_PROGRAMS	= \
	tests/version			\
	tests/timevals			\
	tests/timeouts			\
	tests/standalone-fd-sources

TESTS				= $(check_PROGRAMS)

ccevents_tests_cppflags		= -I$(srcdir)/src $(CCEXCEPTIONS_FLAGS)
ccevents_tests_ldadd		= libccevents.la

tests_version_CPPFLAGS			= $(ccevents_tests_cppflags)
tests_version_LDADD			= $(ccevents_tests_ldadd)

tests_timeouts_CPPFLAGS			= $(ccevents_tests_cppflags)
tests_timeouts_LDADD			= $(ccevents_tests_ldadd)

tests_timevals_CPPFLAGS			= $(ccevents_tests_cppflags)
tests_timevals_LDADD			= $(ccevents_tests_ldadd)

tests_standalone_fd_sources_CPPFLAGS	= $(ccevents_tests_cppflags)
tests_standalone_fd_sources_LDADD	= $(ccevents_tests_ldadd)

installcheck-local: $(TESTS)
	@for f in $(TESTS); do $(builddir)/$$f; done

### end of file
