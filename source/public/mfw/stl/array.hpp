#ifndef _MFW_PUBLIC_STL_ARRAY_HPP
#define _MFW_PUBLIC_STL_ARRAY_HPP

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
	template <typename _Tp, size_t _Sp>
	using array = ::MFW_STD_NAMESPACE::array<_Tp, _Sp>;
}

#endif