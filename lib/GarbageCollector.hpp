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


#ifndef CPARENTHESES_INCLUDE_GARBAGECOLLECTOR
#define CPARENTHESES_INCLUDE_GARBAGECOLLECTOR 1


#include <cstdint>
// std::uintmax_t

#include <map>
// std::map

#include <utility>
// std::move


namespace CParentheses
{
	class GarbageCollector;
}


#include "Object.hpp"
// CParentheses::Object


namespace CParentheses
{


// Cleans up unneeded objects
class GarbageCollector
{
// TODO: actually clean up
public:
	typedef std::uintmax_t refs_amount_t;

	GarbageCollector(void);
	GarbageCollector(GarbageCollector const&) = delete;
	GarbageCollector& operator = (GarbageCollector const&) = delete;
	GarbageCollector(GarbageCollector&&) = delete;
	GarbageCollector& operator = (GarbageCollector&&) = delete;
	~GarbageCollector(void);

	// Track the given object
	GarbageCollector& operator += (Object *);

	void addManagedRef(Object *, Object *);
	void addManagedRefs(Object *, Object *, refs_amount_t);
	void addUnmanagedRef(Object *);
	void addUnmanagedRefs(Object *, refs_amount_t);

	void delManagedRef(Object *, Object *);
	void delManagedRefs(Object *, Object *, refs_amount_t);
	void delUnmanagedRef(Object *);
	void delUnmanagedRefs(Object *, refs_amount_t);

protected:
	class ObjectInfo
	{
	public:
		std::map<
			// An object this object references...
			Object *,
			// ... that many times
			refs_amount_t
		> managedRefs;

		// Amount of references to this object from things this GC doesn't manage
		refs_amount_t unmanagedRefs;

		// Preallocated for the cleanup process
		bool isReachable;

		ObjectInfo(void);
		ObjectInfo(ObjectInfo const&);
		ObjectInfo& operator = (ObjectInfo const&);
		ObjectInfo(ObjectInfo&&);
		ObjectInfo& operator = (ObjectInfo&&);
		~ObjectInfo(void);
	};

	std::map<Object *, ObjectInfo> trackedObjects;
};


inline
GarbageCollector::GarbageCollector(void)
{}

inline
GarbageCollector::~GarbageCollector(void)
{}

inline
GarbageCollector& GarbageCollector::operator += (Object * target)
{
	trackedObjects.emplace(target, ObjectInfo());
	return *this;
}

inline
void GarbageCollector::addManagedRef(Object * from, Object * to)
{
	addManagedRefs(from, to, 1u);
}

inline
void GarbageCollector::addManagedRefs(Object * from, Object * to, GarbageCollector::refs_amount_t refsAmount)
{
	auto& managedRefs (trackedObjects.at(from).managedRefs);
	auto managedRef (managedRefs.emplace(to, refsAmount));
	if (!(managedRef.second || (managedRef.first->second += refsAmount)))
	{
		managedRefs.erase(managedRef.first);
	}
}

inline
void GarbageCollector::addUnmanagedRef(Object * to)
{
	addUnmanagedRefs(to, 1u);
}

inline
void GarbageCollector::addUnmanagedRefs(Object * to, GarbageCollector::refs_amount_t refsAmount)
{
	trackedObjects.at(to).unmanagedRefs += refsAmount;
}

inline
void GarbageCollector::delManagedRef(Object * from, Object * to)
{
	delManagedRefs(from, to, 1u);
}

inline
void GarbageCollector::delManagedRefs(Object * from, Object * to, GarbageCollector::refs_amount_t refsAmount)
{
	addManagedRefs(from, to, -refsAmount);
}

inline
void GarbageCollector::delUnmanagedRef(Object * to)
{
	delUnmanagedRefs(to, 1u);
}

inline
void GarbageCollector::delUnmanagedRefs(Object * to, GarbageCollector::refs_amount_t refsAmount)
{
	addUnmanagedRefs(to, -refsAmount);
}

inline
GarbageCollector::ObjectInfo::ObjectInfo(void) : unmanagedRefs(0u), isReachable(true)
{}

inline
GarbageCollector::ObjectInfo::ObjectInfo(GarbageCollector::ObjectInfo const& origin)
	: managedRefs(origin.managedRefs), unmanagedRefs(origin.unmanagedRefs), isReachable(origin.isReachable)
{}

inline
GarbageCollector::ObjectInfo& GarbageCollector::ObjectInfo::operator = (GarbageCollector::ObjectInfo const& origin)
{
	managedRefs = origin.managedRefs;
	unmanagedRefs = origin.unmanagedRefs;
	isReachable = origin.isReachable;
	return *this;
}

inline
GarbageCollector::ObjectInfo::ObjectInfo(GarbageCollector::ObjectInfo&& origin)
	: managedRefs(std::move(origin.managedRefs)), unmanagedRefs(origin.unmanagedRefs), isReachable(origin.isReachable)
{}

inline
GarbageCollector::ObjectInfo& GarbageCollector::ObjectInfo::operator = (GarbageCollector::ObjectInfo&& origin)
{
	managedRefs = std::move(origin.managedRefs);
	unmanagedRefs = origin.unmanagedRefs;
	isReachable = origin.isReachable;
	return *this;
}

inline
GarbageCollector::ObjectInfo::~ObjectInfo(void)
{}


}


#endif
