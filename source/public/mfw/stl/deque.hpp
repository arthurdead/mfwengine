#ifndef __MFW_PUBLIC_STL_DEQUE_H
#define __MFW_PUBLIC_STL_DEQUE_H

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/memory.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <deque>
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	template <typename T, typename A = allocator<T>>
	using deque = ::MFW_STD_NAMESPACE::deque<T, A>;
#else
	#error
#endif
}

#endif