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


// Function definitions for expressions


#include <stddef.h>
// NULL

#include <stdlib.h>
// free()

#include "expr.h"
// cprnths_expr_t
// cprnths_exprs_t

#include "exec.h"
// cprnths_execenv_t

#include "reference.h"
// cprnths_ref_t


cprnths_error_t
cprnths_expr_eval(
    struct cprnths_expr_t const *restrict const expr,
    struct cprnths_execenv_t *restrict const env,
    struct cprnths_ref_t* *restrict const ref
) {
    return (*expr->cls->expr_eval)(expr, env, ref);
}

void
cprnths_expr_destroy(
    struct cprnths_expr_t *restrict const e
) {
    if (e->cls->expr_destroy != NULL)
        (*e->cls->expr_destroy)(e);
    free(e);
}

void
cprnths_exprs_destroy(
    struct cprnths_exprs_t *restrict const e
) {
    if (e->exprs != NULL) {
        {
            struct cprnths_expr_t* *restrict expr = e->exprs;
            do cprnths_expr_destroy(*expr);
            while (*++expr != NULL);
        }
        free(e->exprs);
    }
    free(e);
}
