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


#ifndef CPARENTHESES_INCLUDE_FUNCDEFS_GARBAGECOLLECTOR
#define CPARENTHESES_INCLUDE_FUNCDEFS_GARBAGECOLLECTOR 1


#include "../GarbageCollector.hpp"
// CParentheses::GarbageCollector

#include "../Object.hpp"
// CParentheses::Object

#include <utility>
// std::move


namespace CParentheses
{


inline
GarbageCollector::GarbageCollector(void)
	: locksAmount(0u), isCleaningUp(false)
{}

inline
GarbageCollector::~GarbageCollector(void) noexcept(false)
{
	cleanUp();
	if (!trackedObjects.empty())
	{
		throw ReachableObjectsExist(
			"The GarbageCollector is being destroyed, but there are still reachable objects depending on it."
			" This will lead to segmentation faults and memory leaks!"
		);
	}
}

inline
void GarbageCollector::track(Object * target)
{
	trackedObjects.emplace(target, ObjectInfo());
	if (locksAmount)
	{
		protectedObjects.emplace(target);
	}
}

inline
void GarbageCollector::addManagedRefs(Object * from, Object * to, GarbageCollector::refs_amount_t refsAmount)
{
	{
		auto managedRef (trackedObjects.at(from).ourManagedRefs.emplace(to, refsAmount));
		if (!managedRef.second)
		{
			managedRef.first->second += refsAmount;
		}
	}

	trackedObjects.at(to).theirManagedRefs += refsAmount;
}

inline
void GarbageCollector::addUnmanagedRefs(Object * to, GarbageCollector::refs_amount_t refsAmount)
{
	trackedObjects.at(to).theirUnmanagedRefs += refsAmount;
}

inline
void GarbageCollector::delManagedRefs(Object * from, Object * to, GarbageCollector::refs_amount_t refsAmount)
{
	{
		auto& managedRefs (trackedObjects.at(from).ourManagedRefs);
		auto managedRef (managedRefs.find(to));
		if (!(managedRef->second -= refsAmount))
		{
			managedRefs.erase(managedRef);
		}
	}

	auto trackedObject (trackedObjects.find(to));
	trackedObject->second.theirManagedRefs -= refsAmount;
	considerDelete(trackedObject);
}

inline
void GarbageCollector::delUnmanagedRefs(Object * to, GarbageCollector::refs_amount_t refsAmount)
{
	auto trackedObject (trackedObjects.find(to));
	trackedObject->second.theirUnmanagedRefs -= refsAmount;
	considerDelete(trackedObject);
}

inline
bool GarbageCollector::cleanUp(void)
{
	{
		bool done (true);
		for (auto& trackedObject : trackedObjects)
		{
			trackedObject.second.cleanupStatus = trackedObject.second.theirUnmanagedRefs ? 1u : 0u;
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

	for (auto protectedObject : protectedObjects)
	{
		trackedObjects.at(protectedObject).cleanupStatus = 1u;
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
					for (auto& referencedObject : trackedObject.second.ourManagedRefs)
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
	CleaningUpState cleaningUpState (*this);
	for (auto trackedObject (trackedObjects.begin()), end (trackedObjects.end()); trackedObject != end;)
	{
		if (trackedObject->second.cleanupStatus)
		{
			++trackedObject;
		}
		else
		{
			delete trackedObject->first;
			trackedObjects.erase(trackedObject++);
			hasUnreachable = true;
		}
	}
	return hasUnreachable;
}

inline
void GarbageCollector::considerDelete(GarbageCollector::consider_delete_target_t target)
{
	{
		auto& targetInfo (target->second);
		if (targetInfo.theirManagedRefs || targetInfo.theirUnmanagedRefs)
		{
			return;
		}
	}

	delete target->first;
	trackedObjects.erase(target);
}

inline
GarbageCollector::ObjectInfo::ObjectInfo(void)
	: theirManagedRefs(0u), theirUnmanagedRefs(0u), cleanupStatus(3u)
{}

inline
GarbageCollector::ObjectInfo::ObjectInfo(GarbageCollector::ObjectInfo const& origin)
	: ourManagedRefs(origin.ourManagedRefs), theirManagedRefs(origin.theirManagedRefs), theirUnmanagedRefs(origin.theirUnmanagedRefs), cleanupStatus(origin.cleanupStatus)
{}

inline
GarbageCollector::ObjectInfo& GarbageCollector::ObjectInfo::operator = (GarbageCollector::ObjectInfo const& origin)
{
	ourManagedRefs = origin.ourManagedRefs;
	theirManagedRefs = origin.theirManagedRefs;
	theirUnmanagedRefs = origin.theirUnmanagedRefs;
	cleanupStatus = origin.cleanupStatus;
	return *this;
}

inline
GarbageCollector::ObjectInfo::ObjectInfo(GarbageCollector::ObjectInfo&& origin)
	: ourManagedRefs(std::move(origin.ourManagedRefs)), theirManagedRefs(origin.theirManagedRefs), theirUnmanagedRefs(origin.theirUnmanagedRefs), cleanupStatus(origin.cleanupStatus)
{}

inline
GarbageCollector::ObjectInfo& GarbageCollector::ObjectInfo::operator = (GarbageCollector::ObjectInfo&& origin)
{
	ourManagedRefs = std::move(origin.ourManagedRefs);
	theirManagedRefs = origin.theirManagedRefs;
	theirUnmanagedRefs = origin.theirUnmanagedRefs;
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
	if (!gc.locksAmount)
	{
		gc.protectedObjects.clear();
	}
}

inline
GarbageCollector::CleaningUpState::CleaningUpState(GarbageCollector& gc) : gc(gc)
{
	gc.isCleaningUp = true;
}

inline
GarbageCollector::CleaningUpState::~CleaningUpState(void)
{
	gc.isCleaningUp = false;
}


}


#endif
