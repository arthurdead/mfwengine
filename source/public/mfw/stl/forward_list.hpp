#ifndef _MFW_PUBLIC_STL_FORWARD_LIST_HPP
#define _MFW_PUBLIC_STL_FORWARD_LIST_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/memory.hpp>

#pragma push_macro("new")
#undef new
#if MFW_STDCPP_IS(DEFAULT)
	#include <forward_list>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/slist.h>
#else
	#error
#endif
#pragma pop_macro("new")

namespace mfw::stl
{
	template <typename _Tp, typename _Alloc = allocator<_Tp>>
	using forward_list = ::MFW_STD_NAMESPACE::
#if MFW_STDCPP_IS(EASTL)
	slist
#else
	forward_list
#endif
	<_Tp, _Alloc>;
}

#endif