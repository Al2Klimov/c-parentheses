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


// Typedefs for classes
#ifndef CPARENTHESES_INCLUDE_CLASS
#define CPARENTHESES_INCLUDE_CLASS


struct cpintern_class_t;
typedef struct cpintern_class_t cprnths_class_t;


#include <stddef.h>
// size_t

#include <stdbool.h>
// bool

#include "object.h"
// cprnths_obj_t

#include "copy_table.h"
// cprnths_copytab_t


// Types of some members of cprnths_class_t (just for easy reusing)
typedef
void
(*cprnths_obj_destruct_t)(
    cprnths_obj_t*
);

typedef
bool
(*cprnths_obj_copy_t)(
    cprnths_obj_t const *,
    cprnths_obj_t*,
    cprnths_copytab_t*
);

// This is for storing information about datatypes non-redundandly.
// (You know, a lot of objects of a type, almost out of memory ...)
struct cpintern_class_t {
    // An object's size (in bytes, MUST be > 0)
    size_t obj_size;


    /* About the following "virtual methods":
     *
     * Each function pointer MAY be NULL when there's nothing to do.
     * This is absolutely equivalent to (but a little bit faster than)
     * a function which does nothing at all.
     *
     * The pointer arguments MUST NOT be NULL.
     *
     * If a function MAY fail, it SHOULD fail as gracefully as possible.
     * Things like program termination, memory leaks and printings to stderr
     * MUST NOT happen!
     */

    // Some objects REQUIRE some cleanup actions at the end of their lifetime.
    // This function SHALL be called with the object's address as the only argument.
    cprnths_obj_destruct_t obj_destruct;

    // Some objects REQUIRE some actions for deep copying.
    // This function SHALL return true if (and only if) the object was copied successfully
    // from the 1st argument to the 2nd one.
    cprnths_obj_copy_t obj_copy;
};


#endif
