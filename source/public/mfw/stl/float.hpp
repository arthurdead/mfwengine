#ifndef _MFW_PUBLIC_STL_FLOAT_HPP
#define _MFW_PUBLIC_STL_FLOAT_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

#include <cfloat>
#if MFW_LIBC_FLAGGED(UNIX)
	#include <bits/floatn.h>
#endif

#if (defined __GLIBC_USE_IEC_60559_TYPES_EXT && __GLIBC_USE_IEC_60559_TYPES_EXT) || defined __STDC_WANT_IEC_60559_TYPES_EXT__
	#define _MFW_IEC_60559_EXT_TYPES
#endif

#if (defined __STDC_IEC_559__ && __STDC_IEC_559__)
	#define MFW_IEC_60559
#endif

#if (defined __HAVE_FLOAT16) && __HAVE_FLOAT16 || (defined __HAVE_DISTINCT_FLOAT16 && __HAVE_DISTINCT_FLOAT16)
	#define MFW_FLOAT16_SUPPORTED
#endif

#if (defined __HAVE_FLOAT128 && __HAVE_FLOAT128) || (defined __HAVE_DISTINCT_FLOAT128 && __HAVE_DISTINCT_FLOAT128)
	#define MFW_FLOAT128_SUPPORTED
#endif

#ifdef MFW_FLOAT16_SUPPORTED
	#define MFW_FLT16_MIN FLT16_MIN
	#define MFW_FLT16_MAX FLT16_MAX
#else
	#define MFW_FLT16_MIN FLT_MIN
	#define MFW_FLT16_MAX FLT_MAX
#endif
#define MFW_FLT32_MIN FLT_MIN
#define MFW_FLT32_MAX FLT_MAX
#define MFW_FLT64_MIN DBL_MIN
#define MFW_FLT64_MAX DBL_MAX
#define MFW_FLT80_MIN LDBL_MIN
#define MFW_FLT80_MAX LDBL_MAX
#ifdef MFW_FLOAT128_SUPPORTED
	#define MFW_FLT128_MIN FLT128_MIN
	#define MFW_FLT128_MAX FLT128_MAX
#else
	#define MFW_FLT128_MIN LDBL_MIN
	#define MFW_FLT128_MAX LDBL_MAX
#endif

namespace mfw::stl
{
#if MFW_LIBC_IS(MS)
	using float16_t = float;
	using float32_t = float;
	using float64_t = double;
	using float80_t = long double;
	using float128_t = long double;
#elif MFW_LIBC_FLAGGED(UNIX)
	#ifdef MFW_FLOAT16_SUPPORTED
	using float16_t = _Float16;
	#else
	using float16_t = _Float32;
	#endif
	using float32_t = _Float32;
	using float64_t = _Float64;
	#if MFW_COMPILER_IS(GCC)
	using float80_t = __float80;
	#else
	using float80_t = long double;
	#endif
	#ifdef MFW_FLOAT128_SUPPORTED
	using float128_t = _Float128;
	#else
	using float128_t = float80_t;
	#endif
#else
	#error
#endif
}

#include <public/mfw/stl/internal/float_funcs.hpp>

#endif