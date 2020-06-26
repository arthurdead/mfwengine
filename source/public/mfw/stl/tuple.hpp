#ifndef __MFW_PUBLIC_STL_TUPLE_H
#define __MFW_PUBLIC_STL_TUPLE_H

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <tuple>
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	using ::MFW_STD_NAMESPACE::tuple;
	using ::MFW_STD_NAMESPACE::tuple_element;
	using ::MFW_STD_NAMESPACE::tuple_element_t;
	using ::MFW_STD_NAMESPACE::get;
	using ::MFW_STD_NAMESPACE::tie;
#else
	#error
#endif
}

#include <public/mfw/stl/detail/tuple_funcs.hpp>

#endif