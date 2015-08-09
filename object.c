/* C() / "C Parentheses"
 * A pure functional programming language
 * Reference implementation
 *
 * Copyright (C) 2015  Alexander A. Klimov
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


// Function definitions for objects


#include <stddef.h>
// NULL
// size_t

#include <stdbool.h>
// bool

#include <stdlib.h>
// malloc()
// free()

#include <string.h>
// memcpy()

#include "object.h"
// cprnths_*_t


void cprnths_obj_destruct(cprnths_obj_t *restrict const o) {
    if (o->obj_destruct != NULL)
        (*o->obj_destruct)(o);

    free(o);
}

cprnths_obj_t* cprnths_obj_copy(cprnths_obj_t *restrict const o) {
    cprnths_obj_t *restrict c = malloc(o->obj_size);

    if (c == NULL)
        goto Finish;

    memcpy(c, o, o->obj_size);

    if (!(o->obj_copy == NULL || (*o->obj_copy)(o, c))) {
        free(c);
        c = NULL;
    }

Finish:
    return c;
}
