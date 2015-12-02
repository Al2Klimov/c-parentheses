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


// Typedefs and functions for strings
#ifndef CPARENTHESES_INCLUDE_STRING
#define CPARENTHESES_INCLUDE_STRING 1


#include <stdint.h>
// uintmax_t

#include <stddef.h>
// size_t

#include <stdbool.h>
// bool


// The type of a string's hash (just for easy reusing)
typedef uintmax_t cprnths_string_hash_t;

// A string.
// (You know, a sequence of characters.)
typedef struct {
    // The string itself (MUST NOT be NULL)
    char const * str;

    // The string's length
    size_t length;

    // The string's hash
    cprnths_string_hash_t hash;
} cprnths_string_t;


// Create a new string from the given amount of characters of the given buffer.
// Return NULL if something went wrong.
cprnths_string_t*
cprnths_string_create(
    char const *,
    size_t
);

// Make a copy of the given string and return a pointer to it.
// Return NULL if something went wrong.
cprnths_string_t*
cprnths_string_copy(
    cprnths_string_t const *
);

// Compare two strings and report whether they're equal.
bool
cprnths_string_equal(
    cprnths_string_t const *,
    cprnths_string_t const *
);

// Destruct and free() the given string.
void
cprnths_string_destruct(
    cprnths_string_t*
);


#endif
