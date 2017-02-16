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

static void             base_C_destructor     (cce_condition_t * C);
static const char *     base_C_static_message (const cce_condition_t * C);

static ccevents_base_D_t base_D = {
  /* This "parent" field is set by the initialisation function. */
  .parent		= NULL,
  .free			= base_C_destructor,
  .static_message	= base_C_static_message
};

const ccevents_base_D_t * ccevents_base_D = &base_D;

void
base_C_destructor (cce_condition_t * C CCEVENTS_UNUSED)
{
  return;
}
const char *
base_C_static_message (const cce_condition_t * C CCEVENTS_UNUSED)
{
  return "CCEvents exceptional condition";
}

/** --------------------------------------------------------------------
 ** Exceptional condition descriptor: timeval base condition.
 ** ----------------------------------------------------------------- */

static void		timeval_C_destructor     (cce_condition_t * C);
static const char *	timeval_C_static_message (const cce_condition_t * C);

static const ccevents_timeval_D_t timeval_D = {
  .parent		= &base_D,
  .free			= timeval_C_destructor,
  .static_message	= timeval_C_static_message
};

const ccevents_timeval_D_t * ccevents_timeval_D = &timeval_D;

/* This struct type has no dynamic fields, so there is only one instance
   of this struct type.  We allocate it statically. */
static const ccevents_timeval_C_t timeval_C = {
  .descriptor = &timeval_D
};

const ccevents_timeval_C_t *
ccevents_timeval_C (void)
{
  return &timeval_C;
}
void
timeval_C_destructor (cce_condition_t * C CCEVENTS_UNUSED)
{
  return;
}
const char *
timeval_C_static_message (const cce_condition_t * C CCEVENTS_UNUSED)
{
  return "timeval exception";
}

/** --------------------------------------------------------------------
 ** Exceptional condition descriptor: timeval invalid.
 ** ----------------------------------------------------------------- */

static void		timeval_invalid_C_destructor     (cce_condition_t * C);
static const char *	timeval_invalid_C_static_message (const cce_condition_t * C);

static const ccevents_timeval_invalid_D_t timeval_invalid_D = {
  .parent		= &timeval_D,
  .free			= timeval_invalid_C_destructor,
  .static_message	= timeval_invalid_C_static_message
};

const ccevents_timeval_invalid_D_t * ccevents_timeval_invalid_D = &timeval_invalid_D;

/* This struct type has no dynamic fields, so there is only one instance
   of this struct type.  We allocate it statically. */
static const ccevents_timeval_invalid_C_t timeval_invalid_C = {
  .descriptor = &timeval_invalid_D
};

const ccevents_timeval_invalid_C_t *
ccevents_timeval_invalid_C (void)
{
  return &timeval_invalid_C;
}
void
timeval_invalid_C_destructor (cce_condition_t * C CCEVENTS_UNUSED)
{
  return;
}
const char *
timeval_invalid_C_static_message (const cce_condition_t * C CCEVENTS_UNUSED)
{
  return "timeval fields specification are invalid";
}

/** --------------------------------------------------------------------
 ** Exceptional condition descriptor: timeval overflow.
 ** ----------------------------------------------------------------- */

static void		timeval_overflow_C_destructor     (cce_condition_t * C);
static const char *	timeval_overflow_C_static_message (const cce_condition_t * C);

static const ccevents_timeval_overflow_D_t timeval_overflow_D = {
  .parent		= &timeval_D,
  .free			= timeval_overflow_C_destructor,
  .static_message	= timeval_overflow_C_static_message
};

const ccevents_timeval_overflow_D_t * ccevents_timeval_overflow_D = &timeval_overflow_D;

/* This struct type has no dynamic fields, so there is only one instance
   of this struct type.  We allocate it statically. */
static const ccevents_timeval_overflow_C_t timeval_overflow_C = {
  .descriptor = &timeval_overflow_D
};

const ccevents_timeval_overflow_C_t *
ccevents_timeval_overflow_C (void)
{
  return &timeval_overflow_C;
}
void
timeval_overflow_C_destructor (cce_condition_t * C CCEVENTS_UNUSED)
{
  return;
}
const char *
timeval_overflow_C_static_message (const cce_condition_t * C CCEVENTS_UNUSED)
{
  return "timeval fields specification would cause overflow in time representation";
}

/** --------------------------------------------------------------------
 ** Exceptional condition descriptor: timeout invalid.
 ** ----------------------------------------------------------------- */

static void		timeout_invalid_C_destructor  (cce_condition_t * C);
static const char *	timeout_invalid_C_static_message (const cce_condition_t * C);

static ccevents_timeout_invalid_D_t timeout_invalid_D = {
  .parent		= &timeval_D,
  .free			= timeout_invalid_C_destructor,
  .static_message	= timeout_invalid_C_static_message
};

const ccevents_timeout_invalid_D_t * ccevents_timeout_invalid_D = &timeout_invalid_D;

/* There  are  no  dynamic  fields  in  this  condition  object,  so  we
   statically allocate it. */
static const ccevents_timeout_invalid_C_t timeout_invalid_C = {
  .descriptor = &timeout_invalid_D
};

const ccevents_timeout_invalid_C_t *
ccevents_timeout_invalid_C (void)
{
  return &timeout_invalid_C;
}
void
timeout_invalid_C_destructor (cce_condition_t * C CCEVENTS_UNUSED)
{
  return;
}
const char *
timeout_invalid_C_static_message (const cce_condition_t * C CCEVENTS_UNUSED)
{
  return "timeout fields specification would cause invalid time";
}

/** --------------------------------------------------------------------
 ** Exceptional condition descriptor: timeout overflow.
 ** ----------------------------------------------------------------- */

static void             timeout_overflow_C_destructor	(cce_condition_t * C);
static const char *     timeout_overflow_C_static_message (const cce_condition_t * C);

static ccevents_timeout_overflow_D_t timeout_overflow_D = {
  .parent		= &timeval_D,
  .free			= timeout_overflow_C_destructor,
  .static_message	= timeout_overflow_C_static_message
};

const ccevents_timeout_overflow_D_t * ccevents_timeout_overflow_D = &timeout_overflow_D;

/* There  are  no  dynamic  fields  in  this  condition  object,  so  we
   statically allocate it. */
static const ccevents_timeout_overflow_C_t condition_timeout_overflow = {
  .descriptor = &timeout_overflow_D
};

const ccevents_timeout_overflow_C_t *
ccevents_timeout_overflow_C (void)
{
  return &condition_timeout_overflow;
}
void
timeout_overflow_C_destructor (cce_condition_t * C CCEVENTS_UNUSED)
{
  return;
}
const char *
timeout_overflow_C_static_message (const cce_condition_t * C CCEVENTS_UNUSED)
{
  return "timeout fields specification would cause overflow in time representation";
}

/** --------------------------------------------------------------------
 ** Module initialisation.
 ** ----------------------------------------------------------------- */

void
ccevents_conditions_module_initialisation (void)
{
  base_D.parent = cce_root_D;
}

/* end of file */
