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


// Function definitions for stacks


#include <stddef.h>
// size_t
// NULL

#include <stdlib.h>
// malloc()
// realloc()
// free()

#include "stack.h"
// cprnths_stack_t
// cprnths_stack_frame_t

#include "dict.h"
// cprnths_dict_t
// cprnths_dict_create()
// cprnths_dict_destroy()

#include "reference.h"
// cprnths_ref_increment()

#include "error.h"
// cprnths_error_*


cprnths_error_t
cprnths_stack_create(
    struct cprnths_stack_t* *restrict const s_,
    size_t const frames_cs,
    size_t const lsymbtab_cs
) {
    struct cprnths_stack_t *restrict const s = malloc(sizeof(struct cprnths_stack_t));
    if (s == NULL)
        return cprnths_error_nomem;

    s->stack = malloc(frames_cs * sizeof(struct cprnths_stack_frame_t));
    if (s->stack == NULL) {
        free(s);
        return cprnths_error_nomem;
    }

    *(size_t*)&s->frames_chunksize = s->frames_total = s->frames_free = frames_cs;
    *(size_t*)&s->lsymbtab_chunksize = lsymbtab_cs;
    s->current_frame = NULL;

    *s_ = s;
    return 0;
}

cprnths_error_t
cprnths_stack_pushframe(
    struct cprnths_stack_t *restrict const s
) {
    if (!s->frames_free) {
        size_t const frames_total = s->frames_total + s->frames_chunksize;
        {
            struct cprnths_stack_frame_t *restrict const S = realloc(
                s->stack,
                frames_total * sizeof(struct cprnths_stack_frame_t)
            );
            if (S == NULL)
                return cprnths_error_nomem;

            s->frames_free += s->frames_chunksize;
            if (s->stack != S) {
                s->stack = S;
                if (s->current_frame != NULL)
                    s->current_frame = s->stack + (frames_total - s->frames_free - 1u);
            }
        }
        s->frames_total = frames_total;
    }

    {
        struct cprnths_dict_t *restrict lsymbtab;
        {
            cprnths_error_t const err = cprnths_dict_create(
                (struct cprnths_dict_t**)&lsymbtab, s->lsymbtab_chunksize
            );
            if (err)
                return err;
        }

        --s->frames_free;
        if (s->current_frame == NULL)
            s->current_frame = s->stack;
        else
            ++s->current_frame;

        s->current_frame->lsymbtab = lsymbtab;
    }

    s->current_frame->return_now = 0;
    s->current_frame->return_val = NULL;

    return 0;
}

void
cprnths_stack_popframe(
    struct cprnths_stack_t *restrict const s
) {
    cprnths_dict_destroy(s->current_frame->lsymbtab);
    if (s->current_frame->return_val != NULL)
        cprnths_ref_increment(s->current_frame->return_val, -1);

    if (s->current_frame == s->stack)
        s->current_frame = NULL;
    else
        --s->current_frame;

    if (++s->frames_free / s->frames_chunksize < 2)
        return;

    size_t const frames_total = s->frames_total - s->frames_chunksize;
    {
        struct cprnths_stack_frame_t *restrict const S = realloc(
            s->stack,
            frames_total * sizeof(struct cprnths_stack_frame_t)
        );
        if (S == NULL)
            return;

        s->frames_free -= s->frames_chunksize;
        if (s->stack != S) {
            s->stack = S;
            if (s->current_frame != NULL)
                s->current_frame = s->stack + (frames_total - s->frames_free - 1u);
        }
    }
    s->frames_total = frames_total;
}

void
cprnths_stack_destroy(
    struct cprnths_stack_t *restrict const s
) {
    if (s->current_frame != NULL) {
        struct cprnths_stack_frame_t *restrict f = s->current_frame;
        do {
            cprnths_dict_destroy(f->lsymbtab);
            if (f->return_val != NULL)
                cprnths_ref_increment(f->return_val, -1);
        } while (f-- != s->stack);
    }

    free(s->stack);
    free(s);
}
