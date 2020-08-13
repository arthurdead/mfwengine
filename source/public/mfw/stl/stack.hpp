#ifndef MFW_PUBLIC_STL_STACK_HPP
#define MFW_PUBLIC_STL_STACK_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/deque.hpp>

#if MFW_STDCPP_IS(DEFAULT)
	#include <stack>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/stack.h>
#else
	#error
#endif

namespace mfw::stl
{
	template <typename T, typename D = deque<T, allocator<T>>>
	using stack = ::MFW_STD_NAMESPACE::stack<T, D>;
}

#endif