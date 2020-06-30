#ifndef _MFW_PUBLIC_STL_ITERATOR_HPP
#define _MFW_PUBLIC_STL_ITERATOR_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

#pragma push_macro("new")
#undef new
#if MFW_STDCPP_IS(DEFAULT)
	#include <iterator>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/iterator.h>
#else
	#error
#endif
#pragma pop_macro("new")

namespace mfw::stl
{
	using ::MFW_STD_NAMESPACE::size;
	using ::MFW_STD_NAMESPACE::reverse_iterator;
#if MFW_CPP_COMPARE(>, 17)
	using ::MFW_STD_NAMESPACE::ssize;
#endif
}

#endif