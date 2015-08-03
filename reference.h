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


// Typedefs and functions for references
#ifndef CPARENTHESES_INCLUDE_REFERENCE
#define CPARENTHESES_INCLUDE_REFERENCE


struct cpintern_ref_t;
typedef struct cpintern_ref_t cprnths_ref_t;


#include <stddef.h>
// size_t

#include "object.h"
// cprnths_obj_t

#include "garbage_table.h"
// cprnths_garbtab_t


// This is for referencing an object and cleaning it up automatically
// when it's not being referenced anymore. (You know, garbage collection.)
struct cpintern_ref_t {
    // The object being referenced (MAY be NULL, indicating invalid reference)
    cprnths_obj_t* obj;

    // The amount of pointers to this reference
    size_t pcnt;

    // The garbage table this reference is listed in (MAY be NULL -- no garbage table)
    cprnths_garbtab_t* garbtab;
};


// Create a new reference to the given object (MAY be NULL for creating
// an invalid reference) and return a pointer to the reference.
// The reference SHALL be listed in the given garbage table (if not NULL).
// Return NULL if something went wrong.
cprnths_ref_t* cprnths_ref_create(cprnths_obj_t*, cprnths_garbtab_t*);

// Increment the given reference's ->pcnt by the 2nd argument
// (which MAY be negative for decrementing).
// If the ->pcnt becomes 0, clean the reference up and remove it from
// its garbage table (if any).
void cprnths_ref_increment(cprnths_ref_t*, size_t);


#endif
