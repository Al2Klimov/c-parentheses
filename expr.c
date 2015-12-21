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


// Function definitions for expressions


#include <stddef.h>
// NULL

#include <stdlib.h>
// free()

#include "expr.h"
// cprnths_expr_t


void
cprnths_expr_destroy(
    struct cprnths_expr_t *restrict const e
) {
    if (e->cls->expr_destroy != NULL)
        (*e->cls->expr_destroy)(e);
    free(e);
}
