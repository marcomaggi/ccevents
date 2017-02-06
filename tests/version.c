/*
  Part of: CCEvents
  Contents: test for version functions
  Date: Jan 11, 2017

  Abstract

	Test file for version functions.

  Copyright (C) 2017 Marco Maggi <marco.maggi-ipsu@poste.it>

  See the COPYING file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <ccevents.h>

int
main (int argc CCEVENTS_UNUSED, const char *const argv[] CCEVENTS_UNUSED)
{
  ccevents_init();
  printf("version number string: %s\n", cct_version_string());
  printf("libtool version number: %d:%d:%d\n",
	 cct_version_interface_current(),
	 cct_version_interface_revision(),
	 cct_version_interface_age());
  exit(EXIT_SUCCESS);
}

/* end of file */
