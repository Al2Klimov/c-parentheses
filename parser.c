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

#include "error.h"
// cprnths_error_*

#include "expr.h"
// cprnths_expr_t
// cprnths_exprcls_t


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
