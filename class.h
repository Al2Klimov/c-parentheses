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


// Typedefs for classes
#ifndef CPARENTHESES_INCLUDE_CLASS
#define CPARENTHESES_INCLUDE_CLASS 1


struct cprnths_class_t;


#include <stddef.h>
// size_t

#include <stdbool.h>
// bool

#include "object.h"
// cprnths_obj_t

#include "copy_table.h"
// cprnths_copytab_t


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
    bool
    // was the copying successful?
    (*obj_copy)(
        // original (not NULL)
        struct cprnths_obj_t const *,
        // copy (target, not NULL)
        struct cprnths_obj_t*,
        // copy table to use (not NULL)
        struct cprnths_copytab_t*
    );
};


#endif
