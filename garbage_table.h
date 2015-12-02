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


// Typedefs and functions for garbage tables
#ifndef CPARENTHESES_INCLUDE_GARBAGE_TABLE
#define CPARENTHESES_INCLUDE_GARBAGE_TABLE 1


struct cprnths_garbtab_t;
typedef struct cprnths_garbtab_t cprnths_garbtab_t;


#include <stddef.h>
// size_t

#include <stdbool.h>
// bool

#include "reference.h"
// cprnths_ref_t


// This is for tracking how many (and which) references there are and being able
// to clean them up manually when they aren't needed anymore, but automatic cleanup
// was unsuccessful. (You know, circular references.)
struct cprnths_garbtab_t {
    // The table itself
    cprnths_ref_t** tab;

    // Amount of slots to allocate memory for at once
    // 1 slot == sizeof( cprnths_ref_t* )
    // SHALL be > 0!
    size_t const chunksize;

    // Amount of present slots
    // SHALL be > 0 and multiple of .chunksize!
    size_t slots_total;

    // Amount of free slots
    // SHALL be <= .total!
    size_t slots_free;
};


// Create a new garbage table with given ->chunksize and return a pointer to it.
// Return NULL if something went wrong.
cprnths_garbtab_t*
cprnths_garbtab_create(
    size_t
);

// Add the given reference to the given garbage table.
// Return true if (and only if) the reference was added successfully.
bool
cprnths_garbtab_addref(
    cprnths_garbtab_t*,
    cprnths_ref_t*
);

// Remove the given reference from the given garbage table.
void
cprnths_garbtab_delref(
    cprnths_garbtab_t*,
    cprnths_ref_t*
);

// Clean the given garbage table's references up manually.
void
cprnths_garbtab_cleanup(
    cprnths_garbtab_t*
);

// Destruct and free() the given garbage table
// without calling cprnths_garbtab_cleanup().
void
cprnths_garbtab_destruct(
    cprnths_garbtab_t*
);


#endif
