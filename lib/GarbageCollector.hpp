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


#ifndef CPARENTHESES_TOP_LEVEL_INCLUDE_CGOU
#define CPARENTHESES_TOP_LEVEL_INCLUDE_CGOU 'G'
#endif


#include <cstdint>
// std::uintmax_t

#include <map>
// std::map

#include <set>
// std::set

#include <stdexcept>
// std::logic_error

#include <unordered_map>
// std::unordered_map


namespace CParentheses
{
	class GarbageCollector;
}


#include "Object.hpp"
// CParentheses::Object

#include "UnmanagedReference.hpp"
// CParentheses::UnmanagedReference


namespace CParentheses
{


// Cleans up unneeded objects
class GarbageCollector
{
// TODO: actually clean up

	friend UnmanagedReference;

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

	class ReachableObjectsExist : public std::logic_error
	{
		using std::logic_error::logic_error;
	};

	typedef std::uintmax_t refs_amount_t;

	GarbageCollector(void);
	GarbageCollector(GarbageCollector const&) = delete;
	GarbageCollector& operator = (GarbageCollector const&) = delete;
	GarbageCollector(GarbageCollector&&) = delete;
	GarbageCollector& operator = (GarbageCollector&&) = delete;
	~GarbageCollector(void) noexcept(false);

	void track(Object *);

	void addManagedRefs(Object *, Object *, refs_amount_t = 1u);
	void delManagedRefs(Object *, Object *, refs_amount_t = 1u);

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

	std::unordered_map<Object *, ObjectInfo> trackedObjects;

	std::uintmax_t locksAmount;
	std::set<Object *> protectedObjects;

	void addUnmanagedRefs(Object *, refs_amount_t = 1u);
	void delUnmanagedRefs(Object *, refs_amount_t = 1u);
};


}


#if CPARENTHESES_TOP_LEVEL_INCLUDE_CGOU == 'G'
#undef CPARENTHESES_TOP_LEVEL_INCLUDE_CGOU
#include "funcdefs/CloneTracker.hpp"
#include "funcdefs/GarbageCollector.hpp"
#include "funcdefs/Object.hpp"
#include "funcdefs/UnmanagedReference.hpp"
#endif


#endif
