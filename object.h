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


// Typedefs and functions for objects
#ifndef CPARENTHESES_INCLUDE_OBJECT
#define CPARENTHESES_INCLUDE_OBJECT 1


struct cprnths_obj_t;
typedef struct cprnths_obj_t cprnths_obj_t;


#include "class.h"
// cprnths_class_t

#include "copy_table.h"
// cprnths_copytab_t


// The "base class" for "everything else".
struct cprnths_obj_t {
    // The object's type (not NULL)
    cprnths_class_t const * cls;
};


// Destroy an object.
void
cprnths_obj_destroy(
    cprnths_obj_t*
    // not NULL
);

// Copy an object deeply and return the copy.
cprnths_obj_t*
// the copy or NULL
cprnths_obj_copy(
    cprnths_obj_t const *,
    // the original (not NULL)
    cprnths_copytab_t*
    // the copy table to use (not NULL)
);


#endif
