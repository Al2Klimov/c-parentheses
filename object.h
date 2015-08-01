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


#include <stddef.h>
// size_t

#include <stdbool.h>
// bool


// Type for storing the (exact) type of a cprnths_obj_t instance
typedef unsigned short int cprnths_objtype_t;

// A lot of datastructures are based on this.
typedef struct {
    // The object's (exact) type
    cprnths_objtype_t obj_type;

    // The object's size (in bytes, MUST be > 0)
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
    void (*obj_destruct)(void*);

    // Some objects REQUIRE some actions for deep copying.
    // This function SHALL return true if (and only if) the object was copied successfully
    // from the 1st argument to the 2nd one.
    bool (*obj_copy)(void*, void*);
} cprnths_obj_t;


/* About the following functions:
 *
 * The pointer arguments MUST NOT be NULL.
 *
 * If a function fails, it will try to fail as gracefully as possible.
 * Things like program termination, memory leaks and printings to stderr
 * won't happen.
 */

// Destruct and free() an object
void cprnths_obj_destruct(cprnths_obj_t*);

// Returns the copy of an object or NULL if the copying failed.
cprnths_obj_t* cprnths_obj_copy(cprnths_obj_t*);


#endif
