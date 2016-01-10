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


// The interpreter


#include <stddef.h>
// NULL
// size_t

#include <stdlib.h>
// _Exit
// EXIT_*
// free()

#include <stdio.h>
// stderr
// vfprintf()
// sprintf()

#include <stdarg.h>
// va_*

#include "error.h"
// cprnths_error_*

#include "io.h"
// cprnths_fcat()

#include "parser.h"
// cprnths_parse_file()

#include "expr.h"
// cprnths_exprs_t

#include "exec.h"
// cprnths_execenv_t
// cprnths_execenv_create()
// cprnths_exec()

#include "stack.h"
// cprnths_stack_pushframe()


static void panicf(char const *, ...);

int main(int const argc, char* *const argv) {
    char const * fname;
    switch (argc) {
        case 1:
            fname = NULL;
            break;
        case 2:
            if (argv[1][0] == '-') {
                if (argv[1][1] == 0)
                    fname = NULL;
                else
                    panicf("Invalid argument: `%s'\n", argv[1]);
            } else {
                fname = argv[1];
            }
            break;
        default:
            panicf("Too many arguments (%d)\n", argc - 1);
    }

    char* content;
    size_t length;
    switch (cprnths_fcat(fname, &content, &length)) {
        case 0:
            break;
        case cprnths_error_nomem:
            goto NoMem;
        case cprnths_error_io_open:
            panicf("Couldn't open `%s' for reading\n", fname);
        default:
            panicf("Couldn't read `%s'\n", fname);
    }

    char const * current = content;
    struct cprnths_exprs_t* exprs;
    cprnths_error_t const err = cprnths_parse_file(&current, content + length, &exprs);
    switch (err) {
        case 0:
            break;
        case cprnths_error_nomem:
            goto NoMem;
        default: {
            char const * msg;
            switch (err) {
                case cprnths_error_parse_eof:
                    msg = "unexpected end of file";
                    current = content + length;
                    break;
                case cprnths_error_parse_nostmt:
                    msg = "unexpected non-statement expression";
                    break;
                default:
                    msg = "invalid syntax";
            }

            char code_near_err[513];
            char* target_pos = code_near_err;
            for (
                char const * c = current - content > 32 ? current - 32 : content;
                c != current;
                ++c
            ) {
                if (31 < *c && *c < 127) {
                    switch (*c) {
                        case '\\':
                        case '"':
                            *target_pos++ = '\\';
                        default:
                            *target_pos++ = *c;
                    }
                } else {
                    sprintf(target_pos, "\\x%02hhX", *(unsigned char const *)c);
                    target_pos += 4;
                }
            }
            *target_pos = 0;

            panicf("Parsing error: %s near \"%s\"\n", msg, code_near_err);
        }
    }

    free(content);

    struct cprnths_execenv_t* env;
    if (!(
        cprnths_execenv_create(&env, 16u, 16u, 0u, 16u, 16u)
        || cprnths_stack_pushframe(env->stack)
        || cprnths_exec(env, exprs)
    ))
        return EXIT_SUCCESS;

NoMem:
    panicf("Fatal error: out of memory\n");
}

static void panicf(char const *const format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    _Exit(EXIT_FAILURE);
}
