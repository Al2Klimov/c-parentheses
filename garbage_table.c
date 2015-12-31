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


// Function definitions for garbage tables


#include <stddef.h>
// NULL
// size_t

#include <stdlib.h>
// malloc()
// realloc()
// free()

#include "garbage_table.h"
// cprnths_garbtab_t

#include "reference.h"
// cprnths_ref_t

#include "object.h"
// cprnths_obj_t
// cprnths_obj_destroy()


struct cprnths_garbtab_t*
cprnths_garbtab_create(
    size_t const s
) {
    struct cprnths_garbtab_t *restrict t = malloc(sizeof(struct cprnths_garbtab_t));

    if (t == NULL)
        goto Finish;

    t->tab = malloc(s * sizeof(struct cprnths_ref_t*));

    if (t->tab == NULL) {
        free(t);
        t = NULL;
        goto Finish;
    }

    {
        struct cprnths_ref_t* *restrict p = t->tab + s;
        do *--p = NULL;
        while (p > t->tab);
    }

    *(size_t*)&t->chunksize = t->slots_total = t->slots_free = s;

Finish:
    return t;
}

_Bool
cprnths_garbtab_addref(
    struct cprnths_garbtab_t *restrict const t,
    struct cprnths_ref_t *const r
) {
    if (t->slots_free) {
        struct cprnths_ref_t* *restrict T = t->tab;
        while (*T != NULL)
            ++T;
        *T = r;
        --t->slots_free;
    } else {
        {
            size_t const slots_total = t->slots_total + t->chunksize;

            {
                struct cprnths_ref_t* *restrict T = realloc(
                    t->tab,
                    slots_total * sizeof(struct cprnths_ref_t*)
                );
                if (T == NULL)
                    return 0;

                t->tab = T;
                T += t->slots_total;
                *T = r;

                struct cprnths_ref_t* *const limit = t->tab + slots_total;
                while (++T < limit)
                    *T = NULL;
            }

            t->slots_total = slots_total;
        }

        t->slots_free += t->chunksize - 1u;
    }

    return 1;
}

void
cprnths_garbtab_delref(
    struct cprnths_garbtab_t *restrict const t,
    struct cprnths_ref_t *const r
) {
    {
        struct cprnths_ref_t* *restrict p = t->tab + t->slots_total;
        do {
            if (*--p == r) {
                *p = NULL;
                ++t->slots_free;
                goto Defrag;
            }
        } while (p > t->tab);
    }

    return;

Defrag:
    if (t->slots_free / t->chunksize < 2u)
        return;

    {
        struct cprnths_ref_t* *restrict free_slot = t->tab;
        struct cprnths_ref_t* *restrict used_slot = t->tab + t->slots_total;
        struct cprnths_ref_t* *const last_used_slot = used_slot - t->chunksize;
        do {
            if (*--used_slot != NULL) {
                while (*free_slot != NULL)
                    ++free_slot;
                *free_slot = *used_slot;
                *used_slot = NULL;
            }
        } while (used_slot > last_used_slot);
    }

    size_t const slots_total = t->slots_total - t->chunksize;
    struct cprnths_ref_t* *restrict const T = realloc(
        t->tab,
        slots_total * sizeof(struct cprnths_ref_t*)
    );
    if (T != NULL) {
        t->tab = T;
        t->slots_total = slots_total;
        t->slots_free -= t->chunksize;
    }
}

void
cprnths_garbtab_cleanup(
    struct cprnths_garbtab_t *const t
) {
    struct cprnths_ref_t* *restrict p;
    struct cprnths_ref_t** limit;
    struct cprnths_obj_t *restrict o;

Search:
    if (t->slots_total == t->slots_free)
        return;

    limit = (p = t->tab) + t->slots_total;
    do {
        if (!(*p == NULL || (*p)->obj == NULL)) {
            o = (*p)->obj;
            (*p)->obj = NULL;
            cprnths_obj_destroy(o);
            goto Search;
        }
    } while (++p < limit);
}

void
cprnths_garbtab_destroy(
    struct cprnths_garbtab_t *restrict const t
) {
    if (t->slots_total != t->slots_free) {
        struct cprnths_ref_t* *restrict p = t->tab + t->slots_total;
        do {
            if (*--p != NULL)
                (*p)->garbtab = NULL;
        } while (p > t->tab);
    }

    free(t->tab);
    free(t);
}
