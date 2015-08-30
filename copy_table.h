/* C() / "C Parentheses"
 * A pure functional programming language
 * Reference implementation
 *
 * Copyright (C) 2015  Alexander A. Klimov
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


// Typedefs and functions for copy tables
#ifndef CPARENTHESES_INCLUDE_COPY_TABLE
#define CPARENTHESES_INCLUDE_COPY_TABLE


struct cpintern_copytab_t;
typedef struct cpintern_copytab_t cprnths_copytab_t;


#include <stddef.h>
// size_t

#include <stdbool.h>
// bool

#include "reference.h"
// cprnths_ref_t


// A row of a copy table (just for easy reusing)
typedef struct {
    // The original
    cprnths_ref_t* orig;

    // The copy
    cprnths_ref_t* copy;
} cprnths_copytab_row_t;

// This is for tracking which references have already been copied during deep copying
// not to copy a reference more than once. (You know, diamond references, ...)
struct cpintern_copytab_t {
    // The table itself
    cprnths_copytab_row_t* tab;

    // Amount of slots to allocate memory for at once
    // 1 slot == sizeof( cprnths_copytab_row_t )
    // SHALL be > 0!
    size_t const chunksize;

    // Amount of present slots
    // SHALL be > 0 and multiple of .chunksize!
    size_t slots_total;

    // Amount of free slots
    // SHALL be <= .slots_total!
    size_t slots_free;
};


// Create a new copy table with given .chunksize and return a pointer to it.
// Return NULL if something went wrong.
cprnths_copytab_t* cprnths_copytab_create(size_t);

// Add the given references (original and copy) to the given copy table.
// Return true if (and only if) the references were added successfully.
bool cprnths_copytab_addrefs(cprnths_copytab_t*, cprnths_ref_t*, cprnths_ref_t*);

// If the given (original) reference is listed in the given copy table,
// return a pointer to its copy. Otherwise, return NULL.
cprnths_ref_t* cprnths_copytab_chkref(cprnths_copytab_t*, cprnths_ref_t*);

// Destruct and free() the given copy table.
void cprnths_copytab_destruct(cprnths_copytab_t*);


#endif
