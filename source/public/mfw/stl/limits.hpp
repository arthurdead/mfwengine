#ifndef MFW_PUBLIC_STL_LIMITS_HPP
#define MFW_PUBLIC_STL_LIMITS_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STDCPP_IS(DEFAULT)
	#include <limits>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/numeric_limits.h>
#else
	#error
#endif

#if MFW_STDC_IS(DEFAULT)
	#include <climits>
#else
	#error
#endif

#if MFW_OS_IS(LINUX)
	#define _PATH_MAX MFW_MACRO_EXPAND(PATH_MAX)
	#undef PATH_MAX
	#define PATH_MAX please avoid this
#endif

namespace mfw::stl
{
	using ::MFW_STD_NAMESPACE::numeric_limits;
}

#endif