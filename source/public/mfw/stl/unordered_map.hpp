#ifndef __MFW_PUBLIC_STL_UNORDERED_MAP_H
#define __MFW_PUBLIC_STL_UNORDERED_MAP_H

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/memory.hpp>
#include <public/mfw/stl/utility.hpp>
#include <public/mfw/stl/functional.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <unordered_map>
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	template <typename K, typename T, typename H = hash<K>, typename E = equal_to<K>, typename A = allocator<pair<const K, T>>>
	using unordered_map = ::MFW_STD_NAMESPACE::unordered_map<K, T, H, E, A>;
#else
	#error
#endif
}

#include <public/mfw/stl/detail/unordered_map_funcs.hpp>

#endif