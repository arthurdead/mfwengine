#ifndef MFW_PUBLIC_STL_INITIALIZER_LIST_HPP
#define MFW_PUBLIC_STL_INITIALIZER_LIST_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STDCPP_IS(DEFAULT)
	#include <initializer_list>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/initializer_list.h>
#else
	#error
#endif

namespace mfw::stl
{
	using ::MFW_STD_NAMESPACE::initializer_list;
}

#endif