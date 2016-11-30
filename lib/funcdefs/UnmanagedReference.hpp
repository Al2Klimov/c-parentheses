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


#ifndef CPARENTHESES_INCLUDE_FUNCDEFS_UNMANAGEDREFERENCE
#define CPARENTHESES_INCLUDE_FUNCDEFS_UNMANAGEDREFERENCE 1


#include "../UnmanagedReference.hpp"
// CParentheses::UnmanagedReference


namespace CParentheses
{


inline
UnmanagedReference::UnmanagedReference(Object * referenced) noexcept
	: referenced(referenced)
{
	if (referenced != nullptr)
	{
		referenced->gc->addUnmanagedRefs(referenced);
	}
}

inline
UnmanagedReference::UnmanagedReference(UnmanagedReference const& rhs) noexcept
	: UnmanagedReference(rhs.referenced)
{}

inline
UnmanagedReference& UnmanagedReference::operator = (UnmanagedReference const& rhs) noexcept
{
	if (referenced != rhs.referenced)
	{
		if (referenced != nullptr)
		{
			referenced->gc->delUnmanagedRefs(referenced);
		}
		referenced = rhs.referenced;
		if (referenced != nullptr)
		{
			referenced->gc->addUnmanagedRefs(referenced);
		}
	}

	return *this;
}

inline
UnmanagedReference::UnmanagedReference(UnmanagedReference&& rhs) noexcept
	: referenced(rhs.referenced)
{
	rhs.referenced = nullptr;
}

inline
UnmanagedReference& UnmanagedReference::operator = (UnmanagedReference&& rhs) noexcept
{
	if (referenced != nullptr)
	{
		referenced->gc->delUnmanagedRefs(referenced);
	}
	referenced = rhs.referenced;
	rhs.referenced = nullptr;

	return *this;
}

inline
UnmanagedReference::~UnmanagedReference(void)
{
	if (referenced != nullptr)
	{
		referenced->gc->delUnmanagedRefs(referenced);
	}
}

inline
UnmanagedReference::operator Object * (void)
{
	return referenced;
}


}


#endif
