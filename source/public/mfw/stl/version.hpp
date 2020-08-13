#ifndef MFW_PUBLIC_STL_VERSION_HPP
#define MFW_PUBLIC_STL_VERSION_HPP

#pragma once

#define _MFW_BIT(i) (1 << (i))

#define _MFW_STRINGFY_IMPL(x) #x
#define _MFW_STRINGFY(x) _MFW_STRINGFY_IMPL(x)
#define _MFW_FILE_LINE_STRING __FILE__ "(" _MFW_STRINGFY(__LINE__) "): "

#define _MFW_MACRO_CONCATENATE_IMPL(x, y) x##y
#define _MFW_MACRO_CONCATENATE(x, y) _MFW_MACRO_CONCATENATE_IMPL(x, y)

#include <public/mfw/stl/internal/version_compiler.hpp>

#if MFW_COMPILER_IS(MSVC)
	#if defined _MSVC_TRADITIONAL && !_MSVC_TRADITIONAL
		#define MFW_MSVC_NEW_PREPROCESSOR
	#endif
#endif

#if MFW_COMPILER_FLAGGED(MSVC)
	#if MFW_COMPILER_FLAGGED(CLANG)
		#define _MFW_PRAGMA_UNIX(x) _Pragma(_MFW_STRINGFY(x))
	#endif
	#ifdef MFW_MSVC_NEW_PREPROCESSOR
		#define _MFW_PRAGMA(x) _Pragma(_MFW_STRINGFY(x))
	#else
		#define _MFW_PRAGMA(x) __pragma(x)
	#endif
#elif MFW_COMPILER_FLAGGED(UNIX)
	#define _MFW_PRAGMA(x) _Pragma(_MFW_STRINGFY(x))
	#define _MFW_PRAGMA_UNIX _MFW_PRAGMA
#else
	#error
#endif

#define _MFW_MESSAGE(text) _MFW_PRAGMA(message(_MFW_FILE_LINE_STRING text))

#include <public/mfw/stl/internal/version_config.hpp>
#include <public/mfw/stl/internal/version_os.hpp>

#if defined __cplusplus || \
	defined _MSVC_LANG
	#define MFW_CPP
	#define MFW_C
#elif defined __ASSEMBLER__
	#define MFW_ASM
#elif defined __STDC__ || \
		defined __STDC_VERSION__ || \
		defined __STDC_HOSTED__
	#define MFW_C
#endif

#if defined __INTELLISENSE__ || \
	defined __cpp_check
	#define MFW_INTELLISENSE
#endif

#include <public/mfw/stl/internal/pre_std.hpp>

#ifdef MFW_CPP
	#include <version>
#endif

#ifdef MFW_C
	#if MFW_OS_IS(LINUX)
		#include <features.h>
	#endif
#endif

#ifdef __has_feature
	#define MFW_HAS_FEATURE(x) __has_feature(x)
#else
	#define MFW_HAS_FEATURE(x) 0
#endif

#ifdef __has_extension
	#define MFW_HAS_EXTENSION(x) __has_extension(x)
#else
	#define MFW_HAS_EXTENSION(x) 0
#endif

#ifdef __has_builtin
	#define MFW_HAS_BUILTIN(x) __has_builtin(x)
#else
	#define MFW_HAS_BUILTIN(x) 0
#endif

#ifdef __has_include
	#define MFW_HAS_INCLUDE(x) __has_include(x)
#else
	#define MFW_HAS_INCLUDE(x) 0
#endif

#ifdef MFW_CPP
	#include <public/mfw/stl/internal/version_cpp.hpp>
#endif
#ifdef MFW_C
	#include <public/mfw/stl/internal/version_c.hpp>
#endif
#include <public/mfw/stl/internal/version_processor.hpp>
#include <public/mfw/stl/internal/version_build.hpp>

#ifdef MFW_C
	#if MFW_C_COMPARE(>=, 99)
		#define MFW_VA_MACROS_SUPPORTED
	#endif
#endif

#if MFW_COMPILER_IS(MSVC)
	#ifdef MFW_C
		#ifdef MFW_MSVC_NEW_PREPROCESSOR
			#define MFW_VA_OPT_SUPPORTED
		#endif
	#endif
#elif MFW_COMPILER_FLAGGED(UNIX)
	#ifdef MFW_CPP
		#if MFW_CPP_COMPARE(>, 17)
			#define MFW_VA_OPT_SUPPORTED
		#endif
	#endif
#endif

#endif