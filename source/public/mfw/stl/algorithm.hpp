#ifndef __MFW_PUBLIC_STL_ALGORITHM_H
#define __MFW_PUBLIC_STL_ALGORITHM_H

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#pragma push_macro("new")
	#undef new
	#include <algorithm>
	#pragma pop_macro("new")
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	using ::MFW_STD_NAMESPACE::clamp;
	using ::MFW_STD_NAMESPACE::min;
	using ::MFW_STD_NAMESPACE::max;
	using ::MFW_STD_NAMESPACE::transform;
	using ::MFW_STD_NAMESPACE::find;
	using ::MFW_STD_NAMESPACE::find_if;
	using ::MFW_STD_NAMESPACE::find_if_not;
#else
	#error
#endif
}

#endif