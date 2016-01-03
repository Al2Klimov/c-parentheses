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


// Function definitions for copy tables


#include <stddef.h>
// NULL
// size_t

#include <stdlib.h>
// malloc()
// realloc()
// free()

#include "copy_table.h"
// cprnths_copytab_t
// cprnths_copytab_row_t

#include "reference.h"
// cprnths_ref_t


struct cprnths_copytab_t*
cprnths_copytab_create(
    size_t const s
) {
    struct cprnths_copytab_t *restrict t = malloc(sizeof(struct cprnths_copytab_t));

    if (t == NULL)
        goto Finish;

    t->tab = malloc(s * sizeof(struct cprnths_copytab_row_t));

    if (t->tab == NULL) {
        free(t);
        t = NULL;
        goto Finish;
    }

    {
        struct cprnths_copytab_row_t *restrict p = t->tab + s;
        do {
            (--p)->orig = NULL;
            p->copy = NULL;
        } while (p > t->tab);
    }

    *(size_t*)&t->chunksize = t->slots_total = t->slots_free = s;

Finish:
    return t;
}

_Bool
cprnths_copytab_addrefs(
    struct cprnths_copytab_t *restrict const t,
    struct cprnths_ref_t const *const o,
    struct cprnths_ref_t *const c
) {
    if (t->slots_free) {
        struct cprnths_copytab_row_t *restrict T = t->tab;
        while (T->orig != NULL)
            ++T;
        T->orig = o;
        T->copy = c;
        --t->slots_free;
    } else {
        {
            size_t const slots_total = t->slots_total + t->chunksize;

            {
                struct cprnths_copytab_row_t *restrict T = realloc(
                    t->tab,
                    slots_total * sizeof(struct cprnths_copytab_row_t)
                );
                if (T == NULL)
                    return 0;

                t->tab = T;
                T += t->slots_total;
                T->orig = o;
                T->copy = c;

                struct cprnths_copytab_row_t *const limit = t->tab + slots_total;
                while (++T < limit)
                    T->orig = T->copy = NULL;
            }

            t->slots_total = slots_total;
        }

        t->slots_free += t->chunksize - 1u;
    }

    return 1;
}

struct cprnths_ref_t*
cprnths_copytab_chkref(
    struct cprnths_copytab_t const *restrict const t,
    struct cprnths_ref_t const *const r
) {
    if (t->slots_total == t->slots_free)
        return NULL;

    struct cprnths_copytab_row_t const *restrict p = t->tab;
    struct cprnths_copytab_row_t const *const limit = t->tab + (t->slots_total - t->slots_free);

    do {
        if (p->orig == r)
            return p->copy;
    } while (++p < limit);

    return NULL;
}

void
cprnths_copytab_destroy(
    struct cprnths_copytab_t *restrict const t
) {
    free(t->tab);
    free(t);
}
