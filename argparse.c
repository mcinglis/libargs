
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


#include "argparse.h"

#include <errno.h>

#include <libmacro/assert.h>
#include <libmacro/debug.h>
#include <libstr/str.h>
#include <libarray/array-str.h>


void
arg_parse_str( char const * const _,
               char const * const arg,
               void * const vdest )
{
    ASSERT( arg != NULL );

    if ( vdest == NULL ) { return; }
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
bool
str_one_of(
        char const * const xs,
        char const * const * const yss )
{
    ASSERT( xs != NULL, yss != NULL );

    for ( size_t i = 0; yss[ i ] != NULL; i++ ) {
        if ( str__equal( xs, yss[ i ] ) ) {
            return true;
        }
    }
    return false;
}


static
ArgFlag const *
find_flag(
        ArrayC_ArgFlag const flags,
        char const * const arg )
{
    ASSERT( arg != NULL );

    for ( size_t i = 0; i < flags.length; i++ ) {
        ArgFlag const * const af = flags.e + i;
        if ( ( af->pattern != NULL && af->pattern( arg ) )
          || ( af->names != NULL && str_one_of( arg, af->names ) )
          || ( af->name != NULL && str__equal( af->name, arg ) ) ) {
            return af;
        }
    }
    return NULL;
}


static
ArgOption const *
find_option(
        ArrayC_ArgOption const options,
        char const * const arg )
{
    ASSERT( arg != NULL );

    for ( size_t i = 0; i < options.length; i++ ) {
        ArgOption const * const ao = options.e + i;
        if ( ( ao->pattern != NULL && ao->pattern( arg ) )
          || ( ao->names != NULL && str_one_of( arg, ao->names ) )
          || ( ao->name != NULL && str__equal( ao->name, arg ) ) ) {
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


void
argparse(
        int const argc,
        char const * const * const argv,
        ArgsError * const err,
        ArgsSpec const spec )
{
    ASSERT( argc >= 1, argv != NULL, err != NULL );

    ArrayC_str const args = { .e = argv + 1, .length = argc - 1 };
    argparse_array( args, err, spec );
}


void
argparse_array(
        ArrayC_str const args,
        ArgsError * const err,
        ArgsSpec const spec )
{
    ASSERT( arrayc_str__is_valid( args ), err != NULL );

    *err = ( ArgsError ){ .type = ArgsError_NONE };
    size_t num_positionals = 0;
    // Positional parsing state:
    ArgPositional const * positional = NULL;
    size_t positional_arg_count = 0;
    bool preserve_positional = false;
    // Option parsing state:
    ArgOption const * option = NULL;
    size_t option_arg_count = 0;
    char const * option_name = NULL;
    bool preserve_option = false;
    // Argument parsing loop:
    for ( size_t i = 0; i < args.length; i++ ) {
        char const * const arg = args.e[ i ];
        // Reset positional state if we aren't parsing positional args:
        if ( !preserve_positional ) {
            if ( positional != NULL ) {
                if ( under_min( positional->num_args,
                                positional_arg_count ) ) {
                    *err = ( ArgsError ){ .type = ArgsError_MISSING_ARG,
                                          .str  = positional->name };
                    return;
                }
                num_positionals++;
            }
            positional = NULL;
            positional_arg_count = 0;
        }
        preserve_positional = false;
        // Reset option state if we aren't parsing option parameters:
        if ( !preserve_option ) {
            if ( option != NULL
              && under_min( option->num_args, option_arg_count ) ) {
                *err = ( ArgsError ){ .type = ArgsError_MISSING_OPTION_ARG,
                                      .str  = option_name };
                return;
            }
            option = NULL;
            option_arg_count = 0;
            option_name = NULL;
        }
        preserve_option = false;
        // If our argument matches a flag name:
        ArgFlag const * const flag = find_flag( spec.flags, arg );
        if ( flag != NULL ) {
            ( flag->parser ? flag->parser : arg_set_true )
                ( arg, NULL, flag->destination );
            if ( errno ) {
                *err = ( ArgsError ){ .type  = ArgsError_PARSE_ARG,
                                      .error = errno,
                                      .str   = arg };
                return;
            }
            if ( flag->stop ) { return; }
            continue;
        }
        // Or, if our argument matches an option name:
        ArgOption const * const new_option = find_option( spec.options, arg );
        if ( new_option != NULL ) {
            if ( option != NULL
              && under_min( option->num_args, option_arg_count ) ) {
                *err = ( ArgsError ){ .type = ArgsError_MISSING_OPTION_ARG,
                                      .str  = option_name };
                return;
            }
            option = new_option;
            option_name = arg;
            option_arg_count = 0;
            preserve_option = true;
            continue;
        }
        // Or, if we're parsing option arguments:
        if ( option != NULL ) {
            ( option->parser ? option->parser : arg_parse_str )
                ( option_name, arg, option->destination );
            if ( errno ) {
                *err = ( ArgsError ){ .type  = ArgsError_PARSE_ARG,
                                      .error = errno,
                                      .str   = arg };
            }
            option_arg_count++;
            preserve_option = !over_or_eq_max( option->num_args,
                                               option_arg_count );
            continue;
        }
        // Or, if we have outstanding positional arguments:
        if ( num_positionals < spec.positionals.length ) {
            positional = spec.positionals.e + num_positionals;
            ( positional->parser ? positional->parser : arg_parse_str )
                ( positional->name, arg, positional->destination );
            if ( errno ) {
                *err = ( ArgsError ){ .type  = ArgsError_PARSE_ARG,
                                      .error = errno,
                                      .str   = arg };
                return;
            }
            positional_arg_count++;
            preserve_positional = !over_or_eq_max( positional->num_args,
                                                   positional_arg_count );
            continue;
        }
        // Otherwise, the argument did not match a specified long/short
        // flag/option, and we're not parsing option parameters, and there
        // are no outstanding positional arguments, so it's invalid:
        *err = ( ArgsError ){ .type = ArgsError_UNKNOWN_ARG,
                              .str  = arg };
        return;
    }
    // If we exited the loop on parsing an option, then we need to check that
    // we parsed enough parameters for that option:
    if ( option != NULL
      && under_min( option->num_args, option_arg_count ) ) {
        *err = ( ArgsError ){ .type = ArgsError_MISSING_OPTION_ARG,
                              .str  = option_name };
        return;
    }
    // Or, if we exited the loop on parsing a positional, then we won't have
    // been able to increment `num_positionals` as appropriate:
    if ( positional != NULL ) {
        if ( under_min( positional->num_args, positional_arg_count ) ) {
            *err = ( ArgsError ){ .type = ArgsError_MISSING_ARG,
                                  .str  = positional->name };
            return;
        }
        num_positionals++;
    }
    // If we parsed fewer than the specified number of positionals, error:
    if ( num_positionals < spec.positionals.length ) {
        *err = ( ArgsError ){ .type = ArgsError_MISSING_ARG,
                              .str  = spec.positionals
                                          .e[ num_positionals ].name };
        return;
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

