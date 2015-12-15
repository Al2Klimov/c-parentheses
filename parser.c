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


// Function definitions for parsing


#include "parser.h"
// cprnths_parse_stat_t
// cprnths_parse_success
// cprnths_parse_malform
// cprnths_parse_eof

#include "expr.h"
// cprnths_expr_t
// cprnths_exprcls_t


extern struct cprnths_exprcls_t const cprnths_exprcls_variable;

cprnths_parse_stat_t
cprnths_parse_anyexpr(
    char const * *restrict const current,
    char const *const end,
    struct cprnths_expr_t* *restrict const expr
) {
    // TODO: Once there will be more expressions, this simple line MUST be replaced
    // with a loop over all available cprnths_exprcls_t. If a .expr_parse() returns
    // cprnths_parse_unknown, call the next one (if any) instead of returning.
    return (*cprnths_exprcls_variable.expr_parse)(current, end, expr);
}

cprnths_parse_stat_t
cprnths_parseutil_skip_spcomm(
    char const * *restrict const current,
    char const *const end
) {
    if (*current == end)
        return cprnths_parse_success;

    char const *restrict p = *current;
    do {
        switch (*p) {
            case '/': // comment
                if (++p == end)
                    return cprnths_parse_eof;

                switch (*p) {
                    case '*': // C
                        while (++p != end)
                            if (*p == '*') {
                                if (++p == end)
                                    return cprnths_parse_eof;
                                if (*p == '/')
                                    goto CommentEnd;
                            }
                        return cprnths_parse_eof;
                    case '/': // BCPL
                        while (++p != end)
                            if (*p == '\n')
                                goto CommentEnd;
                        goto Success;
                }
                *current = p;
                return cprnths_parse_malform;
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
    return cprnths_parse_success;
}
