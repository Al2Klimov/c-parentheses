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


// Typedefs and functions for stacks
#ifndef CPARENTHESES_INCLUDE_STACK
#define CPARENTHESES_INCLUDE_STACK 1


struct cprnths_stack_t;


#include <stddef.h>
// size_t

#include "dict.h"
// cprnths_dict_t

#include "reference.h"
// cprnths_ref_t

#include "error.h"
// cprnths_error_t

#include "expr.h"
// cprnths_expr_t


// A stack frame.
struct cprnths_stack_frame_t {
    // The local symbol table (not NULL)
    struct cprnths_dict_t* lsymbtab;

    // Return explicitly after processing the current expression?
    _Bool return_now;

    // The value to return or NULL
    struct cprnths_ref_t* return_val;

    // The statement to jump to after executing the current one or NULL
    struct cprnths_expr_t const *const * next_stmt;
};

// A stack.
struct cprnths_stack_t {
    // The stack itself (not NULL)
    struct cprnths_stack_frame_t* stack;

    // Amount of frames to allocate memory for at once
    // SHALL be > 0!
    size_t const frames_chunksize;

    // Amount of available frames
    // SHALL be > 0 and multiple of .frames_chunksize!
    size_t frames_total;

    // Amount of unused frames
    // SHALL be <= .frames_total!
    size_t frames_free;

    // The currently active frame or NULL
    struct cprnths_stack_frame_t* current_frame;

    // Local symbol tables' chunksize (> 0)
    size_t const lsymbtab_chunksize;
};

// Create a new stack.
cprnths_error_t
// (see error.h)
cprnths_stack_create(
    struct cprnths_stack_t**,
    // a new stack (not NULL)
    size_t,
    // .frames_chunksize (> 0)
    size_t
    // .lsymbtab_chunksize (> 0)
);

// Add a new frame to a stack.
cprnths_error_t
// (see error.h)
cprnths_stack_pushframe(
    struct cprnths_stack_t*
    // not NULL
);

// Remove the last frame from a stack.
void
cprnths_stack_popframe(
    struct cprnths_stack_t*
    // not NULL
);

// Destroy a stack.
void
cprnths_stack_destroy(
    struct cprnths_stack_t*
    // not NULL
);


#endif
