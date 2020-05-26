#ifndef __MFW_PUBLIC_STL_LIMITS_H
#define __MFW_PUBLIC_STL_LIMITS_H

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <limits>
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	using ::MFW_STD_NAMESPACE::numeric_limits;
#else
	#error
#endif
}

#endif