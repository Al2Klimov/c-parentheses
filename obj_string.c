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


// Object and expression: string


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
// cprnths_parseutil_string()

#include "error.h"
// cprnths_error_*

#include "object.h"
// cprnths_obj_t
// cprnths_class_t

#include "string.h"
// cprnths_string_t
// cprnths_string_copy()
// cprnths_string_destroy()

#include "copy_table.h"
// cprnths_copytab_t


struct cpintern_obj_string_t {
    struct cprnths_obj_t base;
    struct cprnths_string_t *const value;
};

static
void
cpintern_obj_string_destroy(
    struct cprnths_obj_t *restrict const obj
) {
    cprnths_string_destroy(((struct cpintern_obj_string_t*)obj)->value);
}

static
cprnths_error_t
cpintern_obj_string_copy(
    struct cprnths_obj_t *restrict const obj,
    struct cprnths_copytab_t *restrict const copytab
) {
    (void)copytab;

    struct cprnths_string_t* *const value = (struct cprnths_string_t**)&((struct cpintern_obj_string_t*)obj)->value;
    return cprnths_string_copy(*value, value);
}

static
_Bool
cpintern_obj_string_2bool(
    struct cprnths_obj_t const *restrict const obj
) {
    return ((struct cpintern_obj_string_t const *)obj)->value->length;
}

struct cprnths_class_t const cprnths_class_string = {
    sizeof(struct cpintern_obj_string_t),
    &cpintern_obj_string_destroy,
    &cpintern_obj_string_copy,
    &cpintern_obj_string_2bool
};

cprnths_error_t
cprnths_obj_string_create(
    struct cprnths_string_t const *restrict const value,
    struct cprnths_ref_t* *restrict const target
) {
    struct cpintern_obj_string_t *restrict const obj = malloc(sizeof(struct cpintern_obj_string_t));
    if (obj == NULL)
        return cprnths_error_nomem;

    cprnths_error_t err;

    if (( err = cprnths_string_copy(value, (struct cprnths_string_t**)&obj->value) ))
        goto CleanupObj;

    if (( err = cprnths_ref_create(target, (struct cprnths_obj_t*)obj, NULL) ))
        goto CleanupValue;

    *(struct cprnths_class_t const **)&obj->base.cls = &cprnths_class_string;
    return 0;

CleanupValue:
    cprnths_string_destroy(obj->value);

CleanupObj:
    free(obj);

    return err;
}

struct cpintern_expr_string_t {
    struct cprnths_expr_t base;
    struct cprnths_string_t *const value;
};

struct cprnths_exprcls_t const cprnths_exprcls_string;

static
cprnths_error_t
cpintern_expr_string_parse(
    char const * *restrict const current,
    char const *const end,
    struct cprnths_expr_t* *restrict const expr_
) {
    struct cprnths_string_t *restrict value;
    {
        cprnths_error_t const err = cprnths_parseutil_string(
            current, end, (struct cprnths_string_t**)&value
        );
        if (err)
            return err;
    }

    struct cpintern_expr_string_t *restrict const expr = malloc(sizeof(struct cpintern_expr_string_t));
    if (expr == NULL) {
        cprnths_string_destroy(value);
        return cprnths_error_nomem;
    }

    *(struct cprnths_exprcls_t const **)&expr->base.cls = &cprnths_exprcls_string;
    *(struct cprnths_string_t**)&expr->value = value;
    *expr_ = (struct cprnths_expr_t*)expr;
    return 0;
}

static
cprnths_error_t
cpintern_expr_string_eval(
    struct cprnths_expr_t const *restrict const expr,
    struct cprnths_execenv_t *restrict const env,
    struct cprnths_ref_t* *restrict const target
) {
    (void)env;

    // Not checking for target == NULL as
    // string constants aren't allowed in void context.
    return cprnths_obj_string_create(
        ((struct cpintern_expr_string_t const *)expr)->value, target
    );
}

static
void
cpintern_expr_string_destroy(
    struct cprnths_expr_t *restrict const expr
) {
    cprnths_string_destroy(((struct cpintern_expr_string_t*)expr)->value);
}

struct cprnths_exprcls_t const cprnths_exprcls_string = {
    &cpintern_expr_string_parse,
    &cpintern_expr_string_eval,
    &cpintern_expr_string_destroy
};
