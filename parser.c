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


// Function definitions for parsing


#include <stddef.h>
// NULL
// size_t

#include <stdlib.h>
// malloc()
// realloc()
// free()

#include "error.h"
// cprnths_error_*

#include "expr.h"
// cprnths_expr_t
// cprnths_exprs_t
// cprnths_exprcls_t
// cprnths_expr_destroy()
// cprnths_exprs_destroy()


extern struct cprnths_exprcls_t const
    cprnths_exprcls_variable,
    cprnths_exprcls_assign;

static
struct cprnths_exprcls_t const *const cpintern_exprclss[] = {
    &cprnths_exprcls_variable,
    &cprnths_exprcls_assign,
    NULL
};

cprnths_error_t
cprnths_parse_anyexpr(
    char const * *restrict const current,
    char const *const end,
    struct cprnths_expr_t* *restrict const expr
) {
    cprnths_error_t err;
    {
        struct cprnths_exprcls_t const *const *restrict cls = cpintern_exprclss;
        while (cprnths_error_parse_unknown == (
            err = (*(*cls)->expr_parse)(current, end, expr)
        ) && *++cls != NULL);
    }
    return err;
}

cprnths_error_t
cprnths_parseutil_skip_spcomm(
    char const * *restrict const current,
    char const *const end
) {
    if (*current == end)
        return 0;

    char const *restrict p = *current;
    do {
        switch (*p) {
            case '/': // comment
                if (++p == end)
                    return cprnths_error_parse_eof;

                switch (*p) {
                    case '*': // C
                        while (++p != end)
                            if (*p == '*') {
                                if (++p == end)
                                    return cprnths_error_parse_eof;
                                if (*p == '/')
                                    goto CommentEnd;
                            }
                        return cprnths_error_parse_eof;
                    case '/': // BCPL
                        while (++p != end)
                            if (*p == '\n')
                                goto CommentEnd;
                        goto Success;
                }
                *current = p;
                return cprnths_error_parse_malform;
            case ' ': // whitespace
            case '\t':
            case '\r':
            case '\n':
            case '\f':
            case '\v':
                break;
            default: // anything else
                goto Success;
        }
CommentEnd:;
    } while (++p != end);

Success:
    *current = p;
    return 0;
}

cprnths_error_t
cprnths_parse_stmtexprs(
    char const * *restrict const current_,
    char const *const end,
    struct cprnths_exprs_t* *restrict const exprs_
) {
    struct cprnths_exprs_t *restrict const exprs = malloc(sizeof(struct cprnths_exprs_t));
    if (exprs == NULL)
        return cprnths_error_nomem;

    exprs->exprs = malloc(64u * sizeof(struct cprnths_expr_t*));
    if (exprs->exprs == NULL) {
        free(exprs);
        return cprnths_error_nomem;
    }

    cprnths_error_t err;

    {
        size_t used = 0u;

        {
            size_t available = 64u;
            struct cprnths_expr_t** reallocate;

            for (char const * current;;) {
                if (( err = cprnths_parseutil_skip_spcomm(current_, end) ))
                    goto Fail;

                if (*current_ == end)
                    goto ParseEnd;

                if (used == available) {
                    if (NULL == ( reallocate = realloc(
                        exprs->exprs, ( available += 64u ) * sizeof(struct cprnths_expr_t*)
                    ) )) {
                        err = cprnths_error_nomem;
                        goto Fail;
                    }
                    exprs->exprs = reallocate;
                }

                current = *current_;
                switch (( err = cprnths_parse_anyexpr(
                    &current, end, exprs->exprs + used
                ) )) {
                    case cprnths_success:
                        break;
                    case cprnths_error_parse_unknown:
                        goto ParseEnd;
                    default:
                        *current_ = current;
                        goto Fail;
                }

                if (exprs->exprs[used++]->cls != &cprnths_exprcls_assign) {
                    err = cprnths_error_parse_nostmt;
                    goto Fail;
                }
                *current_ = current;
            }

ParseEnd:
            if (used) {
                {
                    size_t const final_size = used + 1u;
                    if (available != final_size) {
                        if (NULL == ( reallocate = realloc(
                            exprs->exprs, final_size * sizeof(struct cprnths_expr_t*)
                        ) )) {
                            err = cprnths_error_nomem;
                            goto Fail;
                        }
                        exprs->exprs = reallocate;
                    }
                }

                exprs->exprs[used] = NULL;
            } else {
                free(exprs->exprs);
                exprs->exprs = NULL;
            }
        }

        *exprs_ = exprs;
        return 0;

Fail:
        if (used) {
            struct cprnths_expr_t* *restrict expr = exprs->exprs + used;
            do cprnths_expr_destroy(*--expr);
            while (expr != exprs->exprs);
        }
    }

    free(exprs->exprs);
    free(exprs);
    return err;
}

cprnths_error_t
cprnths_parse_file(
    char const * *restrict const current_,
    char const *const end,
    struct cprnths_exprs_t* *restrict const exprs_
) {
    char const *restrict current = *current_;

    // Skip #!...\n (if any)
    if (end - current >= 2 && *current == '#' && current[1] == '!') {
        current += 2u;
        while (current != end && *current++ != '\n');
    }

    cprnths_error_t err;
    {
        struct cprnths_exprs_t *restrict exprs;
        if (( err = cprnths_parse_stmtexprs(
            (char const **)&current, end, (struct cprnths_exprs_t**)&exprs
        ) ))
            goto Fail;

        if (current != end) {
            cprnths_exprs_destroy(exprs);
            err = cprnths_error_parse_unknown;
            goto Fail;
        }

        *exprs_ = exprs;
    }

    // current doesn't have to be forwarded as success implies EOF here.
    return 0;

Fail:
    *current_ = current;
    return err;
}
