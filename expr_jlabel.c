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


// Expression: jump label


#include <stddef.h>
// NULL
// size_t

#include <stdlib.h>
// malloc()
// realloc()
// free()

#include "string.h"
// cprnths_string_t
// cprnths_string_destroy()
// cprnths_string_equal()

#include "expr.h"
// cprnths_expr_t
// cprnths_exprcls_t

#include "parser.h"
// cprnths_parseutil_funccall_start()
// cprnths_parseutil_skip_spcomm()
// cprnths_parseutil_string()
// cprnths_jmptab_prep_t
// cprnths_jmptab_prep_row_t

#include "error.h"
// cprnths_error_*

#include "exec.h"
// cprnths_execenv_t

#include "reference.h"
// cprnths_ref_t

#include "obj_bool.h"
// cprnths_obj_bool_create()


struct cpintern_expr_jlabel_t {
    struct cprnths_expr_t base;
    struct cprnths_string_t *const label;
    // not NULL
};

struct cprnths_exprcls_t const cprnths_exprcls_jlabel;

static
cprnths_error_t
cpintern_expr_jlabel_parse(
    char const * *restrict const current_,
    char const *const end,
    struct cprnths_expr_t* *restrict const expr_,
    struct cprnths_jmptab_prep_t *restrict const jmptab_prep
) {
    char const *restrict current = *current_;
    cprnths_error_t err;

    if (( err = cprnths_parseutil_funccall_start((char const **)&current, end, "JLabel") ))
        goto Finish;

    {
        struct cpintern_expr_jlabel_t *restrict const expr = malloc(sizeof(struct cpintern_expr_jlabel_t));
        if (expr == NULL) {
            err = cprnths_error_nomem;
            goto Finish;
        }

        {
            char const *const current_before_label = current;

            switch (( err = cprnths_parseutil_string((char const **)&current, end, (struct cprnths_string_t**)&expr->label) )) {
                case 0:
                    break;
                case cprnths_error_parse_unknown:
                    err = cprnths_error_parse_malform;
                default:
                    goto CleanUpExpr;
            }

            if (current == end) {
                err = cprnths_error_parse_eof;
                goto CleanUpLabel;
            }

            if (*current != ')') {
                if (( err = cprnths_parseutil_skip_spcomm((char const **)&current, end) ))
                    goto CleanUpLabel;
                if (current == end) {
                    err = cprnths_error_parse_eof;
                    goto CleanUpLabel;
                }

                if (*current != ')') {
                    err = cprnths_error_parse_malform;
                    goto CleanUpLabel;
                }
            }
            ++current;

            if (jmptab_prep->used) {
                struct cprnths_jmptab_prep_row_t const *const jmptab_prep_row_last = jmptab_prep->jmptab_prep;
                struct cprnths_jmptab_prep_row_t const *restrict jmptab_prep_row_current = jmptab_prep_row_last + jmptab_prep->used;
                do {
                    if (cprnths_string_equal((--jmptab_prep_row_current)->label, expr->label)) {
                        if (jmptab_prep_row_current->stmt_offset == jmptab_prep->current_stmt_offset)
                            goto SkipJmptabPrep;
                        err = cprnths_error_parse_double_jlabel;
                        current = current_before_label;
                        goto CleanUpLabel;
                    }
                } while (jmptab_prep_row_current != jmptab_prep_row_last);
            }
        }

        if (jmptab_prep->used == jmptab_prep->available) {
            size_t const new_size = jmptab_prep->available + 16u;
            {
                struct cprnths_jmptab_prep_row_t *const reallocate = realloc(
                    jmptab_prep->jmptab_prep, new_size * sizeof(struct cprnths_jmptab_prep_row_t)
                );
                if (reallocate == NULL) {
                    err = cprnths_error_nomem;
                    goto CleanUpLabel;
                }
                jmptab_prep->jmptab_prep = reallocate;
            }
            jmptab_prep->available = new_size;
        }

        jmptab_prep->jmptab_prep[jmptab_prep->used].label = expr->label;
        jmptab_prep->jmptab_prep[jmptab_prep->used].stmt_offset = jmptab_prep->current_stmt_offset;
        ++jmptab_prep->used;
SkipJmptabPrep:

        *(struct cprnths_exprcls_t const **)&expr->base.cls = &cprnths_exprcls_jlabel;
        *expr_ = (struct cprnths_expr_t*)expr;
        // Already done while calling cprnths_parseutil_string().
        //err = 0;
        goto Finish;

CleanUpLabel:
        cprnths_string_destroy(expr->label);

CleanUpExpr:
        free(expr);
    }

Finish:
    *current_ = current;
    return err;
}

static
cprnths_error_t
cpintern_expr_jlabel_eval(
    struct cprnths_expr_t const *restrict const expr,
    struct cprnths_execenv_t *restrict const env,
    struct cprnths_ref_t* *restrict const ref
) {
    if (ref == NULL)
        return 0;

    struct cprnths_string_t const *restrict const came_from = env->stack->current_frame->came_from;
    struct cprnths_string_t const *restrict const label = ((struct cpintern_expr_jlabel_t const *)expr)->label;
    return cprnths_obj_bool_create(came_from != NULL && (
        came_from == label || cprnths_string_equal(came_from, label)
    ), ref);
}

static
void
cpintern_expr_jlabel_destroy(
    struct cprnths_expr_t *restrict const e
) {
    cprnths_string_destroy(((struct cpintern_expr_jlabel_t*)e)->label);
}

struct cprnths_exprcls_t const cprnths_exprcls_jlabel = {
    &cpintern_expr_jlabel_parse,
    &cpintern_expr_jlabel_eval,
    &cpintern_expr_jlabel_destroy
};
