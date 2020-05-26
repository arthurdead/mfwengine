#ifndef __MFW_PUBLIC_STL_SOURCE_LOCATION_H
#define __MFW_PUBLIC_STL_SOURCE_LOCATION_H

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#if MFW_CPP_IS_EXPERIMENTAL(SOURCE_LOCATION)
		#include <experimental/source_location>
	#elif MFW_CPP_IS_SUPPORTED(SOURCE_LOCATION)
		#include <source_location>
	#endif
#else
	#error
#endif

#if !MFW_CPP_IS_SUPPORTED(SOURCE_LOCATION)
	#include <public/mfw/stl/string_view.hpp>
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	#if MFW_CPP_IS_EXPERIMENTAL(SOURCE_LOCATION)
	using ::MFW_STD_NAMESPACE::experimental::source_location;
	#elif MFW_CPP_IS_SUPPORTED(SOURCE_LOCATION)
	using ::MFW_STD_NAMESPACE::source_location;
	#else
		#error
	#endif
#else
	#error
#endif
}

#endif