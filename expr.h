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


// Typedefs for expressions
#ifndef CPARENTHESES_INCLUDE_EXPR
#define CPARENTHESES_INCLUDE_EXPR


#include "reference.h"
// cprnths_ref_t


// An expression.
struct cpintern_expr_t;
typedef struct cpintern_expr_t cprnths_expr_t;
struct cpintern_expr_t {
    // Evaluate the given expression and return the value.
    // Return NULL if something went wrong.
    cprnths_ref_t*
    (*eval)(
        cprnths_expr_t const *
    );
};


#endif
