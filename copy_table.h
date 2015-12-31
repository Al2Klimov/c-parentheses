// C() / "C Parentheses"
// A pure functional programming language
// Reference implementation
//
// Copyright (C) 2015  Alexander A. Klimov
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


// Typedefs and functions for copy tables
#ifndef CPARENTHESES_INCLUDE_COPY_TABLE
#define CPARENTHESES_INCLUDE_COPY_TABLE 1


struct cprnths_copytab_t;


#include <stddef.h>
// size_t

#include "reference.h"
// cprnths_ref_t


// A row of a copy table
struct cprnths_copytab_row_t {
    // The original (not NULL)
    struct cprnths_ref_t const * orig;

    // The copy (not NULL)
    struct cprnths_ref_t* copy;
};

// This is for tracking which references have already been copied during deep copying
// not to copy a reference more than once. (You know, diamond references, ...)
struct cprnths_copytab_t {
    // The table itself (not NULL)
    struct cprnths_copytab_row_t* tab;

    // Amount of slots to allocate memory for at once
    // 1 slot == sizeof( struct cprnths_copytab_row_t )
    // SHALL be > 0!
    size_t const chunksize;

    // Amount of present slots
    // SHALL be > 0 and multiple of .chunksize!
    size_t slots_total;

    // Amount of free slots
    // SHALL be <= .slots_total!
    size_t slots_free;
};


// Create a new copy table.
struct cprnths_copytab_t*
// a new table or NULL
cprnths_copytab_create(
    size_t
    // .chunksize
);

// Add references to a copy table.
_Bool
// was the addition successful?
cprnths_copytab_addrefs(
    struct cprnths_copytab_t*,
    // not NULL
    struct cprnths_ref_t const *,
    // original (not NULL)
    struct cprnths_ref_t*
    // copy (not NULL)
);

// If an original reference is listed in a copy table, return its copy.
struct cprnths_ref_t*
// the copy or NULL
cprnths_copytab_chkref(
    struct cprnths_copytab_t const *,
    // not NULL
    struct cprnths_ref_t const *
    // not NULL
);

// Destroy a copy table.
void
cprnths_copytab_destroy(
    struct cprnths_copytab_t*
    // not NULL
);


#endif
