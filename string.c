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


// Function definitions for strings


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
// memcmp()

#include "string.h"
// cprnths_string_t


cprnths_string_t* cprnths_string_create(char const *restrict const S, size_t const l) {
    cprnths_string_t *restrict s = malloc(sizeof(cprnths_string_t));

    if (s == NULL)
        goto Finish;

    if (NULL == ( s->str = malloc(l + 1u) )) {
        free(s);
        s = NULL;
        goto Finish;
    }

    s->length = l;
    s->hash = 0u;
    ((char*)s->str)[l] = 0;
    if (l) {
        memcpy((char*)s->str, S, l);

        // TODO: replace this with a "real" hash algo
        unsigned char const *restrict p = (unsigned char const *)S;
        unsigned char const *const limit = p + l;
        do s->hash += *p;
        while (++p < limit);
    }

Finish:
    return s;
}

cprnths_string_t* cprnths_string_copy(cprnths_string_t const *restrict const S) {
    cprnths_string_t *restrict s = malloc(sizeof(cprnths_string_t));

    if (s == NULL)
        goto Finish;

    if (NULL == ( s->str = malloc(S->length + 1u) )) {
        free(s);
        s = NULL;
        goto Finish;
    }

    s->length = S->length;
    s->hash = S->hash;
    memcpy((char*)s->str, S->str, S->length + 1u);

Finish:
    return s;
}

bool cprnths_string_equal(cprnths_string_t const *restrict const a, cprnths_string_t const *restrict const b) {
    return a->length == b->length && a->hash == b->hash && !memcmp(a->str, b->str, a->length);
}

void cprnths_string_destruct(cprnths_string_t *restrict const s) {
    free((char*)s->str);
    free(s);
}
