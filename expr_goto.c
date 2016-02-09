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


// Expression: goto (jump)


#include <stddef.h>
// NULL

#include <stdlib.h>
// malloc()
// free()

#include "expr.h"
// cprnths_expr_t
// cprnths_exprcls_t
// cprnths_expr_destroy()
// cprnths_expr_eval()
// cprnths_jmptab_row_t

#include "parser.h"
// cprnths_parse_anyexpr()
// cprnths_jmptab_prep_t
// cprnths_parseutil_funccall_start()
// cprnths_parseutil_funccall_end()

#include "error.h"
// cprnths_error_*

#include "exec.h"
// cprnths_execenv_t

#include "reference.h"
// cprnths_ref_t
// cprnths_ref_increment()

#include "string.h"
// cprnths_string_t
// cprnths_string_equal()

#include "object.h"
// cprnths_class_t

#include "obj_string.h"
// cprnths_obj_string_t

#include "obj_bool.h"
// cprnths_obj_bool_create()


extern struct cprnths_class_t const cprnths_class_string;

struct cpintern_expr_goto_t {
    struct cprnths_expr_t base;
    struct cprnths_expr_t* label;
    // not NULL
};

struct cprnths_exprcls_t const cprnths_exprcls_goto;

static
cprnths_error_t
cpintern_expr_goto_parse(
    char const * *restrict const current_,
    char const *const end,
    struct cprnths_expr_t* *restrict const expr_,
    struct cprnths_jmptab_prep_t *restrict const jmptab_prep
) {
    char const *restrict current = *current_;
    cprnths_error_t err;

    if (( err = cprnths_parseutil_funccall_start((char const **)&current, end, "GoTo") ))
        goto Finish;

    {
        struct cpintern_expr_goto_t *restrict const expr = malloc(sizeof(struct cpintern_expr_goto_t));
        if (expr == NULL) {
            err = cprnths_error_nomem;
            goto Finish;
        }

        switch (( err = cprnths_parse_anyexpr((char const **)&current, end, &expr->label, jmptab_prep) )) {
            case 0:
                break;
            case cprnths_error_parse_unknown:
                err = cprnths_error_parse_malform;
            default:
                goto CleanUpExpr;
        }

        if (( err = cprnths_parseutil_funccall_end((char const **)&current, end) ))
            goto CleanUpLabel;

        *(struct cprnths_exprcls_t const **)&expr->base.cls = &cprnths_exprcls_goto;
        *expr_ = (struct cprnths_expr_t*)expr;
        // Already done while calling cprnths_parse_anyexpr().
        //err = 0;
        goto Finish;

CleanUpLabel:
        cprnths_expr_destroy(expr->label);

CleanUpExpr:
        free(expr);
    }

Finish:
    *current_ = current;
    return err;
}

static
cprnths_error_t
cpintern_expr_goto_eval(
    struct cprnths_expr_t const *restrict const expr,
    struct cprnths_execenv_t *restrict const env,
    struct cprnths_ref_t* *restrict const ref
) {
    cprnths_error_t err;
    struct cprnths_ref_t *restrict label_ref;
    if (( err = cprnths_expr_eval(
        ((struct cpintern_expr_goto_t const *)expr)->label, env, (struct cprnths_ref_t**)&label_ref
    ) ))
        return err;

    if (label_ref == NULL) {
        if (ref != NULL)
            *ref = NULL;
        return 0;
    }

    if (label_ref->obj->cls == &cprnths_class_string) {
        _Bool valid_label = 0;

        if (env->stack->current_frame->jmptab != NULL) {
            struct cprnths_jmptab_row_t const *restrict jmptab_row = env->stack->current_frame->jmptab;
            struct cprnths_string_t const *restrict const label = ((struct cprnths_obj_string_t const *)label_ref->obj)->value;
            do {
                if (cprnths_string_equal(jmptab_row->label, label)) {
                    env->stack->current_frame->next_stmt = jmptab_row->stmt;
                    env->stack->current_frame->going_to = jmptab_row->label;
                    valid_label = 1;
                    break;
                }
            } while ((++jmptab_row)->stmt != NULL);
        }

        if (ref != NULL)
            err = cprnths_obj_bool_create(valid_label, ref);
    } else if (ref != NULL) {
        *ref = NULL;
    }

    cprnths_ref_increment(label_ref, -1);
    return err;
}

static
void
cpintern_expr_goto_destroy(
    struct cprnths_expr_t *restrict const e
) {
    cprnths_expr_destroy(((struct cpintern_expr_goto_t*)e)->label);
}

struct cprnths_exprcls_t const cprnths_exprcls_goto = {
    &cpintern_expr_goto_parse,
    &cpintern_expr_goto_eval,
    &cpintern_expr_goto_destroy
};
