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


// Function definitions for objects


#include <stddef.h>
// NULL
// size_t

#include <stdlib.h>
// malloc()
// free()

#include <string.h>
// memcpy()

#include "object.h"
// cprnths_*_t

#include "error.h"
// cprnths_error_*


void
cprnths_obj_destroy(
    struct cprnths_obj_t *restrict const o
) {
    if (o->cls->obj_destroy != NULL)
        (*o->cls->obj_destroy)(o);

    free(o);
}

cprnths_error_t
cprnths_obj_copy(
    struct cprnths_obj_t const *restrict const o,
    struct cprnths_obj_t* *restrict const c_,
    struct cprnths_copytab_t *restrict const t
) {
    struct cprnths_obj_t *restrict const c = malloc(o->cls->obj_size);
    if (c == NULL)
        return cprnths_error_nomem;

    memcpy(c, o, o->cls->obj_size);
    if (o->cls->obj_copy != NULL) {
        cprnths_error_t const err = (*o->cls->obj_copy)(o, c, t);
        if (err) {
            free(c);
            return err;
        }
    }

    *c_ = c;
    return 0;
}
