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


#ifndef CPARENTHESES_INCLUDE_FUNCDEFS_OBJECT
#define CPARENTHESES_INCLUDE_FUNCDEFS_OBJECT 1


#include "../Object.hpp"
// CParentheses::Object

#include "../CloneTracker.hpp"
// CParentheses::CloneTracker

#include "../GarbageCollector.hpp"
// CParentheses::GarbageCollector


namespace CParentheses
{


inline
Object::Object(GarbageCollector * gc) : gc(gc)
{
	gc->track(this);
}

inline
Object::Object(Object const& origin) : Object(origin.gc)
{}

inline
Object::~Object(void)
{}

inline
Object * Object::clone(void) const
{
	CloneTracker cloneTracker;
	return clone(cloneTracker);
}

inline
Object * Object::clone(CloneTracker& cloneTracker) const
{
	GarbageCollector::Lock gcLock (*gc);

	auto alreadyCopied (cloneTracker.get(this));
	if (alreadyCopied != nullptr)
	{
		return alreadyCopied;
	}

	auto copy (cloneSelf());
	cloneTracker.add(this, copy);
	copy->cloneRefs(cloneTracker);
	return copy;
}

inline
void Object::cloneRefs(CloneTracker&)
{}

inline
Object::operator bool (void) const noexcept
{
	return false;
}


}


#endif
