#ifndef _MFW_PUBLIC_STL_SHARED_CHAR_TRAITS_HPP
#define _MFW_PUBLIC_STL_SHARED_CHAR_TRAITS_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

#pragma push_macro("new")
#undef new
#if MFW_STDCPP_IS(DEFAULT)
	#include <string>
	#include <filesystem>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/string.h>
	#include <filesystem>
#else
	#error
#endif
#pragma pop_macro("new")

#ifdef __cpp_char8_t
	#define MFW_CPP_CHAR8_SUPPORTED 1
#endif

#if MFW_OS_IS(WINDOWS) || defined _GLIBCXX_FILESYSTEM_IS_WINDOWS
	#define _MFW_STD_FILESYSTEM_WIDE_CHAR
#endif

#if defined _GLIBCXX_USE_CHAR8_T || (MFW_LIBCPP_IS(MSVC) && defined MFW_CPP_CHAR8_SUPPORTED)
	#define _MFW_FILESYSTEM_CHAR8_SUPPORTED
#endif

#if MFW_OS_IS(LINUX)
	#define _MFW_WCHAR_32
#endif

namespace mfw::stl
{
	using ::MFW_STD_NAMESPACE::char_traits;

#if MFW_OS_IS(WINDOWS)
	#if MFW_CHARACTERSET_IS(UNICODE)
	using oschar_t = wchar_t;
	#elif MFW_CHARACTERSET_IS(MULTIBYTE)
	using oschar_t = char;
	#else
		#error
	#endif
#elif MFW_OS_IS(LINUX)
	using oschar_t = char;
#else
	#error
#endif

#ifdef _MFW_STD_FILESYSTEM_WIDE_CHAR
	using pchar_t = wchar_t;
#else
	using pchar_t = char;
#endif
}

#endif