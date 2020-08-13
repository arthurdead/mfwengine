#ifndef MFW_PUBLIC_STL_SHARED_CHAR_TRAITS_HPP
#define MFW_PUBLIC_STL_SHARED_CHAR_TRAITS_HPP

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

#if defined __cpp_char8_t || \
	defined __CHAR8_TYPE__
	#define MFW_CPP_CHAR8_SUPPORTED 1
#endif

#if MFW_OS_IS(WINDOWS) || defined _GLIBCXX_FILESYSTEM_IS_WINDOWS
	#define _MFW_STL_FILESYSTEM_WIDE_CHAR
#endif

#if defined _GLIBCXX_USE_CHAR8_T || (MFW_LIBCPP_IS(MSVC) && defined MFW_CPP_CHAR8_SUPPORTED)
	#define _MFW_STL_FILESYSTEM_CHAR8_SUPPORTED
#endif

#if MFW_OS_IS(LINUX)
	#define MFW_WCHAR_SIZE 32
	#define MFW_WCHAR_SIGNED 1
#elif MFW_IS_IS(WINDOWS)
	#define MFW_WCHAR_SIZE 16
	#define MFW_WCHAR_SIGNED 0
#else
	#error
#endif

#if MFW_OS_IS(WINDOWS) && MFW_CHARACTERSET_IS(UNICODE)
	#define MFW_OS_WIDE_CHAR
#endif

#ifdef MFW_OS_WIDE_CHAR
	#define MFW_T(x) MFW_MACRO_CONCATENATE(L, x)
#else
	#define MFW_T(x) x
#endif

#ifndef MFW_CPP_CHAR8_SUPPORTED
using char8_t = unsigned char;
#endif

namespace mfw::stl
{
	using ::MFW_STD_NAMESPACE::char_traits;

	using uchar_t = unsigned char;
	using schar_t = signed char;

#ifdef MFW_OS_WIDE_CHAR
	using oschar_t = wchar_t;
#else
	using oschar_t = char;
#endif

#ifdef _MFW_STL_FILESYSTEM_WIDE_CHAR
	using pchar_t = wchar_t;
#else
	using pchar_t = char;
#endif
}

#endif