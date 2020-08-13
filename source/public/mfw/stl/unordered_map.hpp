#ifndef MFW_PUBLIC_STL_UNORDERED_MAP_HPP
#define MFW_PUBLIC_STL_UNORDERED_MAP_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/memory.hpp>
#include <public/mfw/stl/utility.hpp>
#include <public/mfw/stl/functional.hpp>

#if MFW_STDCPP_IS(DEFAULT)
	#include <unordered_map>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/unordered_map.h>
#else
	#error
#endif

namespace mfw::stl
{
	template <typename K, typename T, typename H = hash<K>, typename E = equal_to<K>, typename A = allocator<pair<const K, T>>>
	using unordered_map = ::MFW_STD_NAMESPACE::unordered_map<K, T, H, E, A>;
}

#include <public/mfw/stl/internal/unordered_map_funcs.hpp>

#endif