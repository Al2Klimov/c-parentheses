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


void cprnths_obj_destruct(cprnths_obj_t* o) {
    {
        cprnths_objdestruct_t f = o->obj_destruct;
        if (f != NULL)
            (*f)(o);
    }

    free(o);
}

cprnths_obj_t* cprnths_obj_copy(cprnths_obj_t* o) {
    cprnths_obj_t* c;

    {
        size_t s = o->obj_size;
        if (( c = malloc(s) ) == NULL)
            goto Finish;
        memcpy(c, o, s);
    }

    {
        cprnths_objcopy_t f = o->obj_copy;
        if (f == NULL || (*f)(o, c))
            goto Finish;
    }

    free(c);
    c = NULL;

Finish:
    return c;
}
