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


// Object and expression: boolean


#include <stddef.h>
// NULL

#include <stdlib.h>
// malloc()
// free()

#include "exec.h"
// cprnths_execenv_t

#include "reference.h"
// cprnths_ref_t
// cprnths_ref_create()

#include "expr.h"
// cprnths_expr_t
// cprnths_exprcls_t

#include "parser.h"
// cprnths_parseutil_startswith_word()

#include "error.h"
// cprnths_error_*

#include "object.h"
// cprnths_obj_t
// cprnths_class_t

#include "obj_bool.h"
// cprnths_obj_bool_t


static
_Bool
cpintern_obj_bool_2bool(
    struct cprnths_obj_t const *restrict const obj
) {
    return ((struct cprnths_obj_bool_t const *)obj)->value;
}

struct cprnths_class_t const cprnths_class_bool = {
    sizeof(struct cprnths_obj_bool_t),
    NULL,
    NULL,
    &cpintern_obj_bool_2bool
};

cprnths_error_t
cprnths_obj_bool_create(
    _Bool const value,
    struct cprnths_ref_t* *restrict const target
) {
    struct cprnths_obj_bool_t *restrict const obj = malloc(sizeof(struct cprnths_obj_bool_t));
    if (obj == NULL)
        return cprnths_error_nomem;

    {
        cprnths_error_t const err = cprnths_ref_create(
            target, (struct cprnths_obj_t*)obj, NULL
        );
        if (err) {
            free(obj);
            return err;
        }
    }

    *(struct cprnths_class_t const **)&obj->base.cls = &cprnths_class_bool;
    *(_Bool*)&obj->value = value;
    return 0;
}

struct cpintern_expr_bool_t {
    struct cprnths_expr_t base;
    _Bool const value;
};

struct cprnths_exprcls_t const cprnths_exprcls_bool;

static
cprnths_error_t
cpintern_expr_bool_parse(
    char const * *restrict const current_,
    char const *const end,
    struct cprnths_expr_t* *restrict const expr_,
    struct cprnths_jmptab_prep_t *restrict const jmptab_prep
) {
    (void)jmptab_prep;

    _Bool value;

    {
        char const *restrict current = *current_;

        switch (cprnths_parseutil_startswith_word((char const **)&current, end, "True")) {
            case -1:
            case 0:
                switch (cprnths_parseutil_startswith_word((char const **)&current, end, "False")) {
                    case -1:
                    case 0:
                        return cprnths_error_parse_unknown;
                }
                value = 0;
                break;
            default:
                value = 1;
        }

        *current_ = current;
    }

    struct cpintern_expr_bool_t *restrict const expr = malloc(sizeof(struct cpintern_expr_bool_t));
    if (expr == NULL)
        return cprnths_error_nomem;

    *(struct cprnths_exprcls_t const **)&expr->base.cls = &cprnths_exprcls_bool;
    *(_Bool*)&expr->value = value;
    *expr_ = (struct cprnths_expr_t*)expr;
    return 0;
}

static
cprnths_error_t
cpintern_expr_bool_eval(
    struct cprnths_expr_t const *restrict const expr,
    struct cprnths_execenv_t *restrict const env,
    struct cprnths_ref_t* *restrict const target
) {
    (void)env;

    // Not checking for target == NULL as
    // boolean constants aren't allowed in void context.
    return cprnths_obj_bool_create(
        ((struct cpintern_expr_bool_t const *)expr)->value, target
    );
}

struct cprnths_exprcls_t const cprnths_exprcls_bool = {
    &cpintern_expr_bool_parse,
    &cpintern_expr_bool_eval,
    NULL
};
