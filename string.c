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


// Function definitions for strings


#include <stddef.h>
// NULL
// size_t

#include <stdlib.h>
// malloc()
// free()

#include <string.h>
// memcpy()
// memcmp()

#include "string.h"
// cprnths_string_t

#include "error.h"
// cprnths_error_*


cprnths_error_t
cprnths_string_create(
    struct cprnths_string_t* *restrict const s_,
    char const *restrict const S,
    size_t const l
) {
    struct cprnths_string_t *restrict const s = malloc(sizeof(struct cprnths_string_t));
    if (s == NULL)
        return cprnths_error_nomem;

    if (NULL == ( s->str = malloc(l + 1u) )) {
        free(s);
        return cprnths_error_nomem;
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

    *s_ = s;
    return 0;
}

cprnths_error_t
cprnths_string_copy(
    struct cprnths_string_t const *restrict const S,
    struct cprnths_string_t* *restrict const s_
) {
    struct cprnths_string_t *restrict const s = malloc(sizeof(struct cprnths_string_t));
    if (s == NULL)
        return cprnths_error_nomem;

    if (NULL == ( s->str = malloc(S->length + 1u) )) {
        free(s);
        return cprnths_error_nomem;
    }

    s->length = S->length;
    s->hash = S->hash;
    memcpy((char*)s->str, S->str, S->length + 1u);

    *s_ = s;
    return 0;
}

_Bool
cprnths_string_equal(
    struct cprnths_string_t const *restrict const a,
    struct cprnths_string_t const *restrict const b
) {
    return a->length == b->length && a->hash == b->hash && !memcmp(a->str, b->str, a->length);
}

void
cprnths_string_destroy(
    struct cprnths_string_t *restrict const s
) {
    free((char*)s->str);
    free(s);
}
