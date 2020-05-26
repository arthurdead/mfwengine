#ifndef __MFW_PUBLIC_STL_UTILITY_H
#define __MFW_PUBLIC_STL_UTILITY_H

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <utility>
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	using ::MFW_STD_NAMESPACE::pair;
	using ::MFW_STD_NAMESPACE::forward;
	using ::MFW_STD_NAMESPACE::move;
#else
	#error
#endif
}

#endif