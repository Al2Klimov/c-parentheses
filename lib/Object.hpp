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


#ifndef CPARENTHESES_INCLUDE_OBJECT
#define CPARENTHESES_INCLUDE_OBJECT 1


#ifndef CPARENTHESES_TOP_LEVEL_INCLUDE
#define CPARENTHESES_TOP_LEVEL_INCLUDE 'O'
#endif


namespace CParentheses
{
	class Object;
}


#include "CloneTracker.hpp"
// CParentheses::CloneTracker

#include "GarbageCollector.hpp"
// CParentheses::GarbageCollector


namespace CParentheses
{


// The root base class of runtime objects
class Object
{
public:
	Object& operator = (Object const&) = delete;
	Object(Object&&) = delete;
	Object& operator = (Object&&) = delete;
	virtual ~Object(void) = 0;

	// Create a deep copy
	Object * clone(void) const;
	Object * clone(CloneTracker&) const;

	virtual explicit operator bool (void) const noexcept;

protected:
	// The garbage collector responsible for cleaning up this object
	GarbageCollector * gc;

	Object(
		// The garbage collector responsible for cleaning up this object
		GarbageCollector *
	);
	Object(Object const&);

	// Create a shallow copy
	virtual Object * cloneSelf() const = 0;

	// Clone all referenced objects (if any) by their `clone(CloneTracker&)',
	// replace pointers to the originals with the ones to the copies
	// and register managed references in the GC
	virtual void cloneRefs(CloneTracker&);
};


}


#if CPARENTHESES_TOP_LEVEL_INCLUDE == 'O'
#undef CPARENTHESES_TOP_LEVEL_INCLUDE
#include "funcdefs/CloneTracker.hpp"
#include "funcdefs/GarbageCollector.hpp"
#include "funcdefs/Object.hpp"
#endif


#endif
