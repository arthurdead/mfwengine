#ifndef __MFW_PUBLIC_STL_FORWARD_LIST_H
#define __MFW_PUBLIC_STL_FORWARD_LIST_H

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/memory.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#pragma push_macro("new")
	#undef new
	#include <forward_list>
	#pragma pop_macro("new")
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	template <typename T, typename A = allocator<T>>
	using forward_list = ::MFW_STD_NAMESPACE::forward_list<T, A>;
#else
	#error
#endif
}

#endif