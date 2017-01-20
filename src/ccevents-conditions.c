/*
  Part of: CCEvents
  Contents: exceptional condition types
  Date: Thu Jan 19, 2017

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



#include "ccevents-internals.h"


/** --------------------------------------------------------------------
 ** Base exceptional condition descriptor.
 ** ----------------------------------------------------------------- */

static void             base_condition_destructor     (cce_condition_t * C);
static const char *     base_condition_static_message (const cce_condition_t * C);

ccevents_condition_descriptor_base_t ccevents_base_condition_descriptor_stru = {
  /* This "parent" field is set by the initialisation function. */
  .parent		= NULL,
  .free			= base_condition_destructor,
  .static_message	= base_condition_static_message
};

const ccevents_condition_descriptor_base_t * ccevents_base_condition_descriptor = &ccevents_base_condition_descriptor_stru;

void
base_condition_destructor (cce_condition_t * C CCEVENTS_UNUSED)
{
  return;
}
const char *
base_condition_static_message (const cce_condition_t * C CCEVENTS_UNUSED)
{
  return "CCEvents exceptional condition";
}


void
ccevents_conditions_module_initialisation (void)
{
  ccevents_base_condition_descriptor_stru.parent = cce_root_condition_descriptor;
}

/* end of file */
