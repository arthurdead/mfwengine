#ifndef MFW_PUBLIC_STL_ANY_HPP
#define MFW_PUBLIC_STL_ANY_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STDCPP_IS(DEFAULT)
	#include <any>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/any.h>
#else
	#error
#endif

namespace mfw::stl
{
	using ::MFW_STD_NAMESPACE::any;
}

#endif