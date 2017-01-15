dnl @configure_input@
dnl

AC_PREREQ([2.69])
AC_INIT([CCEvents],[0.1d0],
  [marco.maggi-ipsu@poste.it],
  [ccevents],[http://marcomaggi.github.com/ccevents])
dnl This is the revision number of this configure.ac
AC_REVISION([0.1])
AC_COPYRIGHT([Copyright (c) 2017 Marco Maggi <marco.maggi-ipsu@poste.it>

This program is free software: you  can redistribute it and/or modify it
under the  terms of the GNU  General Public License as  published by the
Free Software Foundation,  either version 3 of the License,  or (at your
option) any later version.

This program  is distributed  in the  hope that it  will be  useful, but
WITHOUT   ANY   WARRANTY;  without   even   the   implied  warranty   of
MERCHANTABILITY  or  FITNESS FOR  A  PARTICULAR  PURPOSE.  See  the  GNU
General Public License for more details.

You should have received a copy  of the GNU General Public License along
with this program.  If not, see <http://www.gnu.org/licenses/>.
])
AC_CONFIG_SRCDIR([src/])
AC_CONFIG_MACRO_DIR([meta/autotools])
AC_CONFIG_AUX_DIR([meta/autotools])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([foreign dist-xz no-dist-gzip subdir-objects])
AM_MAINTAINER_MODE

AM_PROG_AR
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P

LT_PREREQ([2.4])
LT_INIT

dnl page
#### libraries interface version

dnl This is the version stored in the pkg-config data file.
AC_SUBST([CCEVENTS_PKG_CONFIG_VERSION],[0.1])

MM_LIBTOOL_LIBRARY_VERSIONS([ccevents],1,0,0)

dnl page
#### basic system inspection

AC_LANG([C])
dnl AC_DEFINE([_GNU_SOURCE],[1],[enable everything GNU])
AC_PROG_CC_C99
AX_CHECK_COMPILE_FLAG([-std=c11],[],[AC_MSG_ERROR([*** Compiler does not support -std=c11])],[-pedantic])
AX_CFLAGS="-std=c11"
AX_CHECK_COMPILE_FLAG([-fplan9-extensions],[],[AC_MSG_ERROR([*** Compiler does not support -fplan9-extensions])],[$AX_CFLAGS])
AX_CFLAGS="$AX_CFLAGS -fplan9-extensions"
AC_SUBST(AX_CFLAGS)

AM_PROG_AS
AC_PROG_CC
AM_PROG_CC_C_O
AC_HEADER_STDC
dnl AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([assert.h stdint.h strings.h errno.h stddef.h])
AC_CHECK_HEADER([stdbool.h],,[AC_MSG_ERROR([unable to fine stdbool.h])])
AC_CHECK_HEADER([sys/select.h],,[AC_MSG_ERROR([unable to fine sys/select.h])])
dnl AC_CHECK_SIZEOF(void *)
dnl AC_C_CONST
dnl AC_C_INLINE
dnl AC_C_RESTRICT
dnl AC_TYPE_LONG_LONG_INT
dnl AC_TYPE_UNSIGNED_LONG_LONG_INT
dnl AC_TYPE_SIZE_T
dnl AC_TYPE_SSIZE_T
dnl AC_TYPE_INT8_T
dnl AC_TYPE_INT16_T
dnl AC_TYPE_INT32_T
dnl AC_TYPE_INT64_T
dnl AC_TYPE_UINT8_T
dnl AC_TYPE_UINT16_T
dnl AC_TYPE_UINT32_T
dnl AC_TYPE_UINT64_T

AC_CACHE_SAVE

dnl page
#### checks for library functions

AC_FUNC_MALLOC
dnl AC_FUNC_MEMCMP
dnl AC_CHECK_FUNCS([memmove memset strerror])
dnl AC_CHECK_FUNCS([mmap munmap msync mremap madvise mlock munlock mlockall munlockall mprotect])
dnl AC_CHECK_FUNCS([getenv])

AC_CACHE_SAVE

dnl page
#### external libraries

PKG_CHECK_MODULES([CCEXCEPTIONS],[ccexceptions >= 0.2])

AC_CACHE_SAVE

dnl page
#### operating system specific configuration

dnl case "$target_os" in
dnl   *linux*)
dnl     ;;
dnl   *bsd*)
dnl     ;;
dnl   *cygwin*)
dnl     ;;
dnl   *darwin*)
dnl     ccevents_OS_DARWIN=yes
dnl     ;;
dnl esac
dnl AM_CONDITIONAL([WANT_LINUX], [test "x$ccevents_OS_LINUX"  = xyes])
dnl AM_CONDITIONAL([WANT_BSD],   [test "x$ccevents_OS_BSD"    = xyes])
dnl AM_CONDITIONAL([WANT_CYGWIN],[test "x$ccevents_OS_CYGWIN" = xyes])
dnl AM_CONDITIONAL([WANT_DARWIN],[test "x$ccevents_OS_DARWIN" = xyes])

dnl page
#### finish

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile]
   [meta/scripts/ccevents.pc]
   [meta/slackware/slack-desc])
AC_OUTPUT

### end of file
# Local Variables:
# mode: autoconf
# page-delimiter: "^dnl page"
# End:
