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


// Typedefs and functions for parsing
#ifndef CPARENTHESES_INCLUDE_PARSER
#define CPARENTHESES_INCLUDE_PARSER 1


struct cprnths_jmptab_prep_t;


#include <stddef.h>
// size_t

#include <string.h>
// memcmp()

#include "expr.h"
// cprnths_expr_t
// cprnths_exprs_t

#include "error.h"
// cprnths_error_t

#include "string.h"
// cprnths_string_t


// This is for preparing the jump table.
struct cprnths_jmptab_prep_row_t {
    struct cprnths_string_t** labels;
    // labels of a statement (not NULL)
    size_t stmt_offset;
    // index of the statement
};

// This is for preparing the jump table.
struct cprnths_jmptab_prep_t {
    struct cprnths_jmptab_prep_row_t* jmptab_prep;
    // labels to statements mapping
    size_t available;
    // .jmptab_prep's size
    size_t used;
    // .jmptab_prep's usage
    size_t current_stmt_offset;
    // index of the currently parsed statement
};

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
    return !cprnths_parseutil_is_wordchar(**subj);
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
    struct cprnths_expr_t**,
    // where to store the result in case of success (not NULL)
    struct cprnths_jmptab_prep_t*
    // (see above, not NULL)
);

// Parse statement expressions.
cprnths_error_t
// (see error.h)
cprnths_parse_stmtexprs(
    char const **,
    // current position (not NULL)
    char const *,
    // end of string (not NULL)
    struct cprnths_exprs_t**
    // where to store the result in case of success (not NULL)
);

// Parse a file's content.
cprnths_error_t
// (see error.h)
cprnths_parse_file(
    char const **,
    // start (not NULL)
    char const *,
    // end (not NULL)
    struct cprnths_exprs_t**
    // where to store the result in case of success (not NULL)
);


#endif
