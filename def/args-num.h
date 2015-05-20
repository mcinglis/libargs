
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


#ifndef LIBARGS_DEF_ARGSNUM_H
#define LIBARGS_DEF_ARGSNUM_H


typedef struct argsnum {
    int min;
    int max;
} ArgsNum;

// Special values of the `ArgsNum` fields:
enum {
    ArgsNum_NONE     = -1,
    ArgsNum_INFINITE = -2
};

// `0` in the fields of an `ArgsNum` struct is taken to be `1`; use
// `ArgsNum_NONE` to represent `0`. This allows for users to omit the
// specification of the `num_args` fields in the other structs, and have it
// omit reasonable behavior.


#endif // ifndef LIBARGS_DEF_ARGSNUM_H


