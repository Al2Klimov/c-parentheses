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


// Object: string
#ifndef CPARENTHESES_INCLUDE_OBJ_STRING
#define CPARENTHESES_INCLUDE_OBJ_STRING 1


#include "reference.h"
// cprnths_ref_t

#include "error.h"
// cprnths_error_t

#include "string.h"
// cprnths_string_t


// Create a new reference to a new string.
cprnths_error_t
// (see error.h)
cprnths_obj_string_create(
    struct cprnths_string_t const *,
    // the new string's value
    struct cprnths_ref_t**
    // a new reference (not NULL)
);


#endif
