#ifndef _MFW_PUBLIC_STL_FLOAT_HPP
#define _MFW_PUBLIC_STL_FLOAT_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

#include <cfloat>
#if MFW_COMPILER_FLAGGED(UNIX) && !MFW_LIBC_IS(MUSL)
	#include <bits/floatn.h>
#endif

#if (defined(__HAVE_FLOAT16) && __HAVE_FLOAT16) || (defined(__HAVE_DISTINCT_FLOAT16) && __HAVE_DISTINCT_FLOAT16) || defined __STDC_WANT_IEC_60559_TYPES_EXT__
	#define __MFW_FLOAT16_UNIQUE
#endif

#if 1 || MFW_COMPILER_IS(GCC) || defined __STDC_WANT_IEC_60559_TYPES_EXT__
	#define __MFW_FLOAT80_UNIQUE
#endif

#if MFW_COMPILER_IS(GCC) || (defined(__HAVE_FLOAT128) && __HAVE_FLOAT128) || (defined(__HAVE_DISTINCT_FLOAT128) && __HAVE_DISTINCT_FLOAT128) || defined __STDC_WANT_IEC_60559_TYPES_EXT__
	#define __MFW_FLOAT128_UNIQUE
#endif

#ifdef __STDC_WANT_IEC_60559_TYPES_EXT__
	#define MFW_FLT16_MAX FLT16_MAX
	#define MFW_FLT32_MAX FLT32_MAX
	#define MFW_FLT64_MAX FLT64_MAX
	#define MFW_FLT80_MAX FLT128_MAX
	#define MFW_FLT128_MAX FLT128_MAX
#else
	#define MFW_FLT16_MAX FLT_MAX
	#define MFW_FLT32_MAX FLT_MAX
	#define MFW_FLT64_MAX DBL_MAX
	#define MFW_FLT80_MAX LDBL_MAX
	#define MFW_FLT128_MAX LDBL_MAX
#endif

namespace mfw::stl
{
#if MFW_COMPILER_FLAGGED(MSVC) || MFW_LIBC_IS(MUSL)
	using float16_t = float;
	using float32_t = float;
	using float64_t = double;
	using float80_t = long double;
	using float128_t = long double;
#elif MFW_COMPILER_FLAGGED(UNIX)
	#ifdef __MFW_FLOAT16_UNIQUE
	using float16_t = _Float16;
	#else
	using float16_t = _Float32;
	#endif
	using float32_t = _Float32;
	using float64_t = _Float64;
	#if MFW_COMPILER_IS(GCC)
	using float80_t = __float80;
	using float128_t = _Float128;
	#else
	using float80_t = long double;
	using float128_t = long double;
	#endif
#else
	#error
#endif
}

#include <public/mfw/stl/internal/float_funcs.hpp>

#endif