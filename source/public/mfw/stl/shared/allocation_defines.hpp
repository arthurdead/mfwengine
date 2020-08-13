#ifndef MFW_PUBLIC_STL_SHARED_ALLOCATION_DEFINES_H
#define MFW_PUBLIC_STL_SHARED_ALLOCATION_DEFINES_H

#pragma once

#include <public/mfw/stl/version.hpp>

#ifdef _MFW_ENABLE_CUSTOM_ALLOCATORS
	#if MFW_PROCESSOR_FLAGGED(64BITS)
		#define _MFW_MEM_ALIGN 8
	#elif MFW_PROCESSOR_FLAGGED(32BITS)
		#define _MFW_MEM_ALIGN 4
	#else
		#error
	#endif
#else
	#ifndef __STDCPP_DEFAULT_NEW_ALIGNMENT__
		#if MFW_PROCESSOR_FLAGGED(64BITS)
			#define _MFW_MEM_ALIGN 16
		#elif MFW_PROCESSOR_FLAGGED(32BITS)
			#define _MFW_MEM_ALIGN 8
		#else
			#error
		#endif
	#else
		#define _MFW_MEM_ALIGN __STDCPP_DEFAULT_NEW_ALIGNMENT__
	#endif
#endif

#if MFW_OS_IS(WINDOWS) && MFW_CONFIGURATION_IS(DEBUG)
	#define _MFW_MEM_BLOCK _NORMAL_BLOCK
#endif

#if MFW_COMPILER_IS(GCC)
	#define _MFW_EXTERNALLY_VISIBLE MFW_ATTRIBUTE(__externally_visible__)
#else
	#define _MFW_EXTERNALLY_VISIBLE
#endif

#define _MFW_ALLOC_PRE MFW_DECLSPEC(allocator)
#define _MFW_ALLOC_POST(s) MFW_ATTRIBUTE(__malloc__, __alloc_size__(s))
#define _MFW_ALIGN_ALLOC_POST(s, a) _MFW_ALLOC_POST(s) MFW_ATTRIBUTE(__alloc_align__(a), __assume_aligned__(_MFW_MEM_ALIGN, 0))
#define _MFW_ALIGN_REALLOC_POST(s, a) MFW_ATTRIBUTE(__alloc_size__(s), __alloc_align__(a), __assume_aligned__(_MFW_MEM_ALIGN, 0))

#define _MFW_NEW_POST _MFW_ALLOC_POST(1) _MFW_EXTERNALLY_VISIBLE
#define _MFW_ALIGN_NEW_POST _MFW_NEW_POST MFW_ATTRIBUTE(__assume_aligned__(__MFW_MEM_ALIGN, 0))

#define _MFW_NEW_NOEXCEPT noexcept(false)

#ifdef _MFW_ENABLE_CUSTOM_ALLOCATORS
namespace MFW_STD_NAMESPACE
{
	#if MFW_COMPILER_FLAGGED(MSVC)
		#if MFW_PROCESSOR_FLAGGED(64BITS)
	using size_t = unsigned long long;
		#elif MFW_PROCESSOR_FLAGGED(32BITS)
	using size_t = unsigned long;
		#else
			#error
		#endif
	#else
	using size_t = unsigned long;
	#endif

	using int32_t = signed int;
	using uint32_t = unsigned int;

	enum class align_val_t : size_t;

	struct nothrow_t;
	struct nothrow_t {};
	MFW_VISIBILITY_LOCAL extern const nothrow_t nothrow;

	#if MFW_CPP_IS_SUPPORTED(DESTROYING_DELETE)
	struct destroying_delete_t;
	struct destroying_delete_t {};
	MFW_VISIBILITY_LOCAL inline constexpr destroying_delete_t destroying_delete{};
	#endif
}

namespace mfw::stl
{
using ::MFW_STD_NAMESPACE::size_t;
using ::MFW_STD_NAMESPACE::int32_t;
using ::MFW_STD_NAMESPACE::uint32_t;
using ::MFW_STD_NAMESPACE::align_val_t;
using ::MFW_STD_NAMESPACE::nothrow_t;
using ::MFW_STD_NAMESPACE::nothrow;
	#if MFW_CPP_IS_SUPPORTED(DESTROYING_DELETE)
using ::MFW_STD_NAMESPACE::destroying_delete_t;
using ::MFW_STD_NAMESPACE::destroying_delete;
	#endif
}
#endif

#endif