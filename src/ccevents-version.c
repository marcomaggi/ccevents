/*
  Part of: Ccevents
  Contents: version functions
  Date: Jan 11, 2017

  Abstract



  Copyright (C) 2017 Marco Maggi <marco.maggi-ipsu@poste.it>

  This program is  free software: you can redistribute  it and/or modify
  it  under the  terms  of  the GNU  Lesser  General  Public License  as
  published by  the Free  Software Foundation, either  version 3  of the
  License, or (at your option) any later version.

  This program  is distributed in the  hope that it will  be useful, but
  WITHOUT   ANY  WARRANTY;   without  even   the  implied   warranty  of
  MERCHANTABILITY  or FITNESS  FOR A  PARTICULAR PURPOSE.   See  the GNU
  General Public License for more details.

  You  should have received  a copy  of the  GNU General  Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


/** --------------------------------------------------------------------
 ** Headers.
 ** ----------------------------------------------------------------- */

#include "ccevents-internals.h"


const char *
ccevents_version_string (void)
{
  return ccevents_VERSION_INTERFACE_STRING;
}
int
ccevents_version_interface_current (void)
{
  return ccevents_VERSION_INTERFACE_CURRENT;
}
int
ccevents_version_interface_revision (void)
{
  return ccevents_VERSION_INTERFACE_REVISION;
}
int
ccevents_version_interface_age (void)
{
  return ccevents_VERSION_INTERFACE_AGE;
}

/* end of file */
