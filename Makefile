# C() / "C Parentheses"
# A pure functional programming language
# Reference implementation
#
# Copyright (C) 2015-2016  Alexander A. Klimov
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


# GNU Makefile
# designed for GCC (and GDB) on *nix, other tools/OSs MAY work as well


CSRCFILES = interpreter.c copy_table.c dict.c exec.c expr.c expr_assign.c expr_goto.c expr_jlabel.c expr_variable.c garbage_table.c io.c obj_bool.c obj_string.c object.c parser.c reference.c stack.c string.c
CCOPTS = -std=c99 -pedantic-errors -Wall -Wextra


cprnthsi: $(CSRCFILES)
	gcc $(CCOPTS) -O3 -o cprnthsi $(CSRCFILES)

cprnthsi-dbg: $(CSRCFILES)
	gcc $(CCOPTS) -Og -g -o cprnthsi-dbg $(CSRCFILES)
