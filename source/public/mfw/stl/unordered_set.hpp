#ifndef MFW_PUBLIC_STL_UNORDERED_SET_HPP
#define MFW_PUBLIC_STL_UNORDERED_SET_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/memory.hpp>
#include <public/mfw/stl/utility.hpp>
#include <public/mfw/stl/functional.hpp>

#if MFW_STDCPP_IS(DEFAULT)
	#include <unordered_set>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/unordered_set.h>
#else
	#error
#endif

namespace mfw::stl
{
	template <typename K, typename H = hash<K>, typename E = equal_to<K>, typename A = allocator<K>>
	using unordered_set = ::MFW_STD_NAMESPACE::unordered_set<K, H, E, A>;
}

#endif