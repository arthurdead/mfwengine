#ifndef __MFW_PUBLIC_STL_STACK_H
#define __MFW_PUBLIC_STL_STACK_H

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/deque.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <stack>
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	template <typename T, typename D = deque<T, allocator<T>>>
	using stack = ::MFW_STD_NAMESPACE::stack<T, D>;
#else
	#error
#endif
}

#endif