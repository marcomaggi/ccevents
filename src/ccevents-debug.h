/*
  Part of: CCEvents
  Contents: debugging macros and functions
  Date: Sat Jun 28, 2008

  Abstract

        Define a set of generic macros and functions.

  Copyright (c) 2008-2011, 2017 Marco Maggi <marco.maggi-ipsu@poste.it>

  This program is free software:  you can redistribute it and/or modify
  it under the terms of the  GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or (at
  your option) any later version.

  This program is  distributed in the hope that it  will be useful, but
  WITHOUT  ANY   WARRANTY;  without   even  the  implied   warranty  of
  MERCHANTABILITY  or FITNESS FOR  A PARTICULAR  PURPOSE.  See  the GNU
  General Public License for more details.

  You should  have received  a copy of  the GNU General  Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef CCEVENTS_DEBUG_H
#define CCEVENTS_DEBUG_H 1

#ifndef CCEVENTS_DEBUGGING
#  define CCEVENTS_DEBUGGING			0
#endif
#ifndef CCEVENTS_DEBUGGING_HERE
#  define CCEVENTS_DEBUGGING_HERE		1
#endif
#ifndef CCEVENTS_FPRINTF
#  define CCEVENTS_FPRINTF			fprintf
#endif
#ifndef CCEVENTS_VFPRINTF
#  define CCEVENTS_VFPRINTF			vfprintf
#endif

#if (CCEVENTS_DEBUGGING == 1)
#  warning debugging is on

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#  define ccevents_debug_question(EXPR)		((EXPR)?"yes":"no")
#  define ccevents_debug_mark()			ccevents_debug("mark")

#  define ccevents_debug(...)					\
								\
  if (CCEVENTS_DEBUGGING_HERE)					\
    ccevents_debug_print(__FILE__,__func__,__LINE__,__VA_ARGS__)

static void CCEVENTS_UNUSED
ccevents_debug_print (const char * file, const char * function, int line, const char * template, ...)
{
  va_list	ap;
  int		len, i;
  /* Move "file" to the last '/' char, so that we print file's tail name
     but not the directory part. */
  len = strlen(file);
  for (i=len-1; (i>=0) && ('/' != file[i]) ; --i);
  if ('/' == file[i])
    ++i;
  file += i;
/*   CCEVENTS_FPRINTF(stderr, "%s:%-25s (%d) *** ", file, function, line); */
  CCEVENTS_FPRINTF(stderr, "%-30s(%s:%d) *** ", function, file, line);
  va_start(ap, template);
  CCEVENTS_VFPRINTF(stderr, template, ap);
  CCEVENTS_FPRINTF(stderr, "\n");
  fflush(stderr);
  va_end(ap);
}

#else
#  define ccevents_debug(...)		/* empty */
#  define ccevents_debug_mark()		/* empty */
#  define ccevents_debug_question(...)	/* empty */
#endif

#endif /* CCEVENTS_DEBUG_H */

/* end of file */
