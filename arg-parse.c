
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


#include "arg-parse.h"

#include <errno.h>

#include <libmacro/assert.h>
#include <libstr/str.h>
#include <libarray/array-str.h>


void
arg_parse_str( char const * const _,
               char const * const arg,
               void * const vdest )
{
    ASSERT( arg != NULL, vdest != NULL );

    char const * * const dest = vdest;
    *dest = arg;
}


void
arg_set_false( char const * const _1,
               char const * const _2,
               void * const vdest )
{
    ASSERT( vdest != NULL );

    bool * const dest = vdest;
    *dest = false;
}


void
arg_set_true( char const * const _1,
              char const * const _2,
              void * const vdest )
{
    ASSERT( vdest != NULL );

    bool * const dest = vdest;
    *dest = true;
}


static
ArgFlag const *
find_flag_long_match( ArrayC_ArgFlag const flags,
                      char const * const arg )
{
    ASSERT( arg != NULL );

    for ( size_t i = 0; i < flags.length; i++ ) {
        ArgFlag const * const af = flags.e + i;
        if ( ( af->long_pattern != NULL && af->long_pattern( arg ) )
          || ( af->long_name != NULL && str__equal( af->long_name, arg ) ) ) {
            return af;
        }
    }
    return NULL;
}


static
ArgOption const *
find_option_long_match( ArrayC_ArgOption const options,
                        char const * const arg )
{
    ASSERT( arg != NULL );

    for ( size_t i = 0; i < options.length; i++ ) {
        ArgOption const * const ao = options.e + i;
        if ( ( ao->long_pattern != NULL && ao->long_pattern( arg ) )
          || ( ao->long_name != NULL && str__equal( ao->long_name, arg ) ) ) {
            return ao;
        }
    }
    return NULL;
}


static
ArgFlag const *
find_flag_short_match( ArrayC_ArgFlag const flags,
                       char const * const arg )
{
    ASSERT( arg != NULL );

    for ( size_t i = 0; i < flags.length; i++ ) {
        ArgFlag const * const af = flags.e + i;
        if ( af->short_name != NULL && str__equal( af->short_name, arg ) ) {
            return af;
        }
    }
    return NULL;
}


static
ArgOption const *
find_option_short_match( ArrayC_ArgOption const options,
                         char const * const arg )
{
    ASSERT( arg != NULL );

    for ( size_t i = 0; i < options.length; i++ ) {
        ArgOption const * const ao = options.e + i;
        if ( ao->short_name != NULL && str__equal( ao->short_name, arg ) ) {
            return ao;
        }
    }
    return NULL;
}


static
bool
over_or_eq_max( ArgsNum const argsnum,
                size_t const i )
{
    if ( argsnum.max == ArgsNum_NONE ) {
        return true;
    } else if ( argsnum.max == ArgsNum_INFINITE ) {
        return false;
    } else {
        ASSERT( argsnum.max >= 0 );
        uint const max = ( argsnum.max == 0 ) ? 1 : argsnum.max;
        return i >= max;
    }
}


static
bool
under_min( ArgsNum const argsnum,
           size_t const i )
{
    if ( argsnum.min == ArgsNum_NONE ) {
        return false;
    } else {
        ASSERT( argsnum.min >= 0 );
        uint const min = ( argsnum.min == 0 ) ? 1 : argsnum.min;
        return i < min;
    }
}


static
size_t
parse_option_args( ArgOption const opt,
                   char const * const arg_name,
                   ArrayC_str const args,
                   ArgsError * const err )
{
    ASSERT( arrayc_str__is_valid( args ), err != NULL );

    errno = 0;
    size_t i = 0;
    for ( ; i < args.length; i++ ) {
        if ( over_or_eq_max( opt.num_args, i ) ) {
            break;
        }
        char const * const arg = args.e[ i ];
        if ( str__is_prefix( arg, "-" ) ) {
            break;
        }
        ( opt.parser ? opt.parser : arg_parse_str )
            ( arg_name, arg, opt.destination );
        if ( errno ) {
            *err = ( ArgsError ){ .type  = ArgsError_PARSE_ARG,
                                  .error = errno,
                                  .str   = arg };
            return i;
        }
    }
    if ( under_min( opt.num_args, i ) ) {
        *err = ( ArgsError ){ .type = ArgsError_MISSING_OPTION_ARG,
                              .str  = arg_name };
    }
    return i;
}


void
arg_parse( int const argc,
           char const * const * const argv,
           ArgsError * const err,
           ArgSpec const spec )
{
    ASSERT( argc >= 1, argv != NULL, err != NULL );

    ArrayC_str const args = { .e = argv + 1, .length = argc - 1 };
    arg_parse_array( args, err, spec );
}


void
arg_parse_array( ArrayC_str const args,
                 ArgsError * const err,
                 ArgSpec const spec )
{
    ASSERT( arrayc_str__is_valid( args ), err != NULL );

    *err = ( ArgsError ){ .type = ArgsError_NONE };
    size_t npos = 0;
    for ( size_t i = 0; i < args.length; i++ ) {
        char const * const arg = args.e[ i ];
        // If we're on a long option name:
        if ( str__is_prefix( arg, "--" ) ) {
            char const * const arg_name = arg + 2;
            // Check if any flags match this arg:
            ArgFlag const * const af =
                    find_flag_long_match( spec.flags, arg_name );
            if ( af != NULL ) {
                ( af->parser ? af->parser : arg_set_true )
                    ( arg_name, NULL, af->destination );
                if ( errno ) {
                    *err = ( ArgsError ){ .type  = ArgsError_PARSE_ARG,
                                          .error = errno,
                                          .str   = arg };
                    return;
                }
                if ( af->stop ) { return; }
                continue;
            }
            // Check if any options match this arg:
            ArgOption const * const ao =
                    find_option_long_match( spec.options, arg_name );
            if ( ao != NULL ) {
                ArrayC_str const rest = arrayc_str__drop( args, i + 1 );
                i += parse_option_args( *ao, arg_name, rest, err );
                if ( err->type != ArgsError_NONE ) { return; }
                if ( ao->stop ) { return; }
                continue;
            }
        // Or, if we're on a short option argument:
        } else if ( str__is_prefix( arg, "-" ) ) {
            char const * const arg_name = arg + 1;
            // Check if any flags match this arg:
            ArgFlag const * const af =
                    find_flag_short_match( spec.flags, arg_name );
            if ( af != NULL ) {
                ( af->parser ? af->parser : arg_set_true )
                    ( arg_name, NULL, af->destination );
                if ( errno ) {
                    *err = ( ArgsError ){ .type  = ArgsError_PARSE_ARG,
                                          .error = errno,
                                          .str   = arg };
                    return;
                }
                if ( af->stop ) { return; }
                continue;
            }
            // Check if any options match this arg:
            ArgOption const * const ao =
                    find_option_short_match( spec.options, arg_name );
            if ( ao != NULL ) {
                ArrayC_str const rest = arrayc_str__drop( args, i + 1 );
                i += parse_option_args( *ao, arg_name, rest, err );
                if ( err->type != ArgsError_NONE ) { return; }
                if ( ao->stop ) { return; }
                continue;
            }
        // Or, if we have outstanding positional arguments:
        } else if ( npos < spec.positionals.length ) {
            ArgPositional const ap = spec.positionals.e[ npos ];
            ( ap.parser ? ap.parser : arg_parse_str )
                ( ap.name, arg, ap.destination );
            if ( errno ) {
                *err = ( ArgsError ){ .type  = ArgsError_PARSE_ARG,
                                      .error = errno,
                                      .str   = arg };
                return;
            }
            npos++;
            continue;
        }
        // If we're here, we have no outstanding positional arguments, and
        // the argument did not start with "-" or "--", so it's an error:
        *err = ( ArgsError ){ .type = ArgsError_UNKNOWN_ARG,
                              .str  = arg };
        return;
    }
    if ( npos < spec.positionals.length ) {
        *err = ( ArgsError ){ .type = ArgsError_MISSING_ARG,
                              .str  = spec.positionals.e[ npos ].name };
    }
}


char const *
argserrortype__to_str( enum ArgsErrorType const t )
{
    switch ( t ) {
        case ArgsError_NONE:               return "none";
        case ArgsError_ERROR:              return "error";
        case ArgsError_MISSING_ARG:        return "missing argument";
        case ArgsError_PARSE_ARG:          return "parse argument";
        case ArgsError_UNKNOWN_ARG:        return "unknown argument";
        case ArgsError_MISSING_OPTION_ARG: return "missing option argument";
        case ArgsError_INCONSISTENT_ARG:   return "inconsistent argument";
        case ArgsError_SYSTEM:             return "system error";
        default:                           return "unknown";
    }
}

