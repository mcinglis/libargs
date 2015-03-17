
// Copyright 2015  Malcolm Inglis <http://minglis.id.au>
//
// This file is part of Libargs.
//
// Libargs is free software: you can redistribute it and/or modify it under
// the terms of the GNU Affero General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// Libargs is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
// more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with Libargs. If not, see <https://gnu.org/licenses/>.


#ifndef LIBARGS_ARG_PARSE_H
#define LIBARGS_ARG_PARSE_H


#include <libarray/def/array-str.h>

#include "types.h"


void
arg_parse_str( char const * const _,
               char const * const arg,
               void * const vdest );


void
arg_set_false( char const * const _1,
               char const * const _2,
               void * const vdest );


void
arg_set_true( char const * const _1,
              char const * const _2,
              void * const vdest );


void
arg_parse( int argc,
           char const * const * argv,
           ArgsError * err,
           ArgSpec spec );


void
arg_parse_array( ArrayC_str args,
                 ArgsError * err,
                 ArgSpec spec );


char const *
argserrortype__to_str( enum ArgsErrorType );


#endif

