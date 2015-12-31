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


// Expression: variable access


#include <stddef.h>
// NULL

#include <stdlib.h>
// malloc()
// free()

#include "expr.h"
// cprnths_expr_t
// cprnths_exprcls_t
// cprnths_expr_parse_t
// cprnths_expr_eval_t
// cprnths_expr_destroy_t

#include "parser.h"
// cprnths_parse_stat_t
// cprnths_parse_success
// cprnths_parse_unknown
// cprnths_parse_fail
// cprnths_parseutil_is_wordchar()

#include "string.h"
// cprnths_string_t
// cprnths_string_create()

#include "exec.h"
// cprnths_execenv_t

#include "reference.h"
// cprnths_ref_t
// cprnths_ref_increment()

#include "dict.h"
// cprnths_dict_getval()


struct cpintern_expr_variable_t {
    struct cprnths_expr_t base;
    struct cprnths_string_t* varname;
    _Bool global;
};

struct cprnths_exprcls_t const cprnths_exprcls_variable;

static
cprnths_parse_stat_t
cpintern_expr_variable_parse(
    char const * *restrict const current_,
    char const *const end,
    struct cpintern_expr_variable_t* *restrict const expr_
) {
    if (*current_ == end)
        return cprnths_parse_unknown;

    _Bool global;
    switch (**current_) {
        case '@':
            global = 1;
            break;
        case '#':
            global = 0;
            break;
        default:
            return cprnths_parse_unknown;
    }

    char const *restrict current = *current_ + 1u;
    char const *const start = current;
    while (current != end && cprnths_parseutil_is_wordchar(*current))
        ++current;

    struct cpintern_expr_variable_t *restrict const expr = malloc(sizeof(struct cpintern_expr_variable_t));
    if (expr == NULL)
        return cprnths_parse_fail;

    expr->varname = cprnths_string_create(start, current - start);
    if (expr->varname == NULL) {
        free(expr);
        return cprnths_parse_fail;
    }

    *(struct cprnths_exprcls_t const **)&expr->base.cls = &cprnths_exprcls_variable;
    expr->global = global;
    *expr_ = expr;
    *current_ = current;
    return cprnths_parse_success;
}

static
_Bool
cpintern_expr_variable_eval(
    struct cpintern_expr_variable_t const *restrict const expr,
    struct cprnths_execenv_t *restrict const env,
    struct cprnths_ref_t* *restrict const target
) {
    // Not checking for target == NULL as
    // variable access isn't allowed in void context.
    *target = cprnths_dict_getval(
        expr->global ? env->gsymbtab : env->stack->current_frame->lsymbtab,
        expr->varname
    );
    if (*target != NULL)
        cprnths_ref_increment(*target, 1u);
    return 1;
}

static
void
cpintern_expr_variable_destroy(
    struct cpintern_expr_variable_t *restrict const e
) {
    cprnths_string_destroy(e->varname);
}

struct cprnths_exprcls_t const cprnths_exprcls_variable = {
    (cprnths_expr_parse_t)&cpintern_expr_variable_parse,
    (cprnths_expr_eval_t)&cpintern_expr_variable_eval,
    (cprnths_expr_destroy_t)&cpintern_expr_variable_destroy
};
