#ifndef _MFW_PUBLIC_STL_LIMITS_HPP
#define _MFW_PUBLIC_STL_LIMITS_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STDCPP_IS(DEFAULT)
	#include <limits>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/numeric_limits.h>
#else
	#error
#endif

namespace mfw::stl
{
	using ::MFW_STD_NAMESPACE::numeric_limits;
}

#endif