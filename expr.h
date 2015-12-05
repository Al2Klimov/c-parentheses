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


// Typedefs for expressions
#ifndef CPARENTHESES_INCLUDE_EXPR
#define CPARENTHESES_INCLUDE_EXPR 1


struct cprnths_expr_t;
struct cprnths_exprs_t;


#include <stdbool.h>
// bool

#include <stddef.h>
// size_t

#include "exec.h"
// cprnths_execenv_t

#include "reference.h"
// cprnths_ref_t


// An expression.
struct cprnths_expr_t {
    // Evaluate an expression.
    bool
    // was the evaluation sucessful?
    (*eval)(
        struct cprnths_expr_t const *,
        // not NULL
        struct cprnths_execenv_t*,
        // the environment to evaluate the expression in (not NULL)
        struct cprnths_ref_t**
        // where to store the value/result? (or NULL)
    );
};

// An array of expressions.
struct cprnths_exprs_t {
    // The array itself (or NULL if empty)
    struct cprnths_expr_t* exprs;

    // The array's length
    size_t length;
};


#endif
