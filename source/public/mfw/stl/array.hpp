#ifndef __MFW_PUBLIC_STL_ARRAY_H
#define __MFW_PUBLIC_STL_ARRAY_H

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <array>
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	template <typename T, size_t S>
	using array = ::MFW_STD_NAMESPACE::array<T, S>;
#else
	#error
#endif
}

#endif