#ifndef _MFW_PUBLIC_STL_OPTIONAL_HPP
#define _MFW_PUBLIC_STL_OPTIONAL_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STDCPP_IS(DEFAULT)
	#include <optional>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/optional.h>
#else
	#error
#endif

namespace mfw::stl
{
	using ::MFW_STD_NAMESPACE::optional;
}

#endif