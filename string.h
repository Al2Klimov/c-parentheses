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


// Typedefs and functions for strings
#ifndef CPARENTHESES_INCLUDE_STRING
#define CPARENTHESES_INCLUDE_STRING 1


#include <stdint.h>
// uintmax_t

#include <stddef.h>
// size_t

#include "error.h"
// cprnths_error_t


// The type of a string's hash (just for easy changing)
typedef uintmax_t cprnths_string_hash_t;

// A string.
// (You know, a sequence of characters.)
struct cprnths_string_t {
    // The string itself (not NULL)
    char const * str;

    // The string's length
    size_t length;

    // The string's hash
    cprnths_string_hash_t hash;
};


// Create a new string from a buffer.
cprnths_error_t
// (see error.h)
cprnths_string_create(
    struct cprnths_string_t**,
    // a new string (not NULL)
    char const *,
    // not NULL if not empty
    size_t
    // the amount of characters to use
);

// Create a new string from a custom data source.
cprnths_error_t
// (see error.h)
cprnths_string_create_customds(
    struct cprnths_string_t**,
    // a new string (not NULL)
    size_t,
    // the string's length
    void (*)(void*, char*, size_t),
    // This function (not NULL) will be called
    // once the target buffer becomes ready to be written to.
    // It should write something useful to the buffer.
    // Arguments:
    //   1. (see below)
    //   2. the target buffer
    //   3. the target buffer's length
    void*
    // MAY or MUST NOT be NULL, depending on the above function
);

// Copy a string.
cprnths_error_t
// (see error.h)
cprnths_string_copy(
    struct cprnths_string_t const *,
    // the original (not NULL)
    struct cprnths_string_t**
    // the copy (not NULL)
);

// Compare two strings.
_Bool
// are they equal?
cprnths_string_equal(
    struct cprnths_string_t const *,
    // not NULL
    struct cprnths_string_t const *
    // not NULL
);

// Destroy a string.
void
cprnths_string_destroy(
    struct cprnths_string_t*
    // not NULL
);


#endif
