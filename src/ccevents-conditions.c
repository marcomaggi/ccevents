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

static ccevents_condition_base_descriptor_t condition_base_descriptor = {
  /* This "parent" field is set by the initialisation function. */
  .parent		= NULL,
  .free			= base_condition_destructor,
  .static_message	= base_condition_static_message
};

const ccevents_condition_base_descriptor_t * ccevents_condition_base_descriptor = &condition_base_descriptor;

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
bool
ccevents_condition_is_a_base (const cce_condition_t * C)
{
  return cce_condition_is_a(C, &condition_base_descriptor);
}


/** --------------------------------------------------------------------
 ** Exceptional condition descriptor: timeval base condition.
 ** ----------------------------------------------------------------- */

static void		condition_timeval_destructor     (cce_condition_t * C);
static const char *	condition_timeval_static_message (const cce_condition_t * C);

static const ccevents_condition_timeval_descriptor_t condition_timeval_descriptor = {
  .parent		= &condition_base_descriptor,
  .free			= condition_timeval_destructor,
  .static_message	= condition_timeval_static_message
};

const ccevents_condition_timeval_descriptor_t * \
  ccevents_condition_timeval_descriptor = &condition_timeval_descriptor;

/* This struct type has no dynamic fields, so there is only one instance
   of this struct type.  We allocate it statically. */
static const ccevents_condition_timeval_t condition_timeval = {
  .descriptor = &condition_timeval_descriptor
};

const ccevents_condition_timeval_t *
ccevents_condition_timeval (void)
{
  return &condition_timeval;
}
void
condition_timeval_destructor (cce_condition_t * C CCEVENTS_UNUSED)
{
  return;
}
const char *
condition_timeval_static_message (const cce_condition_t * C CCEVENTS_UNUSED)
{
  return "timeval exception";
}
bool
ccevents_condition_is_a_timeval (const cce_condition_t * C)
{
  return cce_condition_is_a(C, &condition_timeval_descriptor);
}


/** --------------------------------------------------------------------
 ** Exceptional condition descriptor: timeval invalid.
 ** ----------------------------------------------------------------- */

static void		condition_timeval_invalid_destructor     (cce_condition_t * C);
static const char *	condition_timeval_invalid_static_message (const cce_condition_t * C);

static const ccevents_condition_timeval_invalid_descriptor_t condition_timeval_invalid_descriptor = {
  .parent		= &condition_timeval_descriptor,
  .free			= condition_timeval_invalid_destructor,
  .static_message	= condition_timeval_invalid_static_message
};

const ccevents_condition_timeval_invalid_descriptor_t * \
  ccevents_condition_timeval_invalid_descriptor = &condition_timeval_invalid_descriptor;

/* This struct type has no dynamic fields, so there is only one instance
   of this struct type.  We allocate it statically. */
static const ccevents_condition_timeval_invalid_t condition_timeval_invalid = {
  .descriptor = &condition_timeval_invalid_descriptor
};

const ccevents_condition_timeval_invalid_t *
ccevents_condition_timeval_invalid (void)
{
  return &condition_timeval_invalid;
}
void
condition_timeval_invalid_destructor (cce_condition_t * C CCEVENTS_UNUSED)
{
  return;
}
const char *
condition_timeval_invalid_static_message (const cce_condition_t * C CCEVENTS_UNUSED)
{
  return "timeval fields specification are invalid";
}
bool
ccevents_condition_is_a_timeval_invalid (const cce_condition_t * C)
{
  return cce_condition_is_a(C, &condition_timeval_invalid_descriptor);
}


/** --------------------------------------------------------------------
 ** Exceptional condition descriptor: timeval overflow.
 ** ----------------------------------------------------------------- */

static void		condition_timeval_overflow_destructor     (cce_condition_t * C);
static const char *	condition_timeval_overflow_static_message (const cce_condition_t * C);

static const ccevents_condition_timeval_overflow_descriptor_t condition_timeval_overflow_descriptor = {
  .parent		= &condition_timeval_descriptor,
  .free			= condition_timeval_overflow_destructor,
  .static_message	= condition_timeval_overflow_static_message
};

const ccevents_condition_timeval_overflow_descriptor_t * \
  ccevents_condition_timeval_overflow_descriptor = &condition_timeval_overflow_descriptor;

/* This struct type has no dynamic fields, so there is only one instance
   of this struct type.  We allocate it statically. */
static const ccevents_condition_timeval_overflow_t condition_timeval_overflow = {
  .descriptor = &condition_timeval_overflow_descriptor
};

const ccevents_condition_timeval_overflow_t *
ccevents_condition_timeval_overflow (void)
{
  return &condition_timeval_overflow;
}
void
condition_timeval_overflow_destructor (cce_condition_t * C CCEVENTS_UNUSED)
{
  return;
}
const char *
condition_timeval_overflow_static_message (const cce_condition_t * C CCEVENTS_UNUSED)
{
  return "timeval fields specification would cause overflow in time representation";
}
bool
ccevents_condition_is_a_timeval_overflow (const cce_condition_t * C)
{
  return cce_condition_is_a(C, &condition_timeval_overflow_descriptor);
}


/** --------------------------------------------------------------------
 ** Exceptional condition descriptor: timeout invalid.
 ** ----------------------------------------------------------------- */

static void		condition_timeout_invalid_destructor  (cce_condition_t * C);
static const char *	condition_timeout_invalid_static_message (const cce_condition_t * C);

static ccevents_condition_descriptor_timeout_invalid_t condition_timeout_invalid_descriptor = {
  .parent		= &condition_timeval_descriptor,
  .free			= condition_timeout_invalid_destructor,
  .static_message	= condition_timeout_invalid_static_message
};

const ccevents_condition_descriptor_timeout_invalid_t * \
  ccevents_condition_timeout_invalid_descriptor = &condition_timeout_invalid_descriptor;

/* There  are  no  dynamic  fields  in  this  condition  object,  so  we
   statically allocate it. */
static const ccevents_condition_timeout_invalid_t condition_timeout_invalid = {
  .descriptor = &condition_timeout_invalid_descriptor
};

const ccevents_condition_timeout_invalid_t *
ccevents_condition_timeout_invalid (void)
{
  return &condition_timeout_invalid;
}
void
condition_timeout_invalid_destructor (cce_condition_t * C CCEVENTS_UNUSED)
{
  return;
}
const char *
condition_timeout_invalid_static_message (const cce_condition_t * C CCEVENTS_UNUSED)
{
  return "timeout fields specification would cause invalid time";
}
bool
ccevents_condition_is_a_timeout_invalid (const cce_condition_t * C)
{
  return cce_condition_is_a(C, &condition_timeout_invalid_descriptor);
}


/** --------------------------------------------------------------------
 ** Exceptional condition descriptor: timeout overflow.
 ** ----------------------------------------------------------------- */

static void             condition_timeout_overflow_destructor	(cce_condition_t * C);
static const char *     condition_timeout_overflow_static_message (const cce_condition_t * C);

static ccevents_condition_descriptor_timeout_overflow_t condition_timeout_overflow_descriptor = {
  .parent		= &condition_timeval_descriptor,
  .free			= condition_timeout_overflow_destructor,
  .static_message	= condition_timeout_overflow_static_message
};

const ccevents_condition_descriptor_timeout_overflow_t * \
  ccevents_condition_timeout_overflow_descriptor = &condition_timeout_overflow_descriptor;

/* There  are  no  dynamic  fields  in  this  condition  object,  so  we
   statically allocate it. */
static const ccevents_condition_timeout_overflow_t condition_timeout_overflow = {
  .descriptor = &condition_timeout_overflow_descriptor
};

const ccevents_condition_timeout_overflow_t *
ccevents_condition_timeout_overflow (void)
{
  return &condition_timeout_overflow;
}
void
condition_timeout_overflow_destructor (cce_condition_t * C CCEVENTS_UNUSED)
{
  return;
}
const char *
condition_timeout_overflow_static_message (const cce_condition_t * C CCEVENTS_UNUSED)
{
  return "timeout fields specification would cause overflow in time representation";
}
bool
ccevents_condition_is_a_timeout_overflow (const cce_condition_t * C)
{
  return cce_condition_is_a(C, &condition_timeout_overflow_descriptor);
}


void
ccevents_conditions_module_initialisation (void)
{
  condition_base_descriptor.parent = cce_root_condition_descriptor;
}

/* end of file */
