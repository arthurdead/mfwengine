#ifndef MFW_PUBLIC_STL_VARIANT_HPP
#define MFW_PUBLIC_STL_VARIANT_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STDCPP_IS(DEFAULT)
	#include <variant>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/variant.h>
#else
	#error
#endif

namespace mfw::stl
{
	using ::MFW_STD_NAMESPACE::variant;
	using ::MFW_STD_NAMESPACE::get;
}

#endif