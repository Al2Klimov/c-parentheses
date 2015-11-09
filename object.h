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


// Typedefs and functions for objects
#ifndef CPARENTHESES_INCLUDE_OBJECT
#define CPARENTHESES_INCLUDE_OBJECT


struct cpintern_obj_t;
typedef struct cpintern_obj_t cprnths_obj_t;


#include "class.h"
// cprnths_class_t

#include "copy_table.h"
// cprnths_copytab_t


// A lot of datastructures are based on this.
// (You know, the "base class" for everything else.)
struct cpintern_obj_t {
    // The object's type (MUST NOT be NULL)
    cprnths_class_t const * const cls;
};


/* About the following functions:
 *
 * The pointer arguments MUST NOT be NULL.
 *
 * If a function fails, it will try to fail as gracefully as possible.
 * Things like program termination, memory leaks and printings to stderr
 * won't happen.
 */

// Destruct and free() the given object.
void cprnths_obj_destruct(cprnths_obj_t*);

// Make a deep copy of the given object and return a pointer to it.
// Return NULL if something went wrong.
cprnths_obj_t* cprnths_obj_copy(cprnths_obj_t const *, cprnths_copytab_t*);


#endif
