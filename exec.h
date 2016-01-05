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


// Typedefs and functions for execution
#ifndef CPARENTHESES_INCLUDE_EXEC
#define CPARENTHESES_INCLUDE_EXEC 1


struct cprnths_execenv_t;


#include <stddef.h>
// size_t

#include "dict.h"
// cprnths_dict_t

#include "expr.h"
// cprnths_exprs_t

#include "stack.h"
// cprnths_stack_t

#include "garbage_table.h"
// cprnths_garbtab_t

#include "error.h"
// cprnths_error_t


// This contains all information about the current execution state.
struct cprnths_execenv_t {
    // The stack (not NULL)
    struct cprnths_stack_t* stack;

    // The garbage table or NULL
    struct cprnths_garbtab_t* garbtab;

    // The global symbol table (not NULL)
    struct cprnths_dict_t* gsymbtab;

    // Copy tables' chunksize (> 0)
    size_t const copytab_chunksize;
};


// Create a new execution environment.
cprnths_error_t
// (see error.h)
cprnths_execenv_create(
    struct cprnths_execenv_t**,
    // a new environment (not NULL)
    size_t,
    // stack's chunksize (> 0)
    size_t,
    // global symbol table's chunksize (> 0)
    size_t,
    // garbage table's chunksize or 0 to disable garbage tables
    size_t,
    // local symbol tables' chunksize (> 0)
    size_t
    // copy tables' chunksize (> 0)
);

// Execute expressions in an execution environment.
cprnths_error_t
// (see error.h)
cprnths_exec(
    struct cprnths_execenv_t*,
    // not NULL
    struct cprnths_exprs_t const *
    // not NULL
);

// Destroy an execution environment.
void
cprnths_execenv_destroy(
    struct cprnths_execenv_t*
    // not NULL
);


#endif
