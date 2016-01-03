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


// Functions for parsing
#ifndef CPARENTHESES_INCLUDE_PARSER
#define CPARENTHESES_INCLUDE_PARSER 1


#include <stddef.h>
// size_t

#include <string.h>
// memcmp()

#include "expr.h"
// cprnths_expr_t

#include "error.h"
// cprnths_error_t


// Is a character in [A-Za-z0-9_] ?
static inline
_Bool
cprnths_parseutil_is_wordchar(
    char const c
) {
    return ( 47 < c && c < 58 )     // 0-9
        || ( 64 < c && c < 91 )     // A-Z
        || c == 95                  // _
        || ( 96 < c && c < 123 );   // a-z
}

// Does <subj> start with word <frag>?
static inline
signed char
// -1: no
// 0: no word
// 1: yes
// 2: yes, followed by EOF
cprnths_parseutil_startswith_word(
    char const * *restrict const subj,
    // not NULL
    char const *const end,
    // end of <subj> (not NULL)
    char const *restrict const frag,
    // not NULL
    size_t const len
    // length of <frag> (> 0)
) {
    if ((size_t)(end - *subj) < len || memcmp(*subj, frag, len))
        return -1;
    if ((*subj += len) == end)
        return 2;
    return cprnths_parseutil_is_wordchar(**subj);
}

// Skip whitespace and comments (C and BCPL).
cprnths_error_t
// (see error.h)
cprnths_parseutil_skip_spcomm(
    char const **,
    // current position (not NULL)
    char const *
    // end of string (not NULL)
);

// Parse any expression.
cprnths_error_t
// (see error.h)
cprnths_parse_anyexpr(
    char const **,
    // current position (not NULL)
    char const *,
    // end of string (not NULL)
    struct cprnths_expr_t**
    // where to store the result in case of success (not NULL)
);


#endif
