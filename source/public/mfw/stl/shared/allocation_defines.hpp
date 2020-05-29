#ifndef __MFW_PUBLIC_STL_SHARED_ALLOCATION_DEFINES_H
#define __MFW_PUBLIC_STL_SHARED_ALLOCATION_DEFINES_H

#pragma once

#include <public/mfw/stl/version.hpp>

#if defined __MFW_ENABLE_CUSTOM_ALLOCATORS
	#if MFW_PROCESSOR_FLAGGED(64BITS)
		#define __MFW_MEM_ALIGN 8
	#elif MFW_PROCESSOR_FLAGGED(32BITS)
		#define __MFW_MEM_ALIGN 4
	#else
		#error
	#endif
#else
	#ifndef __STDCPP_DEFAULT_NEW_ALIGNMENT__
		#if MFW_PROCESSOR_FLAGGED(64BITS)
			#define __MFW_MEM_ALIGN 16
		#elif MFW_PROCESSOR_FLAGGED(32BITS)
			#define __MFW_MEM_ALIGN 8
		#else
			#error
		#endif
	#else
		#define __MFW_MEM_ALIGN __STDCPP_DEFAULT_NEW_ALIGNMENT__
	#endif
#endif

#if MFW_OS_IS(WINDOWS) && MFW_CONFIGURATION_IS(DEBUG)
	#define __MFW_MEM_BLOCK _NORMAL_BLOCK
#endif

#if MFW_COMPILER_IS(GCC)
	#define __MFW_EXTERNALLY_VISIBLE MFW_ATTRIBUTE(__externally_visible__)
#else
	#define __MFW_EXTERNALLY_VISIBLE
#endif

#define __MFW_ALLOC_PRE MFW_DECLSPEC(allocator)
#define __MFW_ALLOC_POST(s) MFW_ATTRIBUTE(__malloc__, __alloc_size__(s))
#define __MFW_ALIGN_ALLOC_POST(s, a) __MFW_ALLOC_POST(s) MFW_ATTRIBUTE(__alloc_align__(a), __assume_aligned__(__MFW_MEM_ALIGN, 0))
#define __MFW_ALIGN_REALLOC_POST(s, a) MFW_ATTRIBUTE(__alloc_size__(s), __alloc_align__(a), __assume_aligned__(__MFW_MEM_ALIGN, 0))

#ifdef MFW_CPP
	#define __MFW_NEW_POST __MFW_ALLOC_POST(1) __MFW_EXTERNALLY_VISIBLE
	#define __MFW_ALIGN_NEW_POST __MFW_NEW_POST MFW_ATTRIBUTE(__assume_aligned__(__MFW_MEM_ALIGN, 0))

	#define __MFW_NEW_NOEXCEPT noexcept(false)

	#ifdef __MFW_ENABLE_CUSTOM_ALLOCATORS
namespace MFW_STD_NAMESPACE
{
		#if MFW_COMPILER_IS(MSVC)
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
	extern const nothrow_t nothrow;
	
		#if MFW_CPP_IS_SUPPORTED(DESTROYING_DELETE)
	struct destroying_delete_t;
	struct destroying_delete_t {};
	inline constexpr destroying_delete_t destroying_delete{};
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

#endif