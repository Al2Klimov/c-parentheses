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


// Expression: assignment


#include <stddef.h>
// size_t
// NULL

#include <stdlib.h>
// malloc()
// realloc()
// free()

#include "expr.h"
// cprnths_expr_t
// cprnths_exprcls_t
// cprnths_expr_parse_t
// cprnths_expr_eval_t
// cprnths_expr_destroy_t
// cprnths_expr_destroy()

#include "parser.h"
// cprnths_parseutil_startswith_word()
// cprnths_parseutil_is_wordchar()
// cprnths_parseutil_skip_spcomm()
// cprnths_parse_anyexpr()

#include "error.h"
// cprnths_error_*

#include "exec.h"
// cprnths_execenv_t

#include "reference.h"
// cprnths_ref_t
// cprnths_ref_increment()

#include "string.h"
// cprnths_string_t
// cprnths_string_create()
// cprnths_string_destroy()

#include "dict.h"
// cprnths_dict_t
// cprnths_dict_addpair()
// cprnths_dict_delpair()


struct cpintern_expr_assign_t {
    struct cprnths_expr_t base;
    struct cprnths_expr_t* inner_expr;
    // not NULL
    struct cprnths_string_t** global;
    // may be NULL
    struct cprnths_string_t** local;
    // may be NULL
};

struct cprnths_exprcls_t const cprnths_exprcls_assign;

static
cprnths_error_t
cpintern_expr_assign_parse(
    char const * *restrict const current_,
    char const *const end,
    struct cpintern_expr_assign_t* *restrict const expr_
) {
#define SKIP_SPCOMM(x) {\
    if (( err = cprnths_parseutil_skip_spcomm((char const **)&current, end) ))\
        goto x;\
}
#define ASSERT_NO_EOF(x) {\
    if (current == end) {\
        err = cprnths_error_parse_eof;\
        goto x;\
    }\
}
#define ASSERT_CHAR(x, y) {\
    if (*current != (x)) {\
        SKIP_SPCOMM(y)\
        ASSERT_NO_EOF(y)\
        if (*current != (x)) {\
            err = cprnths_error_parse_malform;\
            goto y;\
        }\
    }\
    ++current;\
}

    char const *restrict current = *current_;
    cprnths_error_t err;

    switch (cprnths_parseutil_startswith_word((char const **)&current, end, "Assign", 6u)) {
        case -1:
        case 0:
            return cprnths_error_parse_unknown;
        case 2:
            err = cprnths_error_parse_eof;
            goto Finish;
    }
    // Assign\W.*

    ASSERT_CHAR('(', Finish)
    // Assign\s*\(.*

    SKIP_SPCOMM(Finish)
    // Assign\s*\(\s*.*

    ASSERT_NO_EOF(Finish)
    // Assign\s*\(\s*.+

    {
        struct cpintern_expr_assign_t *restrict const expr = malloc(sizeof(struct cpintern_expr_assign_t));
        if (expr == NULL) {
            err = cprnths_error_nomem;
            goto Finish;
        }

        switch ( err = cprnths_parse_anyexpr((char const **)&current, end, &expr->inner_expr) ) {
            case 0:
                break;
            case cprnths_error_parse_unknown:
                err = cprnths_error_parse_malform;
            default:
                goto CleanUpOuterExpr;
        }
        ASSERT_NO_EOF(CleanUpInnerExpr)
        ASSERT_CHAR(',', CleanUpInnerExpr)

        if (NULL == ( expr->global = malloc(16u * sizeof(struct cprnths_string_t*)) )) {
            err = cprnths_error_nomem;
            goto CleanUpInnerExpr;
        }

        {
            size_t global_used = 0u;

            if (NULL == ( expr->local = malloc(16u * sizeof(struct cprnths_string_t*)) )) {
                err = cprnths_error_nomem;
                goto CleanUpGlobals;
            }

            {
                size_t local_used = 0u;

                {
                    size_t global_available = 16u, local_available = 16u;
                    struct cprnths_string_t** reallocate;

                    {
                        size_t *restrict used;
                        size_t *restrict available;
                        struct cprnths_string_t** *restrict target;
                        _Bool skipable;
                        char const * start;
                        for (;;) {
                            skipable = 1;
ParseVarName:
                            ASSERT_NO_EOF(CleanUpLocals)
                            switch (*current) {
                                case '@':
                                    used = &global_used;
                                    available = &global_available;
                                    target = &expr->global;
                                    break;
                                case '#':
                                    used = &local_used;
                                    available = &local_available;
                                    target = &expr->local;
                                    break;
                                default:
                                    if (skipable) {
                                        skipable = 0;
                                        SKIP_SPCOMM(CleanUpLocals)
                                        goto ParseVarName;
                                    }
                                    err = cprnths_error_parse_malform;
                                    goto CleanUpLocals;
                            }
                            ++current;

                            ASSERT_NO_EOF(CleanUpLocals)
                            start = current;
                            while (cprnths_parseutil_is_wordchar(*current)) {
                                ++current;
                                ASSERT_NO_EOF(CleanUpLocals)
                            }

                            if (( *used == *available )) {
                                if (NULL == ( reallocate = realloc(
                                    *target, (*available + 16u) * sizeof(struct cprnths_string_t*)
                                ) )) {
                                    err = cprnths_error_nomem;
                                    goto CleanUpLocals;
                                }
                                *target = reallocate;
                                *available += 16u;
                            }

                            if (NULL == (
                                (*target)[*used] = cprnths_string_create(start, current - start)
                            )) {
                                err = cprnths_error_nomem;
                                goto CleanUpLocals;
                            }
                            ++*used;

                            skipable = 1;
ParseNext:
                            switch (*current) {
                                case ')':
                                    ++current;
                                    goto ParseEnd;
                                case ',':
                                    ++current;
                                    break;
                                default:
                                    if (skipable) {
                                        skipable = 0;
                                        SKIP_SPCOMM(CleanUpLocals)
                                        ASSERT_NO_EOF(CleanUpLocals)
                                        goto ParseNext;
                                    }
                                    err = cprnths_error_parse_malform;
                                    goto CleanUpLocals;
                            }
                        }
                    }

                    size_t target_size;
ParseEnd:
                    if (global_used) {
                        target_size = global_used + 1u;
                        if (global_available != target_size) {
                            if (NULL == ( reallocate = realloc(
                                expr->global, target_size * sizeof(struct cprnths_string_t*)
                            ) )) {
                                err = cprnths_error_nomem;
                                goto CleanUpLocals;
                            }
                            expr->global = reallocate;
                        }
                        expr->global[global_used] = NULL;
                    } else {
                        free(expr->global);
                        expr->global = NULL;
                    }

                    if (local_used) {
                        target_size = local_used + 1u;
                        if (local_available != target_size) {
                            if (NULL == ( reallocate = realloc(
                                expr->local, target_size * sizeof(struct cprnths_string_t*)
                            ) )) {
                                err = cprnths_error_nomem;
                                goto CleanUpLocals;
                            }
                            expr->local = reallocate;
                        }
                        expr->local[local_used] = NULL;
                    } else {
                        free(expr->local);
                        expr->local = NULL;
                    }
                }

                *(struct cprnths_exprcls_t const **)&expr->base.cls = &cprnths_exprcls_assign;
                *expr_ = expr;
                // Already done while calling cprnths_parse_anyexpr().
                //err = 0;
                goto Finish;

CleanUpLocals:
                if (local_used) {
                    struct cprnths_string_t* *restrict local_current = expr->local + local_used;
                    do cprnths_string_destroy(*--local_current);
                    while (local_current != expr->local);
                }
            }

            free(expr->local);

CleanUpGlobals:
            if (global_used) {
                struct cprnths_string_t* *restrict global_current = expr->global + global_used;
                do cprnths_string_destroy(*--global_current);
                while (global_current != expr->global);
            }
        }

        free(expr->global);

CleanUpInnerExpr:
        cprnths_expr_destroy(expr->inner_expr);

CleanUpOuterExpr:
        free(expr);
    }

Finish:
    *current_ = current;
    return err;

#undef SKIP_SPCOMM
#undef ASSERT_NO_EOF
#undef ASSERT_CHAR
}

static
_Bool
cpintern_expr_assign_eval(
    struct cpintern_expr_assign_t const *restrict const expr,
    struct cprnths_execenv_t *restrict const env,
    struct cprnths_ref_t* *restrict const ref_
) {
    struct cprnths_ref_t *restrict ref;
    if (!(*expr->inner_expr->cls->expr_eval)(
        expr->inner_expr, env, (struct cprnths_ref_t**)&ref
    ))
        return 0;

    {
        struct cprnths_string_t* *restrict current;
        struct cprnths_dict_t *restrict target;

        if (ref == NULL) {
            if (expr->local != NULL) {
                current = expr->local;
                target = env->stack->current_frame->lsymbtab;
                do cprnths_dict_delpair(target, *current);
                while (*++current != NULL);
            }
            if (expr->global != NULL) {
                current = expr->global;
                target = env->gsymbtab;
                do cprnths_dict_delpair(target, *current);
                while (*++current != NULL);
            }
        } else {
            if (expr->local != NULL) {
                current = expr->local;
                target = env->stack->current_frame->lsymbtab;
                do {
                    if (!cprnths_dict_addpair(target, *current, ref)) {
                        cprnths_ref_increment(ref, -1);
                        return 0;
                    }
                } while (*++current != NULL);
            }
            if (expr->global != NULL) {
                current = expr->global;
                target = env->gsymbtab;
                do {
                    if (!cprnths_dict_addpair(target, *current, ref)) {
                        cprnths_ref_increment(ref, -1);
                        return 0;
                    }
                } while (*++current != NULL);
            }
        }
    }

    if (ref_ == NULL)
        cprnths_ref_increment(ref, -1);
    else
        *ref_ = ref;
    return 1;
}

static
void
cpintern_expr_assign_destroy(
    struct cpintern_expr_assign_t *restrict const e
) {
    cprnths_expr_destroy(e->inner_expr);
    struct cprnths_string_t* *restrict current;
    if (e->global != NULL) {
        current = e->global;
        do cprnths_string_destroy(*current);
        while (*++current != NULL);
    }
    if (e->local != NULL) {
        current = e->local;
        do cprnths_string_destroy(*current);
        while (*++current != NULL);
    }
}

struct cprnths_exprcls_t const cprnths_exprcls_assign = {
    (cprnths_expr_parse_t)&cpintern_expr_assign_parse,
    (cprnths_expr_eval_t)&cpintern_expr_assign_eval,
    (cprnths_expr_destroy_t)&cpintern_expr_assign_destroy
};
