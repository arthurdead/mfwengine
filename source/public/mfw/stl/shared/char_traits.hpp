#ifndef __MFW_PUBLIC_STL_SHARED_CHAR_TRAITS_H
#define __MFW_PUBLIC_STL_SHARED_CHAR_TRAITS_H

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#pragma push_macro("new")
	#undef new
	#include <string>
	#include <filesystem>
	#pragma pop_macro("new")
#else
	#error
#endif

#if MFW_OS_IS(WINDOWS) || defined _GLIBCXX_FILESYSTEM_IS_WINDOWS
	#define __MFW_STD_FILESYSTEM_WIDE_CHAR
#endif

#define STR_C(x) MFW_MACRO_CONCATENATE(u8, x)

#if MFW_OS_IS(WINDOWS)
	#define STR_W(x) MFW_MACRO_CONCATENATE(u, x)
	#if MFW_CHARACTERSET == MFW_CHARACTERSET_UNICODE
		#define STR_S(x) MFW_MACRO_CONCATENATE(u, x)
	#elif MFW_CHARACTERSET == MFW_CHARACTERSET_MULTIBYTE
		#define STR_S(x) MFW_MACRO_CONCATENATE(u8, x)
	#else
		#error
	#endif
#elif MFW_OS_IS(LINUX)
	#define STR_W(x) MFW_MACRO_CONCATENATE(U, x)
	#define STR_S(x) MFW_MACRO_CONCATENATE(u8, x)
#else
	#error
#endif

#ifdef __MFW_STD_FILESYSTEM_WIDE_CHAR
	#define STR_P(x) MFW_MACRO_CONCATENATE(L, x)
#else
	#define STR_P(x) x
#endif

#ifndef MFW_CPP_CHAR8_SUPPORTED
using char8_t = char;
#endif

#if defined _GLIBCXX_USE_CHAR8_T || (MFW_LIBCPP_IS(MSVC) && defined MFW_CPP_CHAR8_SUPPORTED)
	#define __MFW_FILESYSTEM_CHAR8
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	using ::MFW_STD_NAMESPACE::char_traits;
#else
	#error
#endif

	using ucchar_t = char8_t;

#if MFW_OS_IS(WINDOWS)
	using uwchar_t = char16_t;
	#if MFW_CHARACTERSET_IS(UNICODE)
	using uschar_t = char16_t;
	#elif MFW_CHARACTERSET_IS(MULTIBYTE)
	using uschar_t = char8_t;
	#else
		#error
	#endif
#elif MFW_OS_IS(LINUX)
	using uwchar_t = char32_t;
	using uschar_t = char8_t;
#else
	#error
#endif

#ifdef __MFW_STD_FILESYSTEM_WIDE_CHAR
	using upchar_t = uwchar_t;
#else
	using upchar_t = ucchar_t;
#endif
#ifdef __MFW_FILESYSTEM_CHAR8
	using pchar8_t = char8_t;
#else
	using pchar8_t = char;
#endif
	using pchar_t = ::MFW_STD_NAMESPACE::filesystem::path::string_type::value_type;
}

#include <public/mfw/stl/detail/char_traits_funcs.hpp>

#endif