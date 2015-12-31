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


// Typedefs and functions for expressions
#ifndef CPARENTHESES_INCLUDE_EXPR
#define CPARENTHESES_INCLUDE_EXPR 1


struct cprnths_expr_t;
struct cprnths_exprs_t;


#include <stddef.h>
// size_t

#include "parser.h"
// cprnths_parse_stat_t

#include "exec.h"
// cprnths_execenv_t

#include "reference.h"
// cprnths_ref_t


struct cprnths_exprcls_t;

// An expression.
struct cprnths_expr_t {
    struct cprnths_exprcls_t const * const cls;
};

typedef
// Parse (create) an expression.
cprnths_parse_stat_t
// (see parser.h)
(*cprnths_expr_parse_t)(
    char const **,
    // current position (not NULL, SHALL be changed in case of success)
    char const *,
    // end of string (not NULL)
    struct cprnths_expr_t **
    // where to store the parsed expression? (not NULL)
);

typedef
// Evaluate an expression.
_Bool
// was the evaluation successful?
(*cprnths_expr_eval_t)(
    struct cprnths_expr_t const *,
    // not NULL
    struct cprnths_execenv_t*,
    // the environment to evaluate the expression in (not NULL)
    struct cprnths_ref_t**
    // where to store the value/result? (or NULL)
);

typedef
// Destroy an expression.
void
(*cprnths_expr_destroy_t)(
    struct cprnths_expr_t*
    // not NULL
);

// This is for storing information about expression types non-redundandly.
struct cprnths_exprcls_t {
    cprnths_expr_parse_t expr_parse;
    // not NULL

    cprnths_expr_eval_t expr_eval;
    // not NULL

    cprnths_expr_destroy_t expr_destroy;
    // MAY be NULL
};

// An array of expressions.
struct cprnths_exprs_t {
    // The array itself (or NULL if empty)
    struct cprnths_expr_t* exprs;

    // The array's length
    size_t length;
};


// Destroy an expression.
void
cprnths_expr_destroy(
    struct cprnths_expr_t*
    // not NULL
);


#endif
