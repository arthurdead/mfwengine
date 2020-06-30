#ifndef _MFW_PUBLIC_STL_ALGORITHM_HPP
#define _MFW_PUBLIC_STL_ALGORITHM_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

#pragma push_macro("new")
#undef new
#if MFW_STDCPP_IS(DEFAULT)
	#include <algorithm>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/algorithm.h>
#else
	#error
#endif
#pragma pop_macro("new")

namespace mfw::stl
{
	using ::MFW_STD_NAMESPACE::clamp;
	using ::MFW_STD_NAMESPACE::min;
	using ::MFW_STD_NAMESPACE::max;
	using ::MFW_STD_NAMESPACE::transform;
	using ::MFW_STD_NAMESPACE::find;
	using ::MFW_STD_NAMESPACE::find_if;
	using ::MFW_STD_NAMESPACE::find_if_not;
}

#endif