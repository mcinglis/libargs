
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


#ifndef LIBARGS_DEF_ARGOPTION_H
#define LIBARGS_DEF_ARGOPTION_H


#include <libtypes/types.h>

#include "argsnum.h"


typedef struct argoption {
    ArgsNum num_args;
    bool ( * pattern )( char const * name );
    char const * const * names;
    char const * name;
    void * destination;
    void ( * parser )( char const * name,
                       char const * arg,
                       void * destination );
    bool stop;
} ArgOption;


#endif // ifndef LIBARGS_DEF_ARGOPTION_H

