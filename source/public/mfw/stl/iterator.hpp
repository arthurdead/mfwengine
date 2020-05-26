#ifndef __MFW_PUBLIC_STL_ITERATOR_H
#define __MFW_PUBLIC_STL_ITERATOR_H

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#pragma push_macro("new")
	#undef new
	#include <iterator>
	#pragma pop_macro("new")
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	using ::MFW_STD_NAMESPACE::size;
	using ::MFW_STD_NAMESPACE::reverse_iterator;
	#if MFW_CPP_COMPARE(>, 17)
	//using ::MFW_STD_NAMESPACE::ssize;
	#endif
#else
	#error
#endif
}

#endif