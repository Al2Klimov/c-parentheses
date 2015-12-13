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


// Function definitions for execution


#include <stddef.h>
// NULL
// size_t

#include <stdbool.h>
// bool

#include <stdlib.h>
// malloc()
// free()

#include "exec.h"
// cprnths_execenv_t

#include "stack.h"
// cprnths_stack_create()
// cprnths_stack_destroy()

#include "expr.h"
// cprnths_expr_t
// cprnths_exprs_t

#include "garbage_table.h"
// cprnths_garbtab_create()
// cprnths_garbtab_destroy()

#include "dict.h"
// cprnths_dict_create()
// cprnths_dict_destroy()


struct cprnths_execenv_t*
cprnths_execenv_create(
    size_t const stack_cs,
    size_t const gsymbtab_cs,
    size_t const garbtab_cs,
    size_t const lsymbtab_cs,
    size_t const copytab_cs
) {
    struct cprnths_execenv_t *restrict e = malloc(sizeof(struct cprnths_execenv_t));
    if (e == NULL)
        goto Finish;

    e->stack = cprnths_stack_create(stack_cs, lsymbtab_cs);
    if (e->stack == NULL)
        goto FailStack;

    if (garbtab_cs) {
        e->garbtab = cprnths_garbtab_create(garbtab_cs);
        if (e->garbtab == NULL)
            goto FailGarbTab;
    } else {
        e->garbtab = NULL;
    }

    e->gsymbtab = cprnths_dict_create(gsymbtab_cs);
    if (e->gsymbtab == NULL)
        goto FailGSymbTab;

    *(size_t*)&e->copytab_chunksize = copytab_cs;
    goto Finish;

FailGSymbTab:
    if (garbtab_cs)
        cprnths_garbtab_destroy(e->garbtab);

FailGarbTab:
    cprnths_stack_destroy(e->stack);

FailStack:
    free(e);
    e = NULL;

Finish:
    return e;
}

bool
cprnths_exec(
    struct cprnths_execenv_t *restrict const env,
    struct cprnths_exprs_t const *restrict const exprs
) {
    if (!exprs->length)
        return true;

    bool success = true;
    {
        struct cprnths_expr_t const *restrict current = exprs->exprs;
        struct cprnths_expr_t const *const last = current;
        do {
            if (!(*current->cls->expr_eval)(current, env, NULL)) {
                success = false;
                break;
            }
        } while (++current < last);
    }
    return success;
}

void
cprnths_execenv_destroy(
    struct cprnths_execenv_t *restrict const e
) {
    cprnths_stack_destroy(e->stack);
    if (e->garbtab != NULL)
        cprnths_garbtab_destroy(e->garbtab);
    cprnths_dict_destroy(e->gsymbtab);
    free(e);
}
