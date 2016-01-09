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


// Function definitions for I/O


#include <stddef.h>
// NULL
// size_t

#include <stdlib.h>
// malloc()
// realloc()
// free()

#include <stdio.h>
// FILE
// fopen()
// fread()
// feof()
// ferror()
// fclose()
// stdin

#include "error.h"
// cprnths_error_*


cprnths_error_t
cprnths_fcat(
    char const *restrict const fname,
    char* *restrict const content_,
    size_t *restrict length
) {
    char *restrict content = malloc(4096u);
    if (content == NULL)
        return cprnths_error_nomem;

    cprnths_error_t err;

    {
        FILE* f;
        if (fname == NULL)
            f = stdin;
        else if (NULL == ( f = fopen(fname, "rb") )) {
            err = cprnths_error_io_open;
            goto CleanupContent;
        }

        {
            size_t used = 0u;

            {
                char* reallocate;
                size_t available = 4096u;
                do {
                    if (available - used < 4096u) {
                        if (NULL == ( reallocate = realloc(
                            content, ( available += 4096u )
                        ) )) {
                            err = cprnths_error_nomem;
                            goto CleanupFile;
                        }
                        content = reallocate;
                    }

                    used += fread(content + used, 1u, 4096u, f);
                    if (ferror(f)) {
                        err = cprnths_error_io_read;
                        goto CleanupFile;
                    }
                } while (!feof(f));

                if (available != used) {
                    if (used) {
                        if (NULL == ( reallocate = realloc(content, used) )) {
                            err = cprnths_error_nomem;
                            goto CleanupFile;
                        }
                        content = reallocate;
                    } else {
                        free(content);
                        content = NULL;
                    }
                }
            }

            *length = used;
        }

    *content_ = content;
    err = 0;

CleanupFile:
        if (fname != NULL)
            // Not checking for errors here as either
            // (1) EOF has been reached successfully and only that matters or
            // (2) an error has already occurred and only that error matters.
            fclose(f);
    }

CleanupContent:
    if (err)
        free(content);

    return err;
}
