/* Copyright 2018  Endless Mobile, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <glib.h>
#include "cog/cog-version.h"

#ifndef _COG_EXTERN
#define _COG_EXTERN extern
#endif

#ifdef COG_DISABLE_DEPRECATION_WARNINGS
#define COG_DEPRECATED _COG_EXTERN
#define COG_DEPRECATED_FOR(f) _COG_EXTERN
#define COG_UNAVAILABLE(maj,min) _COG_EXTERN
#else
#define COG_DEPRECATED G_DEPRECATED _COG_EXTERN
#define COG_DEPRECATED_FOR(f) G_DEPRECATED_FOR(f) _COG_EXTERN
#define COG_UNAVAILABLE(maj,min) G_UNAVAILABLE(maj,min) _COG_EXTERN
#endif

/* Each new cycle should add a new version symbol here */

/**
 * COG_VERSION_0_0:
 *
 * A pre-processor macro that evaluates to the 0.0 version of Libcog, in a
 * format that can be used by %COG_VERSION_MIN_REQUIRED and
 * %COG_VERSION_MAX_ALLOWED.
 */
#define COG_VERSION_0_0 (G_ENCODE_VERSION (0, 0))

/**
 * COG_VERSION_CUR_STABLE:
 *
 * Evaluates to the current stable version.
 *
 * For development cycles, this means the next stable cycle.
 */
#if (COG_MINOR_VERSION == 99)
#define COG_VERSION_CUR_STABLE (G_ENCODE_VERSION (COG_MAJOR_VERSION + 1, 0))
#elif (COG_MINOR_VERSION % 2)
#define COG_VERSION_CUR_STABLE (G_ENCODE_VERSION (COG_MAJOR_VERSION, COG_MINOR_VERSION + 1))
#else
#define COG_VERSION_CUR_STABLE (G_ENCODE_VERSION (COG_MAJOR_VERSION, COG_MINOR_VERSION))
#endif

/**
 * COG_VERSION_PREV_STABLE:
 *
 * Evaluates to the previous stable version.
 */
#if (COG_MINOR_VERSION == 99)
#define COG_VERSION_PREV_STABLE (G_ENCODE_VERSION (COG_MAJOR_VERSION + 1, 0))
#elif (COG_MINOR_VERSION % 2)
#define COG_VERSION_PREV_STABLE (G_ENCODE_VERSION (COG_MAJOR_VERSION, COG_MINOR_VERSION - 1))
#else
#define COG_VERSION_PREV_STABLE (G_ENCODE_VERSION (COG_MAJOR_VERSION, COG_MINOR_VERSION - 2))
#endif

/**
 * COG_VERSION_MIN_REQUIRED:
 *
 * A pre-processor symbol that should be defined by the user prior to including
 * the `dmodel.h` header.
 *
 * The value should be one of the predefined Libcog version macros,
 * for instance: %COG_VERSION_0_0, `COG_VERSION_0_2`, ...
 *
 * This symbol defines the earliest version of Libcog that a project
 * is required to be able to compile against.
 *
 * If the compiler is configured to warn about the use of deprecated symbols,
 * then using symbols that were deprecated in version %COG_VERSION_MIN_REQUIRED,
 * or earlier versions, will emit a deprecation warning, but using functions
 * deprecated in later versions will not.
 *
 * See also: %COG_VERSION_MAX_ALLOWED
 */
#ifndef COG_VERSION_MIN_REQUIRED
# define COG_VERSION_MIN_REQUIRED (COG_VERSION_CUR_STABLE)
#endif

/**
 * COG_VERSION_MAX_ALLOWED:
 *
 * A pre-processor symbol that should be defined by the user prior to including
 * the `dmodel.h` header.
 *
 * The value should be one of the predefined Libcog version macros,
 * for instance: %COG_VERSION_0_0, `COG_VERSION_0_2`, ...
 *
 * This symbol defines the earliest version of Libcog that a project
 * is required to be able to compile against.
 *
 * If the compiler is configured to warn about the use of deprecated symbols,
 * then using symbols that were deprecated in version %COG_VERSION_MAX_ALLOWED,
 * or later versions, will emit a deprecation warning, but using functions
 * deprecated in later versions will not.
 *
 * See also: %COG_VERSION_MIN_REQUIRED
 */
#ifndef COG_VERSION_MAX_ALLOWED
# if COG_VERSION_MIN_REQUIRED > COG_VERSION_PREV_STABLE
#  define COG_VERSION_MAX_ALLOWED (COG_VERSION_MIN_REQUIRED)
# else
#  define COG_VERSION_MAX_ALLOWED (COG_VERSION_CUR_STABLE)
# endif
#endif

/* sanity checks */
#if COG_VERSION_MAX_ALLOWED < COG_VERSION_MIN_REQUIRED
#error "COG_VERSION_MAX_ALLOWED must be >= COG_VERSION_MIN_REQUIRED"
#endif
#if COG_VERSION_MIN_REQUIRED < COG_VERSION_0_0
#error "COG_VERSION_MIN_REQUIRED must be >= COG_VERSION_0_0"
#endif

/* unconditional: we can't have an earlier API version */
#define COG_AVAILABLE_IN_ALL _COG_EXTERN
