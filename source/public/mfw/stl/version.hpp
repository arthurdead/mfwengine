#ifndef __MFW_PUBLIC_STL_VERSION_H
#define __MFW_PUBLIC_STL_VERSION_H

#pragma once

#ifdef __has_include
	#define MFW_HAS_INCLUDE(...) __has_include(__VA_ARGS__)
#else
	#define MFW_HAS_INCLUDE(...) 0
#endif

#ifdef __cplusplus
	#define MFW_CPP
#else
	#define MFW_C
#endif

#ifdef MFW_CPP
	#include <version>
#endif

#if MFW_HAS_INCLUDE(<features.h>)
	#include <features.h>
#endif

#if defined __INTELLISENSE__ || defined __cpp_check
	#define MFW_INTELLISENSE
#endif

#define __MFW_BIT(i) (1 << (i))

#define ____MFW_STRINGFY_IMPL(x) #x
#define __MFW_STRINGFY(x) ____MFW_STRINGFY_IMPL(x)
#define __MFW_FILE_LINE_STRING __FILE__ "(" __MFW_STRINGFY(__LINE__) "): "

#define __MFW_MACRO_CONCATENATE_IMPL(x, y) x##y
#define __MFW_MACRO_CONCATENATE(x, y) __MFW_MACRO_CONCATENATE_IMPL(x, y)

#define MFW_COMPILER_UNIX_FLAG __MFW_BIT(0)
#define __MFW_COMPILER_FLAGS_LAST_BIT 0
#define MFW_COMPILER_MSVC (__MFW_BIT(__MFW_COMPILER_FLAGS_LAST_BIT+1))
#define MFW_COMPILER_CLANG (MFW_COMPILER_UNIX_FLAG|__MFW_BIT(__MFW_COMPILER_FLAGS_LAST_BIT+2))
#define MFW_COMPILER_GCC (MFW_COMPILER_UNIX_FLAG|__MFW_BIT(__MFW_COMPILER_FLAGS_LAST_BIT+3))

#define MFW_COMPILER_FLAGGED(what) (MFW_COMPILER & MFW_COMPILER_##what##_FLAG)
#define MFW_COMPILER_IS(what) (MFW_COMPILER == MFW_COMPILER_##what)

#if defined __clang__ || defined __clang_major__ || defined __clang_minor__ || defined __clang_patchlevel__ || defined __clang_version__
	#define MFW_COMPILER MFW_COMPILER_CLANG
#elif defined __GNUC__ || defined __GNUC_MINOR__ || defined __GNUC_PATCHLEVEL__ || defined __GNUG__ || defined __GNUC_VERSION__
	#define MFW_COMPILER MFW_COMPILER_GCC
#elif defined _MSC_FULL_VER || defined _MSC_VER || defined _MSVC_LANG || defined _MSC_BUILD
	#define MFW_COMPILER MFW_COMPILER_MSVC
#elif defined __cpp_check
	#define MFW_COMPILER MFW_COMPILER_GCC
#else
	#error
#endif

#ifdef MFW_CPP
	#define MFW_CPP_11 201103L
	#define MFW_CPP_14 201402L
	#define MFW_CPP_17 201703L
	
	#define MFW_CPP_COMPARE(cmp, then) (MFW_CPP_VERSION cmp MFW_CPP_##then)

	#if MFW_COMPILER_IS(MSVC)
		#define MFW_CPP_VERSION _MSVC_LANG
	#else
		#define MFW_CPP_VERSION __cplusplus
	#endif
#endif

#if MFW_COMPILER_IS(MSVC)
	#define __MFW_PRAGMA(...) __pragma(__VA_ARGS__)
#elif MFW_COMPILER_FLAGGED(UNIX)
	#define __MFW_PRAGMA(...) _Pragma(__MFW_STRINGFY(__VA_ARGS__))
#else
	#error
#endif

#define __MFW_DISABLE_COMPILE_MESSAGES
#ifndef __MFW_DISABLE_COMPILE_MESSAGES
	#define __MFW_MESSAGE(text) __MFW_PRAGMA(message(__MFW_FILE_LINE_STRING text))
#else
	#define __MFW_MESSAGE(...) 
#endif

#define MFW_CONFIGURATION_RELEASE 0
#define MFW_CONFIGURATION_DEBUG 1

#define MFW_CONFIGURATION_IS(what) (MFW_CONFIGURATION == MFW_CONFIGURATION_##what)

#if defined _DEBUG || defined DEBUG
	#undef DEBUG
	#define MFW_CONFIGURATION MFW_CONFIGURATION_DEBUG
#elif defined _NDEBUG || defined NDEBUG
	#define MFW_CONFIGURATION MFW_CONFIGURATION_RELEASE
#else
	#define MFW_CONFIGURATION MFW_CONFIGURATION_RELEASE
#endif

#define MFW_PROCESSOR_64BITS_FLAG __MFW_BIT(0)
#define MFW_PROCESSOR_32BITS_FLAG __MFW_BIT(1)
#define MFW_PROCESSOR_X86_FLAG __MFW_BIT(2)
#define MFW_PROCESSOR_ARM_FLAG __MFW_BIT(3)
#define __MFW_PROCESSOR_FLAGS_LAST_BIT 3
#define MFW_PROCESSOR_X86_64 (MFW_PROCESSOR_X86_FLAG|MFW_PROCESSOR_64BITS_FLAG|__MFW_BIT(__MFW_PROCESSOR_FLAGS_LAST_BIT+1))
#define MFW_PROCESSOR_X86 (MFW_PROCESSOR_X86_FLAG|MFW_PROCESSOR_32BITS_FLAG|__MFW_BIT(__MFW_PROCESSOR_FLAGS_LAST_BIT+2))
#define MFW_PROCESSOR_ARMV7 (MFW_PROCESSOR_ARM_FLAG|MFW_PROCESSOR_32BITS_FLAG|__MFW_BIT(__MFW_PROCESSOR_FLAGS_LAST_BIT+3))
#define MFW_PROCESSOR_ARMV8 (MFW_PROCESSOR_ARM_FLAG|MFW_PROCESSOR_64BITS_FLAG|__MFW_BIT(__MFW_PROCESSOR_FLAGS_LAST_BIT+4))

#define MFW_PROCESSOR_FLAGGED(what) (MFW_PROCESSOR & MFW_PROCESSOR_##what##_FLAG)
#define MFW_PROCESSOR_IS(what) (MFW_PROCESSOR == MFW_PROCESSOR_##what)

#if defined _M_X64 || defined _M_AMD64 || defined __x86_64__ || defined __x86_64 || defined __amd64 || defined __amd64__
	#define MFW_PROCESSOR MFW_PROCESSOR_X86_64
#else
	#error
#endif

#define MFW_OS_WINDOWS 0
#define MFW_OS_LINUX 1

#define MFW_OS_IS(what) (MFW_OS == MFW_OS_##what)

#if defined __linux__ || defined linux || defined __linux || defined __gnu_linux__ || defined __GNU__ || defined __unix__ || defined __unix
	#define MFW_OS MFW_OS_LINUX
#elif defined _WIN64 || defined WIN64 || defined _WIN32 || defined WIN32 || defined _WINDOWS || defined WINDOWS || defined __WIN32__ || defined __WINDOWS__
	#undef WINDOWS
	#define MFW_OS MFW_OS_WINDOWS
#else
	#error
#endif

#define MFW_PLATFORM_DESKTOP 0
#define MFW_PLATFORM_MOBILE 1

#define MFW_PLATFORM_IS(what) (MFW_PLATFORM == MFW_PLATFORM_##what)

#if MFW_OS_IS(WINDOWS)
	#define MFW_PLATFORM MFW_PLATFORM_DESKTOP
#elif MFW_OS_IS(LINUX)
	#if defined __ANDROID__ || defined __ANDROID_API__
		#define MFW_PLATFORM MFW_PLATFORM_MOBILE
	#else
		#define MFW_PLATFORM MFW_PLATFORM_DESKTOP
	#endif
#else
	#error
#endif

#define MFW_CHARACTERSET_MULTIBYTE 0
#define MFW_CHARACTERSET_UNICODE 1

#define MFW_CHARACTERSET_IS(what) (MFW_CHARACTERSET == MFW_CHARACTERSET_##what)

#if defined _UNICODE || defined UNICODE
	#undef UNICODE
	#define MFW_CHARACTERSET MFW_CHARACTERSET_UNICODE
#elif defined _MBCS || defined MBCS
	#define MFW_CHARACTERSET MFW_CHARACTERSET_MULTIBYTE
#else
	#if MFW_OS_IS(WINDOWS)
		#define MFW_CHARACTERSET MFW_CHARACTERSET_UNICODE
	#elif MFW_OS_IS(LINUX)
		#define MFW_CHARACTERSET MFW_CHARACTERSET_MULTIBYTE
	#else
		#error
	#endif
#endif

#define MFW_BUILD_EXECUTABLE_FLAG __MFW_BIT(0)
#define MFW_BUILD_STATIC_FLAG __MFW_BIT(1)
#define MFW_BUILD_SHARED_FLAG __MFW_BIT(2)
#define __MFW_BUILD_FLAGS_LAST_BIT 2
#define MFW_BUILD_EMBEDDED (MFW_BUILD_STATIC_FLAG|__MFW_BIT(__MFW_BUILD_FLAGS_LAST_BIT+1))
#define MFW_BUILD_STATIC (MFW_BUILD_STATIC_FLAG|__MFW_BIT(__MFW_BUILD_FLAGS_LAST_BIT+2))
#define MFW_BUILD_SHARED (MFW_BUILD_SHARED_FLAG|__MFW_BIT(__MFW_BUILD_FLAGS_LAST_BIT+3))
#define MFW_BUILD_EXECUTABLE (MFW_BUILD_SHARED_FLAG|MFW_BUILD_EXECUTABLE_FLAG|__MFW_BIT(__MFW_BUILD_FLAGS_LAST_BIT+4))
#if MFW_OS_IS(WINDOWS)
	#define MFW_BUILD_EXECUTABLE_WINDOWS (MFW_BUILD_SHARED_FLAG|MFW_BUILD_EXECUTABLE_FLAG|__MFW_BIT(__MFW_BUILD_FLAGS_LAST_BIT+5))
#endif

#define MFW_BUILD_FLAGGED(what) (MFW_BUILD & MFW_BUILD_##what##_FLAG)
#define MFW_BUILD_IS(what) (MFW_BUILD == MFW_BUILD_##what)

#if defined _LIB || defined LIB
	#define MFW_BUILD MFW_BUILD_STATIC
#elif defined _SHARED || defined SHARED
	#define MFW_BUILD MFW_BUILD_SHARED
#elif defined _CONSOLE || defined CONSOLE
	#define MFW_BUILD MFW_BUILD_EXECUTABLE
#endif
#if MFW_OS_IS(WINDOWS) && !defined MFW_BUILD
	#if defined _WINDLL || defined WINDLL || defined _USRDLL || defined USRDLL
		#define MFW_BUILD MFW_BUILD_SHARED
	#elif defined _WINDOWS || defined WINDOWS
		#define MFW_BUILD MFW_BUILD_EXECUTABLE_WINDOWS
	#endif
#endif
#ifndef MFW_BUILD
	#define MFW_BUILD MFW_BUILD_EXECUTABLE
#endif

#define MFW_LIBC_HEADERS_CONFORMING_FLAG __MFW_BIT(0)
#define MFW_LIBC_NAMESPACE_CONFORMING_FLAG __MFW_BIT(1)
#define MFW_LIBC_API_CONFORMING_FLAG __MFW_BIT(2)
#define MFW_LIBC_UNIX_FLAG __MFW_BIT(3)
#define __MFW_LIBC_FLAGS_LAST_BIT 3
#define __MFW_LIBC_ISO_FLAGS MFW_LIBC_HEADERS_CONFORMING_FLAG|MFW_LIBC_NAMESPACE_CONFORMING_FLAG|MFW_LIBC_API_CONFORMING_FLAG
#define MFW_LIBC_DEFAULT (__MFW_LIBC_ISO_FLAGS|__MFW_BIT(__MFW_LIBC_FLAGS_LAST_BIT+1))
#define MFW_LIBC_MS (__MFW_LIBC_ISO_FLAGS|__MFW_BIT(__MFW_LIBC_FLAGS_LAST_BIT+2))
#define MFW_LIBC_GNU (MFW_LIBC_UNIX_FLAG|__MFW_LIBC_ISO_FLAGS|__MFW_BIT(__MFW_LIBC_FLAGS_LAST_BIT+3))
#define MFW_LIBC_MUSL (MFW_LIBC_UNIX_FLAG|__MFW_LIBC_ISO_FLAGS|__MFW_BIT(__MFW_LIBC_FLAGS_LAST_BIT+4))

#define MFW_LIBC_FLAGGED(what) (MFW_LIBC & MFW_LIBC_##what##_FLAG)
#define MFW_LIBC_IS(what) (MFW_LIBC == MFW_LIBC_##what)

#if defined __MUSL__ || defined __MUSL_VER_MAJOR__ || defined __MUSL_VER_MINOR__ || defined __MUSL_VER_PATCH__
	#define MFW_LIBC MFW_LIBC_MUSL
#elif defined __GNU_LIBRARY__ || defined __GNU_LIBRARY_MINOR__ || defined __GLIBC__ || defined __GLIBC_MINOR__
	#define MFW_LIBC MFW_LIBC_GNU
#else
	#define MFW_LIBC MFW_LIBC_DEFAULT
#endif

#ifdef MFW_CPP
	#define MFW_LIBCPP_HEADERS_CONFORMING_FLAG __MFW_BIT(0)
	#define MFW_LIBCPP_NAMESPACE_CONFORMING_FLAG __MFW_BIT(1)
	#define MFW_LIBCPP_API_CONFORMING_FLAG __MFW_BIT(2)
	#define MFW_LIBCPP_UNIX_FLAG __MFW_BIT(3)
	#define __MFW_LIBCPP_FLAGS_LAST_BIT 3
	#define __MFW_LIBCPP_ISO_FLAGS MFW_LIBCPP_HEADERS_CONFORMING_FLAG|MFW_LIBCPP_NAMESPACE_CONFORMING_FLAG|MFW_LIBCPP_API_CONFORMING_FLAG
	#define MFW_LIBCPP_DEFAULT (__MFW_LIBCPP_ISO_FLAGS|__MFW_BIT(__MFW_LIBCPP_FLAGS_LAST_BIT+1))
	#define MFW_LIBCPP_MS (__MFW_LIBCPP_ISO_FLAGS|__MFW_BIT(__MFW_LIBCPP_FLAGS_LAST_BIT+2))
	#define MFW_LIBCPP_GNU (MFW_LIBCPP_UNIX_FLAG|__MFW_LIBCPP_ISO_FLAGS|__MFW_BIT(__MFW_LIBCPP_FLAGS_LAST_BIT+3))
	#define MFW_LIBCPP_LLVM (MFW_LIBCPP_UNIX_FLAG|__MFW_LIBCPP_ISO_FLAGS|__MFW_BIT(__MFW_LIBCPP_FLAGS_LAST_BIT+4))

	#define MFW_LIBCPP_FLAGGED(what) (MFW_LIBCPP & MFW_LIBCPP_##what##_FLAG)
	#define MFW_LIBCPP_IS(what) (MFW_LIBCPP == MFW_LIBCPP_##what)

	#if defined _LIBCPP_VERSION || defined _LIBCPP_ABI_VERSION
		#define MFW_LIBCPP MFW_LIBCPP_LLVM
	#elif defined __GLIBCPP__ || defined __GLIBCXX__
		#define MFW_LIBCPP MFW_LIBCPP_GNU
	#elif defined _CPPLIB_VER
		#define MFW_LIBCPP MFW_LIBCPP_MS
	#else
		#define MFW_LIBCPP MFW_LIBCPP_DEFAULT
	#endif

	#define MFW_STD_HEADERS_CONFORMING_FLAG __MFW_BIT(0)
	#define MFW_STD_NAMESPACE_CONFORMING_FLAG __MFW_BIT(1)
	#define MFW_STD_API_CONFORMING_FLAG __MFW_BIT(2)
	#define MFW_STD_UNIX_FLAG __MFW_BIT(3)
	#define __MFW_STD_FLAGS_LAST_BIT 3
	#define __MFW_STD_ISO_FLAGS MFW_STD_HEADERS_CONFORMING_FLAG|MFW_STD_NAMESPACE_CONFORMING_FLAG|MFW_STD_API_CONFORMING_FLAG
	#define MFW_STD_DEFAULT (__MFW_STD_ISO_FLAGS|__MFW_BIT(__MFW_STD_FLAGS_LAST_BIT+1))
	#define MFW_STD_EA (__MFW_BIT(__MFW_STD_FLAGS_LAST_BIT+2))
	#define MFW_STD_MS (__MFW_STD_ISO_FLAGS|__MFW_BIT(__MFW_STD_FLAGS_LAST_BIT+3))
	#define MFW_STD_GNU (MFW_STD_UNIX_FLAG|__MFW_STD_ISO_FLAGS|__MFW_BIT(__MFW_STD_FLAGS_LAST_BIT+4))
	#define MFW_STD_LLVM (MFW_STD_UNIX_FLAG|__MFW_STD_ISO_FLAGS|__MFW_BIT(__MFW_STD_FLAGS_LAST_BIT+5))

	#define MFW_STD_FLAGGED(what) (MFW_STD & MFW_STD_##what##_FLAG)
	#define MFW_STD_IS(what) (MFW_STD == MFW_STD_##what)

	#if MFW_HAS_INCLUDE(<EASTL/version.h>)
		#define MFW_STD MFW_STD_EA
	#elif MFW_LIBCPP_IS(LLVM)
		#define MFW_STD MFW_STD_LLVM
	#elif MFW_LIBCPP_IS(GNU)
		#define MFW_STD MFW_STD_GNU
	#elif MFW_LIBCPP_IS(MS)
		#define MFW_STD MFW_STD_MS
	#else
		#define MFW_STD MFW_STD_DEFAULT
	#endif

	#if MFW_STD_FLAGGED(NAMESPACE_CONFORMING)
		#define MFW_STD_NAMESPACE std
	#elif MFW_STD_IS(EA)
		#define MFW_STD_NAMESPACE eastl
	#else
		#error
	#endif

namespace MFW_STD_NAMESPACE {}
#endif

#ifdef __has_feature
	#define MFW_HAS_FEATURE(...) __has_feature(__VA_ARGS__)
#else
	#define MFW_HAS_FEATURE(...) 0
#endif

#ifdef __has_extension
	#define MFW_HAS_EXTENSION(...) __has_extension(__VA_ARGS__)
#else
	#define MFW_HAS_EXTENSION(...) 0
#endif

#ifdef __has_builtin
	#define MFW_HAS_BUILTIN(...) __has_builtin(__VA_ARGS__)
#else
	#define MFW_HAS_BUILTIN(...) 0
#endif

#ifdef MFW_CPP
	#ifdef __has_cpp_attribute
		#define MFW_HAS_CPP_ATTRIBUTE(...) __has_cpp_attribute(__VA_ARGS__)
	#else
		#define MFW_HAS_CPP_ATTRIBUTE(...) 0
	#endif

	#define MFW_CPP_EXPERIMENTAL_FLAG __MFW_BIT(0)
	#define __MFW_CPP_FLAGS_LAST_BIT 0
	
	#define MFW_CPP_IS_SUPPORTED(what) (defined(MFW_CPP_##what##_SUPPORTED))
	#define MFW_CPP_IS_EXPERIMENTAL(what) (defined(MFW_CPP_##what##_SUPPORTED) && (MFW_CPP_##what##_SUPPORTED & MFW_CPP_EXPERIMENTAL_FLAG))

	#if MFW_HAS_FEATURE(cxx_rtti) || defined __cpp_rtti || defined __GXX_RTTI || defined _CPPRTTI
		#define MFW_CPP_RTTI_SUPPORTED (__MFW_BIT(__MFW_CPP_FLAGS_LAST_BIT+1))
	#endif
	
	/*
	#if MFW_CONFIGURATION_IS(RELEASE)
		#if MFW_HAS_FEATURE(cxx_exceptions) || defined __cpp_exceptions || defined __EXCEPTIONS || defined _HAS_EXCEPTIONS
			#error
		#endif
	#endif
	*/

	#if MFW_HAS_FEATURE(modules)
		#define MFW_CPP_MODULES_SUPPORTED (__MFW_BIT(__MFW_CPP_FLAGS_LAST_BIT+2))
	#endif

	#if MFW_HAS_FEATURE(cxx_concepts) || defined __cpp_lib_concepts || MFW_HAS_INCLUDE(<concepts>) || defined __cpp_concepts
		#define MFW_CPP_CONCEPTS_SUPPORTED (__MFW_BIT(__MFW_CPP_FLAGS_LAST_BIT+3))
	#elif defined __cpp_experimental_concepts
		#define MFW_CPP_CONCEPTS_SUPPORTED (__MFW_BIT(__MFW_CPP_FLAGS_LAST_BIT+3)|MFW_CPP_EXPERIMENTAL_FLAG)
	#endif

	#if defined __cpp_impl_destroying_delete || defined __cpp_lib_destroying_delete
		#define MFW_CPP_DESTROYING_DELETE_SUPPORTED (__MFW_BIT(__MFW_CPP_FLAGS_LAST_BIT+4))
	#endif

	#if MFW_HAS_INCLUDE(<source_location>)
		#define MFW_CPP_SOURCE_LOCATION_SUPPORTED (__MFW_BIT(__MFW_CPP_FLAGS_LAST_BIT+5))
	#elif MFW_HAS_INCLUDE(<experimental/source_location>) || defined __cpp_lib_experimental_source_location 
		#define MFW_CPP_SOURCE_LOCATION_SUPPORTED (__MFW_BIT(__MFW_CPP_FLAGS_LAST_BIT+5)|MFW_CPP_EXPERIMENTAL_FLAG)
	#endif

	#ifdef __cpp_char8_t
		#define MFW_CPP_CHAR8_SUPPORTED (__MFW_BIT(__MFW_CPP_FLAGS_LAST_BIT+6))
	#endif
	
	#ifdef __cpp_if_constexpr
		#define MFW_CPP_IF_CONSTEXPR_SUPPORTED (__MFW_BIT(__MFW_CPP_FLAGS_LAST_BIT+7))
	#endif
#endif

#ifdef MFW_CPP
namespace mfw
{
	namespace stl {}

	__MFW_MESSAGE("hmmmmmmmm")
	using namespace stl;
}
#endif

#endif