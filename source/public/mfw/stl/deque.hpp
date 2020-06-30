#ifndef _MFW_PUBLIC_STL_DEQUE_HPP
#define _MFW_PUBLIC_STL_DEQUE_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/memory.hpp>

#if MFW_STDCPP_IS(DEFAULT)
	#include <deque>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/deque.h>
#else
	#error
#endif

namespace mfw::stl
{
	template <typename T, typename A = allocator<T>>
	using deque = ::MFW_STD_NAMESPACE::deque<T, A>;
}

#endif