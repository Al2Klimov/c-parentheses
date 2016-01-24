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


// Typedefs and functions for references
#ifndef CPARENTHESES_INCLUDE_REFERENCE
#define CPARENTHESES_INCLUDE_REFERENCE 1


struct cprnths_ref_t;


#include <stddef.h>
// size_t

#include "object.h"
// cprnths_obj_t

#include "garbage_table.h"
// cprnths_garbtab_t

#include "copy_table.h"
// cprnths_copytab_t

#include "error.h"
// cprnths_error_t


// This is for referencing an object and cleaning it up automatically
// when it's not being referenced anymore. (You know, garbage collection.)
struct cprnths_ref_t {
    // The object being referenced (or NULL, indicating invalid reference)
    struct cprnths_obj_t* obj;

    // The amount of pointers to this reference
    size_t pcnt;

    // The garbage table this reference is listed in or NULL
    struct cprnths_garbtab_t* garbtab;
};


// Create a new reference to an object.
cprnths_error_t
// (see error.h)
cprnths_ref_create(
    struct cprnths_ref_t**,
    // a new reference (not NULL)
    struct cprnths_obj_t*,
    // NULL if indicating invalid reference
    struct cprnths_garbtab_t*
    // the garbage table to list the reference in or NULL
);

// Increment a reference's ->pcnt by a number.
// If the ->pcnt becomes 0, clean the reference up and remove it from
// its garbage table (if any).
void
cprnths_ref_increment(
    struct cprnths_ref_t*,
    // not NULL
    size_t
    // MAY be negative for decrementing
);

// Copy a reference deeply.
cprnths_error_t
// (see error.h)
cprnths_ref_copy(
    struct cprnths_ref_t const *,
    // the original (not NULL)
    struct cprnths_ref_t**,
    // the copy (not NULL)
    struct cprnths_copytab_t*
    // the copy table to use (not NULL)
);

// Create a new copy table and use it
// to copy a reference with cprnths_ref_copy().
cprnths_error_t
// (see error.h)
cprnths_ref_copy_newtab(
    struct cprnths_ref_t const *,
    // the original (not NULL)
    struct cprnths_ref_t**,
    // the copy (not NULL)
    size_t
    // .chunksize
);

// Get the object's truth value
_Bool
cprnths_ref_obj2bool(
    struct cprnths_ref_t const *
    // MAY be NULL
);


#endif
