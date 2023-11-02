#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"
#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-forwarding-reference-overload"
#pragma once
#include "ThirdParty.h"
#include "MemoryTracker.h"


// -MEMORY TRACKED CONTAINERS-

// Memory tracked version of std::vector, template == <typename TYPE, MemoryTrackerTags tag>
template <typename TYPE, MemoryTrackerTag tag = MT_TEMPORARY>
struct T_vector : std::vector<TYPE, MemoryTrackerAllocator<TYPE>>
{
#define size_type_t typename std::vector<TYPE, MemoryTrackerAllocator<TYPE>>::size_type
#define vectorCtor std::vector<TYPE, MemoryTrackerAllocator<TYPE>>::vector

	// Match all the std::vector constructor cases, call them, and add our allocator
	T_vector() :										vectorCtor(MemoryTrackerAllocator<TYPE>{tag}) {}
	T_vector(size_type_t count) :						vectorCtor(count, MemoryTrackerAllocator<TYPE>{tag}) {}
	T_vector(size_type_t count, const TYPE& value) :	vectorCtor(count, value, MemoryTrackerAllocator<TYPE>{tag}) {}
	T_vector(const auto& vec) :							vectorCtor(vec, MemoryTrackerAllocator<TYPE>{tag}) {}
	T_vector(auto&& vec) :								vectorCtor(vec, MemoryTrackerAllocator<TYPE>{tag}) {}
	T_vector(std::initializer_list<TYPE> init) :		vectorCtor(init, MemoryTrackerAllocator<TYPE>{tag}) {}
	T_vector(auto it1, auto it2) :						vectorCtor(it1, it2, MemoryTrackerAllocator<TYPE>{tag}) {}

#undef size_type_t // Make compiler happy
};

// Memory tracked version of std::unordered_map, template == <typename KEY, typename TYPE, MemoryTrackerTags tag>
template <typename KEY, typename TYPE, MemoryTrackerTag tag = MT_TEMPORARY>
struct T_unordered_map : std::unordered_map<KEY, TYPE, std::hash<KEY>, std::equal_to<KEY>, MemoryTrackerAllocator<std::pair<const KEY, TYPE>>>
{
#define size_type_t typename std::unordered_map<KEY, TYPE, std::hash<KEY>, std::equal_to<KEY>, MemoryTrackerAllocator<std::pair<const KEY, TYPE>>>::size_type
#define unordered_mapCtor std::unordered_map<KEY, TYPE, std::hash<KEY>, std::equal_to<KEY>, MemoryTrackerAllocator<std::pair<const KEY, TYPE>>>::unordered_map

	// Match all the std::unordered_map constructor cases, call them, and add our allocator
	T_unordered_map() :																						unordered_mapCtor(MemoryTrackerAllocator<std::pair<const KEY, TYPE>>{tag}) {}
	T_unordered_map(size_type_t bucket_count) :																unordered_mapCtor(bucket_count, MemoryTrackerAllocator<std::pair<const KEY, TYPE>>{tag}) {}
	T_unordered_map(auto it1, auto it2, size_type_t bucket_count) :											unordered_mapCtor(it1, it2, bucket_count, MemoryTrackerAllocator<std::pair<const KEY, TYPE>>{tag}) {}
	T_unordered_map(const auto& other) :																	unordered_mapCtor(other, MemoryTrackerAllocator<std::pair<const KEY, TYPE>>{tag}) {}
	T_unordered_map(auto&& other) :																			unordered_mapCtor(other, MemoryTrackerAllocator<std::pair<const KEY, TYPE>>{tag}) {}
	T_unordered_map(std::initializer_list<std::pair<const KEY, TYPE>> init, size_type_t bucket_count) :		unordered_mapCtor(init, bucket_count, MemoryTrackerAllocator<std::pair<const KEY, TYPE>>{tag}) {}

#undef size_type_t // Make compiler happy
};

// Memory tracked version of std::basic_string, Always tagged MT_STRING, Use Typedef versions T_string and T_wstring
template <typename TYPE>
struct T_basicString : std::basic_string<TYPE, std::char_traits<TYPE>, MemoryTrackerAllocator<TYPE>>
{
#define size_type_t typename std::basic_string<TYPE, std::char_traits<TYPE>, MemoryTrackerAllocator<TYPE>>::size_type
#define stringCtor std::basic_string<TYPE, std::char_traits<TYPE>, MemoryTrackerAllocator<TYPE>>::basic_string

	typedef std::basic_string<TYPE>::iterator iterator;
	static constexpr auto npos{ static_cast<std::basic_string<TYPE>::size_type>(-1) };

	// Match all the std::string constructor cases, call them, and add our allocator
	T_basicString() :															stringCtor(MemoryTrackerAllocator<TYPE>{MT_STRING}) {}
	T_basicString(const auto& str) :											stringCtor(str, MemoryTrackerAllocator<TYPE>{MT_STRING}) {}
	T_basicString(const auto& str, size_type_t pos, size_type_t len = npos) :	stringCtor(str, pos, len, MemoryTrackerAllocator<TYPE>{MT_STRING}) {}
	T_basicString(const TYPE* cstr) :											stringCtor(cstr, MemoryTrackerAllocator<TYPE>{MT_STRING}) {}
	T_basicString(const TYPE* cstr, size_type_t copyNum) :						stringCtor(cstr, copyNum, MemoryTrackerAllocator<TYPE>{MT_STRING}) {}
	T_basicString(size_type_t copyNum, TYPE fillChar) :							stringCtor(copyNum, fillChar, MemoryTrackerAllocator<TYPE>{MT_STRING}) {}
	T_basicString(auto&& str) :													stringCtor(str, MemoryTrackerAllocator<TYPE>{MT_STRING}) {}
	T_basicString(iterator it1, iterator it2) :									stringCtor(it1, it2, MemoryTrackerAllocator<TYPE>{MT_STRING}) {}
	T_basicString(std::initializer_list<TYPE> init) :							stringCtor(init, MemoryTrackerAllocator<TYPE>{MT_STRING}) {}

	// Custom ctor that takes in any number of c strings/T_strings and appends them together
	template<typename... Args>
	T_basicString(const auto& first, const Args&... args) : stringCtor(first, MemoryTrackerAllocator<TYPE>{MT_STRING})
	{
		this->AppendMany(args...);
	}

	// Takes in any number of c strings/T_strings and appends them in the order they are passed
	template<typename... Args>
	void AppendMany(const auto& first, const Args&... args)
	{
		this->append(first);
		this->AppendMany(args...);
	}

	// Version of operator= that doesn't change string capacity but erases current string and appends rhs
	T_basicString& operator= (const auto& rhs)
	{
		this->clear();
		this->append(rhs);
		return *this;
	}

private:
	void AppendMany() {}	// Blank function to cap off AppendMany when args runs out.

#undef size_type_t // Make compiler happy
};

// Memory tracked version of std::string, Always tagged MT_STRING
typedef T_basicString<char>		T_string;
// Memory tracked version of std::wstring, Always tagged MT_STRING
typedef T_basicString<w16>		T_wstring;



#pragma clang diagnostic pop
#pragma clang diagnostic pop