#define MFW_CPP_98 199711L
#define MFW_CPP_11 201103L
#define MFW_CPP_14 201402L
#define MFW_CPP_17 201703L

#define MFW_CPP_COMPARE(cmp, then) (MFW_CPP_VERSION cmp MFW_CPP_##then)

#if MFW_COMPILER_FLAGGED(MSVC)
	#define MFW_CPP_VERSION _MSVC_LANG
#else
	#define MFW_CPP_VERSION __cplusplus
#endif

#ifdef __has_cpp_attribute
	#define MFW_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
	#define MFW_HAS_CPP_ATTRIBUTE(x) 0
#endif

#define MFW_LIBCPP_UNIX_FLAG _MFW_BIT(1)
#define _MFW_LIBCPP_FLAGS_LAST_BIT 1
#define MFW_LIBCPP_MS (_MFW_BIT(_MFW_LIBCPP_FLAGS_LAST_BIT+1))
#define MFW_LIBCPP_GNU (MFW_LIBCPP_UNIX_FLAG|_MFW_BIT(_MFW_LIBCPP_FLAGS_LAST_BIT+2))
#define MFW_LIBCPP_LLVM (MFW_LIBCPP_UNIX_FLAG|_MFW_BIT(_MFW_LIBCPP_FLAGS_LAST_BIT+3))

#define MFW_LIBCPP_FLAGGED(what) (MFW_LIBCPP & MFW_LIBCPP_##what##_FLAG)
#define MFW_LIBCPP_IS(what) (MFW_LIBCPP == MFW_LIBCPP_##what)

#if defined _LIBCPP_VERSION || \
	defined _LIBCPP_ABI_VERSION
	#define MFW_LIBCPP MFW_LIBCPP_LLVM
#elif defined __GLIBCPP__ || \
		defined __GLIBCXX__
	#define MFW_LIBCPP MFW_LIBCPP_GNU
#elif defined _CPPLIB_VER
	#define MFW_LIBCPP MFW_LIBCPP_MS
#else
	#error
#endif

#if MFW_LIBCPP_IS(LLVM)
	#include <__config>

	#if !defined _LIBCPP_HAS_MUSL_LIBC && defined _MFW_BROWSER_DETECTED
		#define _LIBCPP_HAS_MUSL_LIBC
	#endif
	#if defined _LIBCPP_HAS_MUSL_LIBC && !defined __MUSL__
		#define __MUSL__
	#endif
#endif

#define MFW_STDCPP_DEFAULT 0
#define MFW_STDCPP_EA 1

#define MFW_STDCPP_IS(what) (MFW_STDCPP == MFW_STDCPP_##what)

#ifdef _MFW_USE_EASTL
	#define MFW_STDCPP MFW_STDCPP_EA
	#define MFW_STD_NAMESPACE eastl
#else
	#define MFW_STDCPP MFW_STDCPP_DEFAULT
	#define MFW_STD_NAMESPACE std
#endif

#define MFW_CPP_IS_SUPPORTED(what) (defined(MFW_CPP_##what##_SUPPORTED) && (MFW_CPP_##what##_SUPPORTED == 1))
#define MFW_CPP_IS_EXPERIMENTAL(what) (defined(MFW_CPP_##what##_SUPPORTED) && (MFW_CPP_##what##_SUPPORTED == 2))

#if MFW_HAS_FEATURE(modules)
	#define MFW_CPP_MODULES_SUPPORTED
#endif

#if MFW_HAS_FEATURE(cxx_concepts) || \
	defined __cpp_lib_concepts || \
	MFW_HAS_INCLUDE(<concepts>) || \
	defined __cpp_concepts
	#define MFW_CPP_CONCEPTS_SUPPORTED 1
#elif defined __cpp_experimental_concepts
	#define MFW_CPP_CONCEPTS_SUPPORTED 2
#endif

#ifdef __cpp_if_constexpr
	#define MFW_CPP_IF_CONSTEXPR_SUPPORTED 1
#endif