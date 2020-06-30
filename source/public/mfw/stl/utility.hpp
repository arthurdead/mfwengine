#ifndef _MFW_PUBLIC_STL_UTILITY_HPP
#define _MFW_PUBLIC_STL_UTILITY_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STDCPP_IS(DEFAULT)
	#include <utility>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/utility.h>
#else
	#error
#endif

namespace mfw::stl
{
	using ::MFW_STD_NAMESPACE::pair;
	using ::MFW_STD_NAMESPACE::forward;
	using ::MFW_STD_NAMESPACE::move;
	using ::MFW_STD_NAMESPACE::swap;
}

#endif