#ifndef __MFW_PUBLIC_STL_FUNCTIONAL_H
#define __MFW_PUBLIC_STL_FUNCTIONAL_H

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#pragma push_macro("new")
	#undef new
	#include <functional>
	#pragma pop_macro("new")
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	using ::MFW_STD_NAMESPACE::equal_to;
	using ::MFW_STD_NAMESPACE::hash;
	using ::MFW_STD_NAMESPACE::function;
	using ::MFW_STD_NAMESPACE::reference_wrapper;
	using ::MFW_STD_NAMESPACE::ref;
	using ::MFW_STD_NAMESPACE::cref;
	using ::MFW_STD_NAMESPACE::bind;
#else
	#error
#endif
}

#endif