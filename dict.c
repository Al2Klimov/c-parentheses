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


// Function definitions for dictionaries


#include <stddef.h>
// NULL
// size_t

#include <stdlib.h>
// malloc()
// realloc()
// free()

#include "dict.h"
// cprnths_dict_t
// cprnths_dict_pair_t

#include "string.h"
// cprnths_string_t
// cprnths_string_equal()
// cprnths_string_copy()
// cprnths_string_destroy()

#include "reference.h"
// cprnths_ref_t
// cprnths_ref_increment()


struct cprnths_dict_t*
cprnths_dict_create(
    size_t const s
) {
    struct cprnths_dict_t *restrict d = malloc(sizeof(struct cprnths_dict_t));

    if (d == NULL)
        goto Finish;

    d->dict = malloc(s * sizeof(struct cprnths_dict_pair_t));

    if (d->dict == NULL) {
        free(d);
        d = NULL;
        goto Finish;
    }

    {
        struct cprnths_dict_pair_t *restrict p = d->dict + s;
        do {
            (--p)->key = NULL;
            p->value = NULL;
        } while (p > d->dict);
    }

    *(size_t*)&d->chunksize = d->slots_total = d->slots_free = s;

Finish:
    return d;
}

_Bool
cprnths_dict_addpair(
    struct cprnths_dict_t *restrict const d,
    struct cprnths_string_t const *restrict k,
    struct cprnths_ref_t *restrict const v
) {
    if (d->slots_total == d->slots_free) {
        if (NULL == ( k = cprnths_string_copy(k) ))
            return 0;

        d->dict->key = (struct cprnths_string_t*)k;
        cprnths_ref_increment(v, 1);
        d->dict->value = v;
        --d->slots_free;
        return 1;
    }

    if (d->slots_free)
        for (struct cprnths_dict_pair_t *restrict D = d->dict;; ++D) {
            if (D->key == NULL) {
                struct cprnths_dict_pair_t *restrict const free_slot = D;

                for (struct cprnths_dict_pair_t *const limit = D + d->slots_total; ++D < limit;)
                    if (D->key != NULL && cprnths_string_equal(D->key, k)) {
                        if (D->value != v) {
                            cprnths_ref_increment(v, 1);
                            cprnths_ref_increment(D->value, -1);
                            D->value = v;
                        }
                        return 1;
                    }

                if (NULL == ( k = cprnths_string_copy(k) ))
                    return 0;

                free_slot->key = (struct cprnths_string_t*)k;
                cprnths_ref_increment(v, 1);
                free_slot->value = v;
                --d->slots_free;
                return 1;
            }

            if (cprnths_string_equal(D->key, k)) {
                if (D->value != v) {
                    cprnths_ref_increment(v, 1);
                    cprnths_ref_increment(D->value, -1);
                    D->value = v;
                }
                return 1;
            }
        }

    {
        struct cprnths_dict_pair_t *restrict D = d->dict;
        struct cprnths_dict_pair_t *const limit = D + d->slots_total;
        do {
            if (cprnths_string_equal(D->key, k)) {
                if (D->value != v) {
                    cprnths_ref_increment(v, 1);
                    cprnths_ref_increment(D->value, -1);
                    D->value = v;
                }
                return 1;
            }
        } while (++D < limit);
    }

    {
        size_t const slots_total = d->slots_total + d->chunksize;

        {
            if (NULL == ( k = cprnths_string_copy(k) ))
                return 0;

            struct cprnths_dict_pair_t *restrict D = realloc(
                d->dict,
                slots_total * sizeof(struct cprnths_dict_pair_t)
            );
            if (D == NULL) {
                cprnths_string_destroy((struct cprnths_string_t*)k);
                return 0;
            }

            d->dict = D;
            D += d->slots_total;
            D->key = (struct cprnths_string_t*)k;
            cprnths_ref_increment(v, 1);
            D->value = v;

            struct cprnths_dict_pair_t *const limit = d->dict + slots_total;
            while (++D < limit) {
                D->key = NULL;
                D->value = NULL;
            }
        }

        d->slots_total = slots_total;
    }

    d->slots_free += d->chunksize - 1u;

    return 1;
}

static
struct cprnths_dict_pair_t*
cpintern_dict_getpairpos(
    struct cprnths_dict_t const *restrict const d,
    struct cprnths_string_t const *restrict const k
) {
    if (d->slots_total != d->slots_free) {
        struct cprnths_dict_pair_t *restrict D = d->dict;
        struct cprnths_dict_pair_t *const limit = D + d->slots_total;
        do {
            if (D->key != NULL && cprnths_string_equal(D->key, k))
                return D;
        } while (++D < limit);
    }

    return NULL;
}

struct cprnths_ref_t*
cprnths_dict_getval(
    struct cprnths_dict_t const *restrict const d,
    struct cprnths_string_t const *restrict const k
) {
    {
        struct cprnths_dict_pair_t const *restrict const D = cpintern_dict_getpairpos(d, k);
        if (D != NULL) {
            cprnths_ref_increment(D->value, 1);
            return D->value;
        }
    }

    return NULL;
}

static
void
cpintern_dict_delpair(
    struct cprnths_dict_t *restrict const d,
    struct cprnths_dict_pair_t *restrict const p
) {
    cprnths_string_destroy(p->key);
    p->key = NULL;
    cprnths_ref_increment(p->value, -1);
    p->value = NULL;
    ++d->slots_free;

    if (d->slots_free / d->chunksize < 2u)
        return;

    {
        struct cprnths_dict_pair_t *restrict free_slot = d->dict;
        struct cprnths_dict_pair_t *restrict used_slot = d->dict + d->slots_total;
        struct cprnths_dict_pair_t *const last_used_slot = used_slot - d->chunksize;
        do {
            if ((--used_slot)->key != NULL) {
                while (free_slot->key != NULL)
                    ++free_slot;
                free_slot->key = used_slot->key;
                free_slot->value = used_slot->value;
                used_slot->key = NULL;
                used_slot->value = NULL;
            }
        } while (used_slot > last_used_slot);
    }

    size_t const slots_total = d->slots_total - d->chunksize;
    struct cprnths_dict_pair_t *restrict const D = realloc(
        d->dict,
        slots_total * sizeof(struct cprnths_dict_pair_t)
    );
    if (D != NULL) {
        d->dict = D;
        d->slots_total = slots_total;
        d->slots_free -= d->chunksize;
    }
}

struct cprnths_ref_t*
cprnths_dict_popval(
    struct cprnths_dict_t *restrict const d,
    struct cprnths_string_t const *restrict const k
) {
    {
        struct cprnths_dict_pair_t *restrict const D = cpintern_dict_getpairpos(d, k);
        if (D != NULL) {
            struct cprnths_ref_t *const v = D->value;
            cprnths_ref_increment(v, 1);
            cpintern_dict_delpair(d, D);
            return v;
        }
    }

    return NULL;
}

_Bool
cprnths_dict_delpair(
    struct cprnths_dict_t *restrict const d,
    struct cprnths_string_t const *restrict const k
) {
    {
        struct cprnths_dict_pair_t *restrict const D = cpintern_dict_getpairpos(d, k);
        if (D != NULL) {
            cpintern_dict_delpair(d, D);
            return 1;
        }
    }

    return 0;
}

void
cprnths_dict_destroy(
    struct cprnths_dict_t *restrict const d
) {
    {
        struct cprnths_dict_pair_t *restrict D = d->dict;
        struct cprnths_dict_pair_t *const limit = D + d->slots_total;
        do {
            if (D->key != NULL) {
                cprnths_string_destroy(D->key);
                cprnths_ref_increment(D->value, -1);
            }
        } while (++D < limit);
    }

    free(d->dict);
    free(d);
}
