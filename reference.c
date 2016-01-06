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


// Function definitions for references


#include <stddef.h>
// NULL
// size_t

#include <stdlib.h>
// malloc()
// free()

#include "object.h"
// cprnths_obj_t
// cprnths_obj_copy()
// cprnths_obj_destroy()

#include "reference.h"
// cprnths_ref_t

#include "garbage_table.h"
// cprnths_garbtab_t
// cprnths_garbtab_*ref()

#include "copy_table.h"
// cprnths_copytab_t
// cprnths_copytab_create()
// cprnths_copytab_addrefs()
// cprnths_copytab_chkref()
// cprnths_copytab_destroy()

#include "error.h"
// cprnths_error_*


cprnths_error_t
cprnths_ref_create(
    struct cprnths_ref_t* *restrict const r_,
    struct cprnths_obj_t *restrict const o,
    struct cprnths_garbtab_t *restrict const t
) {
    struct cprnths_ref_t *restrict const r = malloc(sizeof(struct cprnths_ref_t));
    if (r == NULL)
        return cprnths_error_nomem;

    if (t != NULL) {
        cprnths_error_t const err = cprnths_garbtab_addref(t, r);
        if (err) {
            free(r);
            return err;
        }
    }

    r->obj = o;
    r->pcnt = 1u;
    r->garbtab = t;

    *r_ = r;
    return 0;
}

void
cprnths_ref_increment(
    struct cprnths_ref_t *restrict const r,
    size_t const i
) {
    if (( r->pcnt += i ))
        return;

    if (r->obj != NULL)
        cprnths_obj_destroy(r->obj);

    if (r->garbtab != NULL)
        cprnths_garbtab_delref(r->garbtab, r);

    free(r);
}

cprnths_error_t
cprnths_ref_copy(
    struct cprnths_ref_t const *restrict const r,
    struct cprnths_ref_t* *restrict const R_,
    struct cprnths_copytab_t *restrict const t
) {
    struct cprnths_ref_t* R = cprnths_copytab_chkref(t, r);
    if (R == NULL) {
        cprnths_error_t err;
        if (( err = cprnths_ref_create(&R, NULL, r->garbtab) ))
            return err;

        if (( err = cprnths_copytab_addrefs(t, r, R) ) || (
            r->obj != NULL && ( err = cprnths_obj_copy(r->obj, &R->obj, t) )
        )) {
            cprnths_ref_increment(R, -1);
            return err;
        }
    } else {
        cprnths_ref_increment(R, 1u);
    }

    *R_ = R;
    return 0;
}

cprnths_error_t
cprnths_ref_copy_newtab(
    struct cprnths_ref_t const *restrict const r,
    struct cprnths_ref_t* *restrict const R,
    size_t const s
) {
    cprnths_error_t err;
    struct cprnths_copytab_t *restrict t;
    if (( err = cprnths_copytab_create((struct cprnths_copytab_t**)&t, s) ))
        return err;

    err = cprnths_ref_copy(r, R, t);

    cprnths_copytab_destroy(t);

    return err;
}
