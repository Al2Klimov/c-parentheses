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


// Typedefs and functions for objects
#ifndef CPARENTHESES_INCLUDE_OBJECT
#define CPARENTHESES_INCLUDE_OBJECT 1


#include <stddef.h>
// size_t

#include "copy_table.h"
// cprnths_copytab_t

#include "error.h"
// cprnths_error_t


struct cprnths_class_t;

// The "base class" for "everything else".
struct cprnths_obj_t {
    // The object's type (not NULL)
    struct cprnths_class_t const *const cls;
};

// This is for storing information about datatypes non-redundandly.
struct cprnths_class_t {
    // An object's size (in bytes, > 0)
    size_t obj_size;

    // Each of the following function pointers MAY be NULL if there's nothing to do.

    // Destroy the object
    void
    (*obj_destroy)(
        struct cprnths_obj_t*
        // the object to clean up (not NULL)
    );

    // Copy the object deeply
    cprnths_error_t
    // (see error.h)
    (*obj_copy)(
        struct cprnths_obj_t*,
        // copy (not NULL)
        struct cprnths_copytab_t*
        // copy table to use (not NULL)
    );
};


// Destroy an object.
void
cprnths_obj_destroy(
    struct cprnths_obj_t*
    // not NULL
);

// Copy an object deeply.
cprnths_error_t
// (see error.h)
cprnths_obj_copy(
    struct cprnths_obj_t const *,
    // the original (not NULL)
    struct cprnths_obj_t**,
    // the copy (not NULL)
    struct cprnths_copytab_t*
    // the copy table to use (not NULL)
);


#endif
