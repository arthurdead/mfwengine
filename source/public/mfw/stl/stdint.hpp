#ifndef _MFW_PUBLIC_STL_STDINT_HPP
#define _MFW_PUBLIC_STL_STDINT_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/type_traits.hpp>

#if MFW_STDC_IS(DEFAULT)
	#include <cstdint>
	#include <cstddef>
	#include <cstdbool>
#else
	#error
#endif

#if MFW_STDC_IS(EA)
	#include <EAStdC/Int128_t.h>
#endif

#if MFW_LIBC_FLAGGED(UNIX)
	#include <sys/types.h>
#endif

#if MFW_COMPILER_FLAGGED(UNIX) || MFW_STDC_IS(EA)
	#define MFW_INT128_SUPPORTED
#endif

#if MFW_OS_IS(LINUX)
	#define MFW_LONG_SIZE 64
#elif MFW_OS_IS(WINDOWS)
	#define MFW_LONG_SIZE 32
#else
	#error
#endif

#define MFW_INT8_MIN INT8_MIN
#define MFW_INT8_MAX INT8_MAX
#define MFW_UINT8_MAX UINT8_MAX

#define MFW_INT16_MIN INT16_MIN
#define MFW_INT16_MAX INT16_MAX
#define MFW_UINT16_MAX UINT16_MAX

#define MFW_INT32_MIN INT32_MIN
#define MFW_INT32_MAX INT32_MAX
#define MFW_UINT32_MAX UINT32_MAX

#define MFW_INT64_MIN INT64_MIN
#define MFW_INT64_MAX INT64_MAX
#define MFW_UINT64_MAX UINT64_MAX

#define MFW_INT128_MIN INT128_MIN
#define MFW_INT128_MAX INT128_MAX
#define MFW_UINT128_MAX UINT128_MAX

namespace mfw::stl
{
	using ldouble_t = long double;
	using uchar_t = unsigned char;
	using schar_t = signed char;
	using ushort_t = unsigned short;
	using sshort_t = signed short;
	using uint_t = unsigned int;
	using sint_t = signed int;
	using ulong_t = unsigned long;
	using slong_t = signed long;
	using ullong_t = unsigned long long;
	using sllong_t = signed long long;

	enum class radix_t : uchar_t
	{
		binary = 2,
		octal = 8,
		decimal = 10,
		duodecimal = 12,
		hexadecimal = 16,
		vigesimal = 20,
		sexagesimal = 60,
	};

#if MFW_LIBC_FLAGGED(UNIX)
	using ::ssize_t;
#else
	using ssize_t = make_signed_t<::MFW_STD_NAMESPACE::size_t>;
#endif

	using ::MFW_STD_NAMESPACE::size_t;
	using ::MFW_STD_NAMESPACE::ptrdiff_t;
	using ::MFW_STD_NAMESPACE::int16_t;
	using ::MFW_STD_NAMESPACE::uint16_t;
	using ::MFW_STD_NAMESPACE::int32_t;
	using ::MFW_STD_NAMESPACE::uint32_t;
	using ::MFW_STD_NAMESPACE::int64_t;
	using ::MFW_STD_NAMESPACE::uint64_t;

#if MFW_STDC_IS(EA)
	using ::EA::StdC::int128_t;
	using ::EA::StdC::uint128_t;
#elif MFW_COMPILER_FLAGGED(UNIX)
	MFW_EXTENSION using int128_t = __int128;
	MFW_EXTENSION using uint128_t = unsigned __int128;
#else
	using int128_t = int64_t;
	using int128_t = uint64_t;
#endif
	enum int8_t : schar_t {};
	enum uint8_t : uchar_t {};
	using byte = uint8_t;
	MFW_ENUM_INT(int8_t)
	MFW_ENUM_INT(uint8_t)
}

#include <public/mfw/stl/internal/stdint_funcs.hpp>

#endif