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


// A (key, value) pair of a dictionary (just for easy reusing)
typedef struct {
    cprnths_string_t* key;
    cprnths_ref_t* value;
} cprnths_dict_pair_t;

// This is for mapping particular keys to particular values.
// (You know, an associative array.)
typedef struct {
    // The dictionary itself
    cprnths_dict_pair_t* dict;

    // Amount of slots to allocate memory for at once
    // 1 slot == sizeof( cprnths_dict_pair_t )
    // SHALL be > 0!
    size_t const chunksize;

    // Amount of present slots
    // SHALL be > 0 and multiple of .chunksize!
    size_t slots_total;

    // Amount of free slots
    // SHALL be <= .slots_total!
    size_t slots_free;
} cprnths_dict_t;


// Create a new dictionary with given .chunksize and return a pointer to it.
// Return NULL if something went wrong.
cprnths_dict_t*
cprnths_dict_create(
    size_t
);

// Add the given (key, value) pair to the given dictionary.
// If a pair with the given key already exists, override the value.
// Return true if (and only if) the pair was added (or overridden) successfully.
bool
cprnths_dict_addpair(
    cprnths_dict_t*,
    cprnths_string_t const *,
    cprnths_ref_t*
);

// If the given key is listed in the given dictionary,
// return a pointer to the value. Otherwise, return NULL.
cprnths_ref_t*
cprnths_dict_getval(
    cprnths_dict_t const *,
    cprnths_string_t const *
);

// If the given key is listed in the given dictionary,
// remove the pair and return a (usable) pointer to the value.
// Otherwise, return NULL.
cprnths_ref_t*
cprnths_dict_popval(
    cprnths_dict_t*,
    cprnths_string_t const *
);

// Remove the given key from the given dictionary if it's listed there
// and report whether it was listed there.
bool
cprnths_dict_delpair(
    cprnths_dict_t*,
    cprnths_string_t const *
);

// Destruct and free() the given dictionary.
void
cprnths_dict_destruct(
    cprnths_dict_t*
);


#endif
