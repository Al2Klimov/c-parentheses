// C() / "C Parentheses"
// A pure functional programming language
// Reference implementation
//
// Copyright (C) 2015-2016  Alexander A. Klimov
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


// Typedefs and functions for garbage tables
#ifndef CPARENTHESES_INCLUDE_GARBAGE_TABLE
#define CPARENTHESES_INCLUDE_GARBAGE_TABLE 1


struct cprnths_garbtab_t;


#include <stddef.h>
// size_t

#include "reference.h"
// cprnths_ref_t

#include "error.h"
// cprnths_error_t


// This is for tracking how many (and which) references there are and being able
// to clean them up manually when they aren't needed anymore, but automatic cleanup
// was unsuccessful. (You know, circular references.)
struct cprnths_garbtab_t {
    // The table itself (not NULL)
    struct cprnths_ref_t** tab;

    // Amount of slots to allocate memory for at once
    // 1 slot == sizeof( struct cprnths_ref_t* )
    // SHALL be > 0!
    size_t const chunksize;

    // Amount of present slots
    // SHALL be > 0 and multiple of .chunksize!
    size_t slots_total;

    // Amount of free slots
    // SHALL be <= .total!
    size_t slots_free;
};


// Create a new garbage table.
cprnths_error_t
// (see error.h)
cprnths_garbtab_create(
    struct cprnths_garbtab_t**,
    // a new garbage table (not NULL)
    size_t
    // .chunksize
);

// Add a reference to a garbage table.
cprnths_error_t
// (see error.h)
cprnths_garbtab_addref(
    struct cprnths_garbtab_t*,
    // not NULL
    struct cprnths_ref_t*
    // not NULL
);

// Remove a reference from a garbage table.
void
cprnths_garbtab_delref(
    struct cprnths_garbtab_t*,
    // not NULL
    struct cprnths_ref_t*
    // not NULL
);

// Clean a garbage table's references up manually.
void
cprnths_garbtab_cleanup(
    struct cprnths_garbtab_t*
    // not NULL
);

// Destroy a garbage table without calling cprnths_garbtab_cleanup().
void
cprnths_garbtab_destroy(
    struct cprnths_garbtab_t*
    // not NULL
);


#endif
