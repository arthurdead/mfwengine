#ifndef _MFW_PUBLIC_STL_FUNCTIONAL_HPP
#define _MFW_PUBLIC_STL_FUNCTIONAL_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

#pragma push_macro("new")
#undef new
#if MFW_STDCPP_IS(DEFAULT)
	#include <functional>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/functional.h>
#else
	#error
#endif
#pragma pop_macro("new")

namespace mfw::stl
{
	using ::MFW_STD_NAMESPACE::equal_to;
	using ::MFW_STD_NAMESPACE::hash;
	using ::MFW_STD_NAMESPACE::function;
	using ::MFW_STD_NAMESPACE::reference_wrapper;
	using ::MFW_STD_NAMESPACE::ref;
	using ::MFW_STD_NAMESPACE::cref;
	using ::MFW_STD_NAMESPACE::bind;
}

#endif