
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


#ifndef LIBARGS_ARGS_DEF_H
#define LIBARGS_ARGS_DEF_H


#include <libtypes/types.h>
#include <libmacro/nelem.h>


enum ArgsErrorType {
    ArgsError_NONE,
    ArgsError_ERROR,
    ArgsError_MISSING_ARG,
    ArgsError_PARSE_ARG,
    ArgsError_UNKNOWN_ARG,
    ArgsError_MISSING_OPTION_ARG,
    ArgsError_INCONSISTENT_ARG,
    ArgsError_SYSTEM
};


typedef struct argserror {
    enum ArgsErrorType type;
    int error;
    char const * str;
    char * strm;
} ArgsError;


typedef void ( * arg_parser_fn )( char const * name,
                                  char const * arg,
                                  void * destination );


typedef struct argpositional {
    char const * name;
    void * destination;
    arg_parser_fn parser;
} ArgPositional;


typedef struct argflag {
    char const * short_name;
    char const * long_name;
    bool ( * long_pattern )( char const * name );
    void * destination;
    arg_parser_fn parser;
    bool stop;
} ArgFlag;


typedef struct argsnum {
    size_t min;
    size_t max;
} ArgsNum;


typedef struct argoption {
    ArgsNum num_args;
    char const * short_name;
    char const * long_name;
    bool ( * long_pattern )( char const * name );
    void * destination;
    arg_parser_fn parser;
    bool stop;
} ArgOption;


typedef struct arrayc_argflag {
    ArgFlag const * e;
    size_t length;
} ArrayC_ArgFlag;


typedef struct arrayc_argpositional {
    ArgPositional const * e;
    size_t length;
} ArrayC_ArgPositional;


typedef struct arrayc_argoption {
    ArgOption const * e;
    size_t length;
} ArrayC_ArgOption;


// @public
#define ARRAY_ARGPOSITIONAL( ... ) \
    { .e      = ( ArgPositional[] ){ __VA_ARGS__ }, \
      .length = NELEM( ( ArgPositional[] ){ __VA_ARGS__ } ) }


// @public
#define ARRAY_ARGFLAG( ... ) \
    { .e      = ( ArgFlag[] ){ __VA_ARGS__ }, \
      .length = NELEM( ( ArgFlag[] ){ __VA_ARGS__ } ) }


// @public
#define ARRAY_ARGOPTION( ... ) \
    { .e      = ( ArgOption[] ){ __VA_ARGS__ }, \
      .length = NELEM( ( ArgOption[] ){ __VA_ARGS__ } ) }


typedef struct argspec {
    ArrayC_ArgPositional positionals;
    ArrayC_ArgFlag flags;
    ArrayC_ArgOption options;
} ArgSpec;


#endif // ifndef LIBARGS_ARGS_DEF_H

