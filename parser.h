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


// Typedefs for parsing
#ifndef CPARENTHESES_INCLUDE_PARSER
#define CPARENTHESES_INCLUDE_PARSER 1


// The kind of parse error (if any).
typedef enum {
    cprnths_parse_success,
    // an expression was parsed successfully
    cprnths_parse_malform,
    // matching type, but invalid expression
    cprnths_parse_unknown,
    // non-matching type or invalid syntax
    cprnths_parse_eof,
    // unexpected end of string
    cprnths_parse_fail
    // failed for other reason
} cprnths_parse_stat_t;


#endif
