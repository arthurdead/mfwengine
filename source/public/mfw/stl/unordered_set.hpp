#ifndef __MFW_PUBLIC_STL_UNORDERED_SET_H
#define __MFW_PUBLIC_STL_UNORDERED_SET_H

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/memory.hpp>
#include <public/mfw/stl/utility.hpp>
#include <public/mfw/stl/functional.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <unordered_set>
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	template <typename K, typename H = hash<K>, typename E = equal_to<K>, typename A = allocator<K>>
	using unordered_set = ::MFW_STD_NAMESPACE::unordered_set<K, H, E, A>;
#else
	#error
#endif
}

#endif