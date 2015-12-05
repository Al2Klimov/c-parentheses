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


// Typedefs and functions for dictionaries
#ifndef CPARENTHESES_INCLUDE_DICT
#define CPARENTHESES_INCLUDE_DICT 1


#include <stddef.h>
// size_t

#include <stdbool.h>
// bool

#include "string.h"
// cprnths_string_t

#include "reference.h"
// cprnths_ref_t


// A (key, value) pair of a dictionary
struct cprnths_dict_pair_t {
    struct cprnths_string_t* key;
    // not NULL

    struct cprnths_ref_t* value;
    // not NULL
};

// An associative array. (aka "dictionary")
struct cprnths_dict_t {
    // The dictionary itself (not NULL)
    struct cprnths_dict_pair_t* dict;

    // Amount of slots to allocate memory for at once
    // 1 slot == sizeof( struct cprnths_dict_pair_t )
    // SHALL be > 0!
    size_t const chunksize;

    // Amount of present slots
    // SHALL be > 0 and multiple of .chunksize!
    size_t slots_total;

    // Amount of free slots
    // SHALL be <= .slots_total!
    size_t slots_free;
};


// Create a new dictionary.
struct cprnths_dict_t*
// a new dictionary or NULL
cprnths_dict_create(
    size_t
    // .chunksize
);

// Add the given (key, value) pair to a dictionary.
// If the given key already exists, override the value.
bool
// was the addition successful?
cprnths_dict_addpair(
    struct cprnths_dict_t*,
    // not NULL
    struct cprnths_string_t const *,
    // key (not NULL)
    struct cprnths_ref_t*
    // value (not NULL)
);

// If a key is listed in a dictionary, return the value.
struct cprnths_ref_t*
// the value or NULL
cprnths_dict_getval(
    struct cprnths_dict_t const *,
    // not NULL
    struct cprnths_string_t const *
    // key (not NULL)
);

// If a key is listed in a dictionary, remove the pair and return the value.
struct cprnths_ref_t*
// the value or NULL
cprnths_dict_popval(
    struct cprnths_dict_t*,
    // not NULL
    struct cprnths_string_t const *
    // key (not NULL)
);

// Remove a key from a dictionary if it's listed there.
bool
// was it listed there?
cprnths_dict_delpair(
    struct cprnths_dict_t*,
    // not NULL
    struct cprnths_string_t const *
    // key (not NULL)
);

// Destroy a dictionary.
void
cprnths_dict_destroy(
    struct cprnths_dict_t*
    // not NULL
);


#endif
