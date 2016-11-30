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


#ifndef CPARENTHESES_INCLUDE_MANAGEDREFERENCE
#define CPARENTHESES_INCLUDE_MANAGEDREFERENCE 1


#ifndef CPARENTHESES_TOP_LEVEL_INCLUDE_CGMOU
#define CPARENTHESES_TOP_LEVEL_INCLUDE_CGMOU 'M'
#endif


namespace CParentheses
{
	class ManagedReference;
}


#include "Object.hpp"
// CParentheses::Object


namespace CParentheses
{


// A reference to an object from another object
class ManagedReference
{
public:
	ManagedReference(Object * /* referencing */, Object * /* referenced */ = nullptr);
	ManagedReference(ManagedReference const&) noexcept;
	ManagedReference& operator = (ManagedReference const&);
	ManagedReference(ManagedReference&&) noexcept;
	ManagedReference& operator = (ManagedReference&&) noexcept;
	~ManagedReference(void);

	operator Object * (void);

protected:
	Object *referencing, *referenced;
};


}


#if CPARENTHESES_TOP_LEVEL_INCLUDE_CGMOU == 'M'
#undef CPARENTHESES_TOP_LEVEL_INCLUDE_CGMOU
#include "funcdefs/CloneTracker.hpp"
#include "funcdefs/GarbageCollector.hpp"
#include "funcdefs/ManagedReference.hpp"
#include "funcdefs/Object.hpp"
#include "funcdefs/UnmanagedReference.hpp"
#endif


#endif
