#ifndef __MFW_PUBLIC_STL_SYSTEM_ERROR_H
#define __MFW_PUBLIC_STL_SYSTEM_ERROR_H

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <system_error>
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	using ::MFW_STD_NAMESPACE::error_code;
	using ::MFW_STD_NAMESPACE::errc;
#else
	#error
#endif
}

#endif