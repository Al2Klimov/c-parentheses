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
	// Prevents a garbage collector from cleaning up any objects until destroyed
	class Lock
	{
	public:
		Lock(
			// target GC
			GarbageCollector&
		);
		Lock(Lock const&) = delete;
		Lock& operator = (Lock const&) = delete;
		Lock(Lock&&) = delete;
		Lock& operator = (Lock&&) = delete;
		~Lock(void);

	protected:
		// target GC
		GarbageCollector& gc;
	};

	typedef std::uintmax_t refs_amount_t;

	GarbageCollector(void);
	GarbageCollector(GarbageCollector const&) = delete;
	GarbageCollector& operator = (GarbageCollector const&) = delete;
	GarbageCollector(GarbageCollector&&) = delete;
	GarbageCollector& operator = (GarbageCollector&&) = delete;
	~GarbageCollector(void);

	void track(Object *);

	void addManagedRefs(Object *, Object *, refs_amount_t = 1u);
	void addUnmanagedRefs(Object *, refs_amount_t = 1u);
	void delManagedRefs(Object *, Object *, refs_amount_t = 1u);
	void delUnmanagedRefs(Object *, refs_amount_t = 1u);

	bool // Whether some objects have been cleaned up
	cleanUp(void);

protected:
	class ObjectInfo
	{
	public:
		// Bitset:
		// 1st (lowest) bit : marked as reachable
		// 2nd bit          : has been preprocessed
		typedef unsigned char cleanup_status_t;

		std::map<
			// An object this object references...
			Object *,
			// ... that many times
			refs_amount_t
		> managedRefs;

		// Amount of references to this object from things this GC doesn't manage
		refs_amount_t unmanagedRefs;

		// Preallocated for the cleanup process
		cleanup_status_t cleanupStatus;

		ObjectInfo(void);
		ObjectInfo(ObjectInfo const&);
		ObjectInfo& operator = (ObjectInfo const&);
		ObjectInfo(ObjectInfo&&);
		ObjectInfo& operator = (ObjectInfo&&);
		~ObjectInfo(void);
	};

	std::map<Object *, ObjectInfo> trackedObjects;

	std::uintmax_t locksAmount;
};


inline
GarbageCollector::GarbageCollector(void) : locksAmount(0u)
{}

inline
GarbageCollector::~GarbageCollector(void)
{}

inline
void GarbageCollector::track(Object * target)
{
	trackedObjects.emplace(target, ObjectInfo());
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
void GarbageCollector::addUnmanagedRefs(Object * to, GarbageCollector::refs_amount_t refsAmount)
{
	trackedObjects.at(to).unmanagedRefs += refsAmount;
}

inline
void GarbageCollector::delManagedRefs(Object * from, Object * to, GarbageCollector::refs_amount_t refsAmount)
{
	addManagedRefs(from, to, -refsAmount);
}

inline
void GarbageCollector::delUnmanagedRefs(Object * to, GarbageCollector::refs_amount_t refsAmount)
{
	addUnmanagedRefs(to, -refsAmount);
}

inline
bool GarbageCollector::cleanUp(void)
{
	if (locksAmount)
	{
		return false;
	}

	{
		bool done (true);
		for (auto& trackedObject : trackedObjects)
		{
			trackedObject.second.cleanupStatus = trackedObject.second.unmanagedRefs ? 1u : 0u;
			if (!trackedObject.second.cleanupStatus)
			{
				done = false;
			}
		}
		if (done)
		{
			return false;
		}
	}

	{
		bool markedAll;
		do
		{
			markedAll = true;
			for (auto& trackedObject : trackedObjects)
			{
				if (trackedObject.second.cleanupStatus == (ObjectInfo::cleanup_status_t)1u)
				{
					for (auto& referencedObject : trackedObject.second.managedRefs)
					{
						trackedObjects.at(referencedObject.first).cleanupStatus |= (ObjectInfo::cleanup_status_t)1u;
					}
					trackedObject.second.cleanupStatus = 3u;
					markedAll = false;
				}
			}
		} while (!markedAll);
	}

	bool hasUnreachable (false);
	for (auto trackedObject (trackedObjects.begin()); trackedObject != trackedObjects.end();)
	{
		if (trackedObject->second.cleanupStatus)
		{
			++trackedObject;
		}
		else
		{
			trackedObjects.erase(trackedObject);
			trackedObject = trackedObjects.begin();
			delete trackedObject->first;
			hasUnreachable = true;
		}
	}
	return hasUnreachable;
}

inline
GarbageCollector::ObjectInfo::ObjectInfo(void) : unmanagedRefs(0u), cleanupStatus(3u)
{}

inline
GarbageCollector::ObjectInfo::ObjectInfo(GarbageCollector::ObjectInfo const& origin)
	: managedRefs(origin.managedRefs), unmanagedRefs(origin.unmanagedRefs), cleanupStatus(origin.cleanupStatus)
{}

inline
GarbageCollector::ObjectInfo& GarbageCollector::ObjectInfo::operator = (GarbageCollector::ObjectInfo const& origin)
{
	managedRefs = origin.managedRefs;
	unmanagedRefs = origin.unmanagedRefs;
	cleanupStatus = origin.cleanupStatus;
	return *this;
}

inline
GarbageCollector::ObjectInfo::ObjectInfo(GarbageCollector::ObjectInfo&& origin)
	: managedRefs(std::move(origin.managedRefs)), unmanagedRefs(origin.unmanagedRefs), cleanupStatus(origin.cleanupStatus)
{}

inline
GarbageCollector::ObjectInfo& GarbageCollector::ObjectInfo::operator = (GarbageCollector::ObjectInfo&& origin)
{
	managedRefs = std::move(origin.managedRefs);
	unmanagedRefs = origin.unmanagedRefs;
	cleanupStatus = origin.cleanupStatus;
	return *this;
}

inline
GarbageCollector::ObjectInfo::~ObjectInfo(void)
{}

inline
GarbageCollector::Lock::Lock(GarbageCollector& gc) : gc(gc)
{
	++gc.locksAmount;
}

inline
GarbageCollector::Lock::~Lock(void)
{
	--gc.locksAmount;
}


}


#endif
