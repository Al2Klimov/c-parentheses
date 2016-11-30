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


#ifndef CPARENTHESES_INCLUDE_FUNCDEFS_MANAGEDREFERENCE
#define CPARENTHESES_INCLUDE_FUNCDEFS_MANAGEDREFERENCE 1


#include "../ManagedReference.hpp"
// CParentheses::ManagedReference


namespace CParentheses
{


inline
ManagedReference::ManagedReference(Object * referencing, Object * referenced)
	: referencing(referencing), referenced(referenced)
{
	if (referenced != nullptr)
	{
		referenced->gc->addManagedRefs(referencing, referenced);
	}
}

inline
ManagedReference::ManagedReference(ManagedReference const& rhs) noexcept
	: ManagedReference(rhs.referencing, rhs.referenced)
{}

inline
ManagedReference& ManagedReference::operator = (ManagedReference const& rhs)
{
	if (referenced != rhs.referenced)
	{
		if (referenced != nullptr)
		{
			referenced->gc->delManagedRefs(referencing, referenced);
		}
		referenced = rhs.referenced;
		if (referenced != nullptr)
		{
			referenced->gc->addManagedRefs(referencing, referenced);
		}
	}

	return *this;
}

inline
ManagedReference::ManagedReference(ManagedReference&& rhs) noexcept
	: referencing(rhs.referencing), referenced(rhs.referenced)
{
	rhs.referenced = nullptr;
}

inline
ManagedReference& ManagedReference::operator = (ManagedReference&& rhs) noexcept
{
	if (referenced != nullptr)
	{
		referenced->gc->delManagedRefs(referencing, referenced);
	}
	referenced = rhs.referenced;
	rhs.referenced = nullptr;

	return *this;
}

inline
ManagedReference::~ManagedReference(void)
{
	if (referenced != nullptr)
	{
		referenced->gc->delManagedRefs(referencing, referenced);
	}
}

inline
ManagedReference::operator Object * (void)
{
	return referenced;
}


}


#endif
