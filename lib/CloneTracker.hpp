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


#ifndef CPARENTHESES_INCLUDE_CLONETRACKER
#define CPARENTHESES_INCLUDE_CLONETRACKER 1


#include <map>
// std::map


namespace CParentheses
{
	class CloneTracker;
}


#include "Object.hpp"
// CParentheses::Object


namespace CParentheses
{


// Tracks which objects have already been cloned while cloning an objects chain
// (this is required to copy diamond and circular references as expected)
class CloneTracker
{
public:
	CloneTracker(void);
	CloneTracker(CloneTracker const&) = delete;
	CloneTracker& operator = (CloneTracker const&) = delete;
	CloneTracker(CloneTracker&&) = delete;
	CloneTracker& operator = (CloneTracker&&) = delete;
	~CloneTracker(void);

	// Register a copied object
	void add(Object const * /* original */, Object * /* copy */);

	// Get the copy of the given original (or NULL if not copied yet)
	Object * get(Object const *) const noexcept;

protected:
	std::map<Object const * /* original */, Object * /* copy */> copiedObjects;
};

inline
CloneTracker::CloneTracker(void)
{}

inline
CloneTracker::~CloneTracker(void)
{}

inline
void CloneTracker::add(Object const * original, Object * copy)
{
	copiedObjects.emplace(original, copy);
}

inline
Object * CloneTracker::get(Object const * original) const noexcept
{
	auto copiedObject (copiedObjects.find(original));
	return copiedObject != copiedObjects.end() ? copiedObject->second : nullptr;
}


}


#endif
