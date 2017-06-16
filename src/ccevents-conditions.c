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

static cce_condition_static_message_fun_t	descriptor_base_static_message;

static ccevents_descriptor_base_t descriptor_base = {
  /* This "parent" field is set by the initialisation function. */
  .descriptor.parent		= NULL,
  .descriptor.delete		= NULL,
  .descriptor.final		= NULL,
  .descriptor.static_message	= descriptor_base_static_message
};

ccevents_descriptor_base_t const * const ccevents_descriptor_base = &descriptor_base;

const char *
descriptor_base_static_message (cce_condition_t const * C CCEVENTS_UNUSED)
{
  return "CCEvents exceptional condition";
}


/** --------------------------------------------------------------------
 ** Exceptional condition descriptor: timeval base condition.
 ** ----------------------------------------------------------------- */

static cce_condition_static_message_fun_t	condition_timeval_static_message;

static const ccevents_descriptor_timeval_t descriptor_timeval = {
  .descriptor.parent		= &descriptor_base.descriptor,
  .descriptor.delete		= NULL,
  .descriptor.final		= NULL,
  .descriptor.static_message	= condition_timeval_static_message
};

ccevents_descriptor_timeval_t const * const ccevents_descriptor_timeval = &descriptor_timeval;

/* This struct type has no dynamic fields, so there is only one instance
   of this struct type.  We allocate it statically. */
static ccevents_condition_timeval_t const condition_timeval = {
  .base.condition.descriptor	= &descriptor_timeval.descriptor
};

cce_condition_t const *
ccevents_condition_new_timeval (void)
{
  return (cce_condition_t *)&condition_timeval;
}

const char *
condition_timeval_static_message (cce_condition_t const * C CCEVENTS_UNUSED)
{
  return "timeval exception";
}


/** --------------------------------------------------------------------
 ** Exceptional condition descriptor: timeval invalid.
 ** ----------------------------------------------------------------- */

static cce_condition_static_message_fun_t	condition_timeval_invalid_static_message;

static const ccevents_descriptor_timeval_invalid_t descriptor_timeval_invalid = {
  .descriptor.parent		= &descriptor_timeval.descriptor,
  .descriptor.delete		= NULL,
  .descriptor.final		= NULL,
  .descriptor.static_message	= condition_timeval_invalid_static_message
};

ccevents_descriptor_timeval_invalid_t const * const ccevents_descriptor_timeval_invalid = &descriptor_timeval_invalid;

/* This struct type has no dynamic fields, so there is only one instance
   of this struct type.  We allocate it statically. */
static ccevents_condition_timeval_invalid_t const condition_timeval_invalid = {
  .timeval.base.condition.descriptor	= &descriptor_timeval_invalid.descriptor
};

cce_condition_t const *
ccevents_condition_new_timeval_invalid (void)
{
  return (cce_condition_t *)&condition_timeval_invalid;
}

const char *
condition_timeval_invalid_static_message (cce_condition_t const * C CCEVENTS_UNUSED)
{
  return "timeval fields specification would cause invalid time representation";
}


/** --------------------------------------------------------------------
 ** Exceptional condition descriptor: timeval overflow.
 ** ----------------------------------------------------------------- */

static cce_condition_static_message_fun_t	condition_timeval_overflow_static_message;

static const ccevents_descriptor_timeval_overflow_t descriptor_timeval_overflow = {
  .descriptor.parent		= &descriptor_timeval.descriptor,
  .descriptor.delete		= NULL,
  .descriptor.final		= NULL,
  .descriptor.static_message	= condition_timeval_overflow_static_message
};

ccevents_descriptor_timeval_overflow_t const * const ccevents_descriptor_timeval_overflow = &descriptor_timeval_overflow;

/* This struct type has no dynamic fields, so there is only one instance
   of this struct type.  We allocate it statically. */
static ccevents_condition_timeval_overflow_t const condition_timeval_overflow = {
  .timeval.base.condition.descriptor	= &descriptor_timeval_overflow.descriptor
};

cce_condition_t const *
ccevents_condition_new_timeval_overflow (void)
{
  return (cce_condition_t *)&condition_timeval_overflow;
}

const char *
condition_timeval_overflow_static_message (cce_condition_t const * C CCEVENTS_UNUSED)
{
  return "timeval fields specification would cause overflow in time representation";
}


/** --------------------------------------------------------------------
 ** Exceptional condition descriptor: timeout base condition.
 ** ----------------------------------------------------------------- */

static cce_condition_static_message_fun_t	condition_timeout_static_message;

static const ccevents_descriptor_timeout_t descriptor_timeout = {
  .descriptor.parent		= &descriptor_base.descriptor,
  .descriptor.delete		= NULL,
  .descriptor.final		= NULL,
  .descriptor.static_message	= condition_timeout_static_message
};

ccevents_descriptor_timeout_t const * const ccevents_descriptor_timeout = &descriptor_timeout;

/* This struct type has no dynamic fields, so there is only one instance
   of this struct type.  We allocate it statically. */
static ccevents_condition_timeout_t const condition_timeout = {
  .base.condition.descriptor	= &descriptor_timeout.descriptor
};

cce_condition_t const *
ccevents_condition_new_timeout (void)
{
  return (cce_condition_t *)&condition_timeout;
}

const char *
condition_timeout_static_message (cce_condition_t const * C CCEVENTS_UNUSED)
{
  return "timeout exception";
}


/** --------------------------------------------------------------------
 ** Exceptional condition descriptor: timeout invalid.
 ** ----------------------------------------------------------------- */

static cce_condition_static_message_fun_t	condition_timeout_invalid_static_message;

static const ccevents_descriptor_timeout_invalid_t descriptor_timeout_invalid = {
  .descriptor.parent		= &descriptor_timeout.descriptor,
  .descriptor.delete		= NULL,
  .descriptor.final		= NULL,
  .descriptor.static_message	= condition_timeout_invalid_static_message
};

ccevents_descriptor_timeout_invalid_t const * const ccevents_descriptor_timeout_invalid = &descriptor_timeout_invalid;

/* This struct type has no dynamic fields, so there is only one instance
   of this struct type.  We allocate it statically. */
static ccevents_condition_timeout_invalid_t const condition_timeout_invalid = {
  .timeout.base.condition.descriptor	= &descriptor_timeout_invalid.descriptor
};

cce_condition_t const *
ccevents_condition_new_timeout_invalid (void)
{
  return (cce_condition_t *)&condition_timeout_invalid;
}

const char *
condition_timeout_invalid_static_message (cce_condition_t const * C CCEVENTS_UNUSED)
{
  return "timeout fields specification would cause invalid time representation";
}


/** --------------------------------------------------------------------
 ** Exceptional condition descriptor: timeout overflow.
 ** ----------------------------------------------------------------- */

static cce_condition_static_message_fun_t	condition_timeout_overflow_static_message;

static const ccevents_descriptor_timeout_overflow_t descriptor_timeout_overflow = {
  .descriptor.parent		= &descriptor_timeout.descriptor,
  .descriptor.delete		= NULL,
  .descriptor.final		= NULL,
  .descriptor.static_message	= condition_timeout_overflow_static_message
};

ccevents_descriptor_timeout_overflow_t const * const ccevents_descriptor_timeout_overflow = &descriptor_timeout_overflow;

/* This struct type has no dynamic fields, so there is only one instance
   of this struct type.  We allocate it statically. */
static const ccevents_condition_timeout_overflow_t condition_timeout_overflow = {
  .timeout.base.condition.descriptor	= &descriptor_timeout_overflow.descriptor
};

cce_condition_t const *
ccevents_condition_new_timeout_overflow (void)
{
  return (cce_condition_t *)&condition_timeout_overflow;
}

const char *
condition_timeout_overflow_static_message (cce_condition_t const * C CCEVENTS_UNUSED)
{
  return "timeout fields specification would cause overflow in time representation";
}


/** --------------------------------------------------------------------
 ** Module initialisation.
 ** ----------------------------------------------------------------- */

void
ccevents_conditions_module_initialisation (void)
{
  cce_descriptor_set_root_parent(&descriptor_base.descriptor);
}

/* end of file */
