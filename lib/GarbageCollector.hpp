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


#ifndef CPARENTHESES_TOP_LEVEL_INCLUDE_CGMOU
#define CPARENTHESES_TOP_LEVEL_INCLUDE_CGMOU 'G'
#endif


#include <cstdint>
// std::uintmax_t

#include <map>
// std::map

#include <set>
// std::set

#include <stdexcept>
// std::logic_error


namespace CParentheses
{
	class GarbageCollector;
}


#include "ManagedReference.hpp"
// CParentheses::ManagedReference

#include "Object.hpp"
// CParentheses::Object

#include "UnmanagedReference.hpp"
// CParentheses::UnmanagedReference


namespace CParentheses
{


// Cleans up unneeded objects
class GarbageCollector
{
	friend ManagedReference;
	friend UnmanagedReference;

public:
	class Lock;
	class ReachableObjectsExist;

	GarbageCollector(void);
	GarbageCollector(GarbageCollector const&) = delete;
	GarbageCollector& operator = (GarbageCollector const&) = delete;
	GarbageCollector(GarbageCollector&&) = delete;
	GarbageCollector& operator = (GarbageCollector&&) = delete;
	~GarbageCollector(void) noexcept(false);

	void track(Object *);

	bool // Whether some objects have been cleaned up
	cleanUp(void);

protected:
	typedef std::uintmax_t refs_amount_t;
	typedef long double ref_del_series_count_t;

	class ObjectInfo;
	class CleaningUpState;

	template<class T, T base>
	class LogarithmicallyRaising;

	std::map<Object *, ObjectInfo> trackedObjects;

	std::uintmax_t locksAmount;
	std::set<Object *> protectedObjects;

	bool isCleaningUp;

	bool refDelSeriesRunning;
	ref_del_series_count_t refDelSeriesCount;
	LogarithmicallyRaising<ref_del_series_count_t, 2.0F> refDelSeriesUntilGC;

	void addManagedRefs(Object *, Object *, refs_amount_t = 1u);
	void delManagedRefs(Object *, Object *, refs_amount_t = 1u);
	void addUnmanagedRefs(Object *, refs_amount_t = 1u);
	void delUnmanagedRefs(Object *, refs_amount_t = 1u);
	void onRefAdd(void);

	typedef decltype(trackedObjects)::iterator consider_delete_target_t;

	void considerDelete(consider_delete_target_t);
};

// Prevents a garbage collector from cleaning up any objects until destroyed
class GarbageCollector::Lock
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

class GarbageCollector::ReachableObjectsExist : public std::logic_error
{
	using std::logic_error::logic_error;
};

class GarbageCollector::ObjectInfo
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
	> ourManagedRefs;

	// Amount of references to this object from other objects
	refs_amount_t theirManagedRefs;

	// Amount of references to this object from things this GC doesn't manage
	refs_amount_t theirUnmanagedRefs;

	// Preallocated for the cleanup process
	cleanup_status_t cleanupStatus;

	ObjectInfo(void);
	ObjectInfo(ObjectInfo const&);
	ObjectInfo& operator = (ObjectInfo const&);
	ObjectInfo(ObjectInfo&&);
	ObjectInfo& operator = (ObjectInfo&&);
	~ObjectInfo(void);
};

// Indicates that a garbage collector is cleaning up until destroyed
class GarbageCollector::CleaningUpState
{
public:
	CleaningUpState(
		// target GC
		GarbageCollector&
	);
	CleaningUpState(CleaningUpState const&) = delete;
	CleaningUpState& operator = (CleaningUpState const&) = delete;
	CleaningUpState(CleaningUpState&&) = delete;
	CleaningUpState& operator = (CleaningUpState&&) = delete;
	~CleaningUpState(void);

protected:
	// target GC
	GarbageCollector& gc;
};

template<class T, T base>
// y = round(log(x + 1) / log(base))
class GarbageCollector::LogarithmicallyRaising
{
public:
	LogarithmicallyRaising(T /* x */ = 0.0F) noexcept;
	LogarithmicallyRaising(LogarithmicallyRaising const&) noexcept;
	LogarithmicallyRaising& operator = (LogarithmicallyRaising const&) noexcept;
	LogarithmicallyRaising(LogarithmicallyRaising&&) noexcept;
	LogarithmicallyRaising& operator = (LogarithmicallyRaising&&) noexcept;
	~LogarithmicallyRaising(void);

	T getX(void) const noexcept;
	T getY(void) const noexcept;
	void setX(T) noexcept;

protected:
	static T const maxSafeInt, logBase;

	T x, y;
};


}


#if CPARENTHESES_TOP_LEVEL_INCLUDE_CGMOU == 'G'
#undef CPARENTHESES_TOP_LEVEL_INCLUDE_CGMOU
#include "funcdefs/CloneTracker.hpp"
#include "funcdefs/GarbageCollector.hpp"
#include "funcdefs/ManagedReference.hpp"
#include "funcdefs/Object.hpp"
#include "funcdefs/UnmanagedReference.hpp"
#endif


#endif
