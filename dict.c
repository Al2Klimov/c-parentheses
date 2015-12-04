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


// Function definitions for dictionaries


#include <stddef.h>
// NULL
// size_t

#include <stdbool.h>
// bool

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
// cprnths_string_destruct()

#include "reference.h"
// cprnths_ref_t
// cprnths_ref_increment()


cprnths_dict_t*
cprnths_dict_create(
    size_t const s
) {
    cprnths_dict_t *restrict d = malloc(sizeof(cprnths_dict_t));

    if (d == NULL)
        goto Finish;

    d->dict = malloc(s * sizeof(cprnths_dict_pair_t));

    if (d->dict == NULL) {
        free(d);
        d = NULL;
        goto Finish;
    }

    {
        cprnths_dict_pair_t *restrict p = d->dict + s;
        do {
            (--p)->key = NULL;
            p->value = NULL;
        } while (p > d->dict);
    }

    *(size_t*)&d->chunksize = d->slots_total = d->slots_free = s;

Finish:
    return d;
}

bool
cprnths_dict_addpair(
    cprnths_dict_t *restrict const d,
    cprnths_string_t const *restrict k,
    cprnths_ref_t *restrict const v
) {
    if (d->slots_total == d->slots_free) {
        if (NULL == ( k = cprnths_string_copy(k) ))
            return false;

        d->dict->key = (cprnths_string_t*)k;
        cprnths_ref_increment(v, 1);
        d->dict->value = v;
        --d->slots_free;
        return true;
    }

    if (d->slots_free)
        for (cprnths_dict_pair_t *restrict D = d->dict;; ++D) {
            if (D->key == NULL) {
                cprnths_dict_pair_t *restrict const free_slot = D;

                for (cprnths_dict_pair_t *const limit = D + d->slots_total; ++D < limit;)
                    if (D->key != NULL && cprnths_string_equal(D->key, k)) {
                        if (D->value != v) {
                            cprnths_ref_increment(v, 1);
                            cprnths_ref_increment(D->value, -1);
                            D->value = v;
                        }
                        return true;
                    }

                if (NULL == ( k = cprnths_string_copy(k) ))
                    return false;

                free_slot->key = (cprnths_string_t*)k;
                cprnths_ref_increment(v, 1);
                free_slot->value = v;
                --d->slots_free;
                return true;
            }

            if (cprnths_string_equal(D->key, k)) {
                if (D->value != v) {
                    cprnths_ref_increment(v, 1);
                    cprnths_ref_increment(D->value, -1);
                    D->value = v;
                }
                return true;
            }
        }

    {
        cprnths_dict_pair_t *restrict D = d->dict;
        cprnths_dict_pair_t *const limit = D + d->slots_total;
        do {
            if (cprnths_string_equal(D->key, k)) {
                if (D->value != v) {
                    cprnths_ref_increment(v, 1);
                    cprnths_ref_increment(D->value, -1);
                    D->value = v;
                }
                return true;
            }
        } while (++D < limit);
    }

    {
        size_t const slots_total = d->slots_total + d->chunksize;

        {
            if (NULL == ( k = cprnths_string_copy(k) ))
                return false;

            cprnths_dict_pair_t *restrict D = realloc(
                d->dict,
                slots_total * sizeof(cprnths_dict_pair_t)
            );
            if (D == NULL) {
                cprnths_string_destruct((cprnths_string_t*)k);
                return false;
            }

            d->dict = D;
            D += d->slots_total;
            D->key = (cprnths_string_t*)k;
            cprnths_ref_increment(v, 1);
            D->value = v;

            cprnths_dict_pair_t *const limit = d->dict + slots_total;
            while (++D < limit) {
                D->key = NULL;
                D->value = NULL;
            }
        }

        d->slots_total = slots_total;
    }

    d->slots_free += d->chunksize - 1u;

    return true;
}

static
cprnths_dict_pair_t*
cpintern_dict_getpairpos(
    cprnths_dict_t const *restrict const d,
    cprnths_string_t const *restrict const k
) {
    if (d->slots_total != d->slots_free) {
        cprnths_dict_pair_t *restrict D = d->dict;
        cprnths_dict_pair_t *const limit = D + d->slots_total;
        do {
            if (D->key != NULL && cprnths_string_equal(D->key, k))
                return D;
        } while (++D < limit);
    }

    return NULL;
}

cprnths_ref_t*
cprnths_dict_getval(
    cprnths_dict_t const *restrict const d,
    cprnths_string_t const *restrict const k
) {
    {
        cprnths_dict_pair_t const *restrict const D = cpintern_dict_getpairpos(d, k);
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
    cprnths_dict_t *restrict const d,
    cprnths_dict_pair_t *restrict const p
) {
    cprnths_string_destruct(p->key);
    p->key = NULL;
    cprnths_ref_increment(p->value, -1);
    p->value = NULL;
    ++d->slots_free;

    if (d->slots_free / d->chunksize < 2u)
        return;

    {
        cprnths_dict_pair_t *restrict free_slot = d->dict;
        cprnths_dict_pair_t *restrict used_slot = d->dict + d->slots_total;
        cprnths_dict_pair_t *const last_used_slot = used_slot - d->chunksize;
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
    cprnths_dict_pair_t *restrict const D = realloc(
        d->dict,
        slots_total * sizeof(cprnths_dict_pair_t)
    );
    if (D != NULL) {
        d->dict = D;
        d->slots_total = slots_total;
        d->slots_free -= d->chunksize;
    }
}

cprnths_ref_t*
cprnths_dict_popval(
    cprnths_dict_t *restrict const d,
    cprnths_string_t const *restrict const k
) {
    {
        cprnths_dict_pair_t *restrict const D = cpintern_dict_getpairpos(d, k);
        if (D != NULL) {
            cprnths_ref_t *const v = D->value;
            cprnths_ref_increment(v, 1);
            cpintern_dict_delpair(d, D);
            return v;
        }
    }

    return NULL;
}

bool
cprnths_dict_delpair(
    cprnths_dict_t *restrict const d,
    cprnths_string_t const *restrict const k
) {
    {
        cprnths_dict_pair_t *restrict const D = cpintern_dict_getpairpos(d, k);
        if (D != NULL) {
            cpintern_dict_delpair(d, D);
            return true;
        }
    }

    return false;
}

void
cprnths_dict_destruct(
    cprnths_dict_t *restrict const d
) {
    {
        cprnths_dict_pair_t *restrict D = d->dict;
        cprnths_dict_pair_t *const limit = D + d->slots_total;
        do {
            if (D->key != NULL) {
                cprnths_string_destruct(D->key);
                cprnths_ref_increment(D->value, -1);
            }
        } while (++D < limit);
    }

    free(d->dict);
    free(d);
}
