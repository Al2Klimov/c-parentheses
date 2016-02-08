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
// cprnths_jmptab_row_t

#include "parser.h"
// cprnths_jmptab_prep_t
// cprnths_jmptab_prep_row_t
// cprnths_parseutil_startswith_word()

#include "string.h"
// cprnths_string_t
// cprnths_string_create()
// cprnths_string_create_customds()


extern struct cprnths_exprcls_t const
    cprnths_exprcls_variable,
    cprnths_exprcls_string,
    cprnths_exprcls_assign,
    cprnths_exprcls_jlabel,
    cprnths_exprcls_bool;

static
struct cprnths_exprcls_t const *const cpintern_exprclss[] = {
    &cprnths_exprcls_variable,
    &cprnths_exprcls_string,
    &cprnths_exprcls_assign,
    &cprnths_exprcls_jlabel,
    &cprnths_exprcls_bool,
    NULL
};

static
struct cprnths_exprcls_t const *const cpintern_stmtexprclss[] = {
    &cprnths_exprcls_assign,
    &cprnths_exprcls_jlabel,
    NULL
};

cprnths_error_t
cprnths_parse_anyexpr(
    char const * *restrict const current,
    char const *const end,
    struct cprnths_expr_t* *restrict const expr,
    struct cprnths_jmptab_prep_t *restrict const jmptab_prep
) {
    cprnths_error_t err;
    {
        struct cprnths_exprcls_t const *const *restrict cls = cpintern_exprclss;
        while (cprnths_error_parse_unknown == (
            err = (*(*cls)->expr_parse)(current, end, expr, jmptab_prep)
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
    cprnths_error_t err;

    struct cprnths_exprs_t *restrict const exprs = malloc(sizeof(struct cprnths_exprs_t));
    if (exprs == NULL) {
        err = cprnths_error_nomem;
        goto Finish;
    }

    exprs->exprs = malloc(64u * sizeof(struct cprnths_expr_t*));
    if (exprs->exprs == NULL) {
        err = cprnths_error_nomem;
        goto CleanUpExprs;
    }

    {
        struct cprnths_jmptab_prep_t jmptab_prep;

        jmptab_prep.jmptab_prep = malloc(16u * sizeof(struct cprnths_jmptab_prep_row_t));
        if (jmptab_prep.jmptab_prep == NULL) {
            err = cprnths_error_nomem;
            goto CleanUpExprsExprs;
        }

        jmptab_prep.available = 16u;
        jmptab_prep.used = jmptab_prep.current_stmt_offset = 0u;

        {
            size_t used = 0u;

            {
                size_t available = 64u;
                struct cprnths_expr_t** reallocate;

                for (char const * current;; ++jmptab_prep.current_stmt_offset) {
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
                        &current, end, exprs->exprs + used, &jmptab_prep
                    ) )) {
                        case cprnths_success:
                            break;
                        case cprnths_error_parse_unknown:
                            goto ParseEnd;
                        default:
                            *current_ = current;
                            goto Fail;
                    }

                    {
                        struct cprnths_exprcls_t const *const cls = exprs->exprs[used++]->cls;
                        struct cprnths_exprcls_t const *const *restrict clsp = cpintern_stmtexprclss;
                        for (;;) {
                            if (cls == *clsp)
                                break;
                            if (++clsp == NULL) {
                                err = cprnths_error_parse_nostmt;
                                goto Fail;
                            }
                        }
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

            if (jmptab_prep.used) {
                if (NULL == ( exprs->jmptab = malloc(
                    (jmptab_prep.used + 1u) * sizeof(struct cprnths_jmptab_row_t)
                ) )) {
                    err = cprnths_error_nomem;
                    goto Fail;
                }

                struct cprnths_jmptab_row_t *restrict current = exprs->jmptab;
                {
                    struct cprnths_jmptab_row_t *const limit = current + jmptab_prep.used;
                    struct cprnths_jmptab_prep_row_t const *restrict current_prep = jmptab_prep.jmptab_prep;
                    do {
                        current->label = current_prep->label;
                        current->stmt = (struct cprnths_expr_t const *const *)exprs->exprs + current_prep++->stmt_offset;
                    } while (++current < limit);
                }
                current->label = NULL;
                current->stmt = NULL;
            } else {
                exprs->jmptab = NULL;
            }

            *exprs_ = exprs;
            err = 0;
            goto CleanUpJmpTabPrep;

Fail:
            if (used) {
                struct cprnths_expr_t* *restrict expr = exprs->exprs + used;
                do cprnths_expr_destroy(*--expr);
                while (expr != exprs->exprs);
            }
        }

CleanUpJmpTabPrep:
        free(jmptab_prep.jmptab_prep);
    }

    if (!err)
        goto Finish;

CleanUpExprsExprs:
    free(exprs->exprs);

CleanUpExprs:
    free(exprs);

Finish:
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

static
void
cpintern_parseutil_string_ds(
    void *const src_,
    char *restrict tgt,
    size_t const length
) {
    char const *restrict src = src_;
    char *const stop = tgt + length;
    do {
        *tgt++ = *src;
        if (*src == '\'')
            src += 2u;
        else
            ++src;
    } while (tgt < stop);
}

cprnths_error_t
cprnths_parseutil_string(
    char const * *restrict const current_,
    char const *const end,
    struct cprnths_string_t* *restrict const target
) {
    if (*current_ == end || **current_ != '\'')
        return cprnths_error_parse_unknown;

    {
        char const *restrict current = *current_ + 1u;
        char const *const start = current;
        size_t quote_chars = 0u;

        for (;; ++current) {
            if (current == end)
                return cprnths_error_parse_eof;

            if (*current == '\'') {
                if (++current == end || *current != '\'')
                    break;
                else
                    ++quote_chars;
            }
        }
        *current_ = current;

        size_t const length = current - (start + quote_chars + 1u);
        if (length)
            return cprnths_string_create_customds(target, length, &cpintern_parseutil_string_ds, (void*)start);
    }

    return cprnths_string_create(target, NULL, 0u);
}

cprnths_error_t
cprnths_parseutil_funccall_start(
    char const * *restrict const current_,
    char const *const end,
    char const *restrict const func_name
) {
    char const *restrict current = *current_;

    switch (cprnths_parseutil_startswith_word((char const **)&current, end, func_name)) {
        case -1:
        case 0:
            return cprnths_error_parse_unknown;
        case 2:
            return cprnths_error_parse_eof;
    }

    cprnths_error_t err;

    if (*current != '(') {
        if (( err = cprnths_parseutil_skip_spcomm((char const **)&current, end) ))
            goto Finish;
        if (current == end)
            return cprnths_error_parse_eof;

        if (*current != '(') {
            err = cprnths_error_parse_malform;
            goto Finish;
        }
    }
    ++current;

    if (( err = cprnths_parseutil_skip_spcomm((char const **)&current, end) ))
        goto Finish;

    if (current == end)
        return cprnths_error_parse_eof;

    // Already done while calling cprnths_parseutil_skip_spcomm().
    //err = 0;

Finish:
    *current_ = current;
    return err;
}

cprnths_error_t
cprnths_parseutil_funccall_end(
    char const * *restrict const current_,
    char const *const end
) {
    char const *restrict current = *current_;
    cprnths_error_t err;

    if (current == end)
        return cprnths_error_parse_eof;

    if (*current == ')') {
        err = 0;
    } else {
        if (( err = cprnths_parseutil_skip_spcomm((char const **)&current, end) ))
            goto Finish;
        if (current == end)
            return cprnths_error_parse_eof;

        if (*current != ')') {
            err = cprnths_error_parse_malform;
            goto Finish;
        }
    }
    ++current;

Finish:
    *current_ = current;
    return err;
}
