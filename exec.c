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


// Function definitions for execution


#include <stddef.h>
// NULL
// size_t

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
// cprnths_garbtab_cleanup()
// cprnths_garbtab_destroy()

#include "dict.h"
// cprnths_dict_create()
// cprnths_dict_destroy()

#include "error.h"
// cprnths_error_*


cprnths_error_t
cprnths_execenv_create(
    struct cprnths_execenv_t* *restrict const env_,
    size_t const stack_cs,
    size_t const gsymbtab_cs,
    size_t const garbtab_cs,
    size_t const lsymbtab_cs,
    size_t const copytab_cs
) {
    cprnths_error_t err;

    {
        struct cprnths_execenv_t *restrict const env = malloc(sizeof(struct cprnths_execenv_t));
        if (env == NULL) {
            err = cprnths_error_nomem;
            goto Finish;
        }

        if (( err = cprnths_stack_create(&env->stack, stack_cs, lsymbtab_cs) ))
            goto FailStack;

        if (garbtab_cs) {
            if (( err = cprnths_garbtab_create(&env->garbtab, garbtab_cs) ))
                goto FailGarbTab;
        } else {
            env->garbtab = NULL;
        }

        if (( err = cprnths_dict_create(&env->gsymbtab, gsymbtab_cs) ))
            goto FailGSymbTab;

        *(size_t*)&env->copytab_chunksize = copytab_cs;
        *env_ = env;
        // Already done implicitly.
        //err = 0;
        goto Finish;

FailGSymbTab:
        if (garbtab_cs)
            cprnths_garbtab_destroy(env->garbtab);

FailGarbTab:
        cprnths_stack_destroy(env->stack);

FailStack:
        free(env);
    }

Finish:
    return err;
}

cprnths_error_t
cprnths_exec(
    struct cprnths_execenv_t *restrict const env,
    struct cprnths_exprs_t const *restrict const exprs
) {
    if (exprs->exprs == NULL)
        return 0;

    cprnths_error_t err;
    {
        struct cprnths_expr_t const *const *restrict current = (struct cprnths_expr_t const *const *)exprs->exprs;
        while (!(
            err = (*(*current)->cls->expr_eval)(*current, env, NULL)
        ) && *++current != NULL);
    }
    return err;
}

void
cprnths_execenv_destroy(
    struct cprnths_execenv_t *restrict const e
) {
    cprnths_stack_destroy(e->stack);
    cprnths_dict_destroy(e->gsymbtab);
    if (e->garbtab != NULL) {
        cprnths_garbtab_cleanup(e->garbtab);
        cprnths_garbtab_destroy(e->garbtab);
    }
    free(e);
}
