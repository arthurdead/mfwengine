#ifndef MFW_PUBLIC_STL_ARRAY_HPP
#define MFW_PUBLIC_STL_ARRAY_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STDCPP_IS(DEFAULT)
	#include <array>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/array.h>
#else
	#error
#endif

namespace mfw::stl
{
	using ::MFW_STD_NAMESPACE::array;
	using ::MFW_STD_NAMESPACE::get;
}

#endif