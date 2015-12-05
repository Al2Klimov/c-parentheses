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


// Typedefs and functions for execution
#ifndef CPARENTHESES_INCLUDE_EXEC
#define CPARENTHESES_INCLUDE_EXEC 1


struct cprnths_execenv_t;


#include <stddef.h>
// size_t

#include <stdbool.h>
// bool

#include "dict.h"
// cprnths_dict_t

#include "expr.h"
// cprnths_exprs_t

#include "reference.h"
// cprnths_ref_t

#include "garbage_table.h"
// cprnths_garbtab_t


// A stack frame.
struct cprnths_stackframe_t {
    // The local symbol table (not NULL)
    struct cprnths_dict_t* lsymbtab;

    // Return explicitly after processing the current expression?
    bool return_now;

    // The value to return or NULL
    struct cprnths_ref_t* return_val;
};

// This contains all information about the current execution state.
struct cprnths_execenv_t {
    // The stack (not NULL)
    struct cprnths_stackframe_t* stack;

    // Amount of stack frames to allocate memory for at once
    // SHALL be > 0!
    size_t const frames_chunksize;

    // Amount of available stack frames
    // SHALL be > 0 and multiple of .frames_chunksize!
    size_t frames_total;

    // Amount of unused stack frames
    // SHALL be <= .frames_total!
    size_t frames_free;

    // The currently active stack frame or NULL
    struct cprnths_stackframe_t* current_frame;

    // The garbage table or NULL
    struct cprnths_garbtab_t* garbtab;

    // The global symbol table (not NULL)
    struct cprnths_dict_t* gsymbtab;

    // Local symbol tables' chunksize (> 0)
    size_t const lsymbtab_chunksize;

    // Copy tables' chunksize (> 0)
    size_t const copytab_chunksize;
};


// Create a new execution environment.
struct cprnths_execenv_t*
// a new environment or NULL
cprnths_execenv_create(
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
bool
// was the execution successful?
cprnths_exec(
    struct cprnths_execenv_t*,
    // not NULL
    struct cprnths_exprs_t const *
    // not NULL
);

// Add a new frame to an execution environment's stack.
bool
// was the addition successful?
cprnths_execenv_pushframe(
    struct cprnths_execenv_t*
    // not NULL
);

// Remove the last frame from an execution environment's stack.
void
cprnths_execenv_popframe(
    struct cprnths_execenv_t*
    // not NULL
);

// Destroy an execution environment.
void
cprnths_execenv_destroy(
    struct cprnths_execenv_t*
    // not NULL
);


#endif
