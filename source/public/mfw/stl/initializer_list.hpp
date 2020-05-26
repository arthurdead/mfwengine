#ifndef __MFW_PUBLIC_STL_INITIALIZER_LIST_H
#define __MFW_PUBLIC_STL_INITIALIZER_LIST_H

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <initializer_list>
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	using ::MFW_STD_NAMESPACE::initializer_list;
#else
	#error
#endif
}

#endif