#include <public/mfw/stl/memory.hpp>

#if MFW_OS_IS(LINUX)
	#include <malloc.h>
#elif MFW_OS_IS(WINDOWS)
	#include <Windows.h>
#endif

#if MFW_STDC_IS(DEFAULT)
	#include <cstring>
#else
	#error
#endif

namespace mfw::stl
{
	namespace __private_memory_funcs_cpp MFW_VISIBILITY_LOCAL
	{
		static bool check_bytes(const unsigned char *first, unsigned char value, size_t size) noexcept
		{
			const unsigned char *last{first + size};
			for(const unsigned char *it{first}; it != last; ++it) {
				if(*it != value) {
					return false;
				}
			}
			return true;
		}
	}
	
	MFW_STL_API bool MFW_STL_CALL is_aligned(const void *ptr) noexcept
	{
		static constexpr size_t align_gap_size{sizeof(void *)};
		static constexpr unsigned char align_land_fill{0xED};

		const unsigned char *possible_alignment_gap{reinterpret_cast<const unsigned char *>(reinterpret_cast<uintptr_t>(ptr) & (~sizeof(uintptr_t) - 1)) - align_gap_size};
		return __private_memory_funcs_cpp::check_bytes(possible_alignment_gap, align_land_fill, align_gap_size);
	}

	MFW_STL_API size_t MFW_STL_CALL get_size(const void *ptr,
#if MFW_CONFIGURATION_IS(DEBUG) && MFW_OS_IS(WINDOWS)
	int32_t block,
#endif
	size_t alignment, size_t offset) noexcept
	{
	#if MFW_OS_IS(WINDOWS)
		if(alignment != 0) {
		#if MFW_CONFIGURATION_IS(DEBUG)
			return _aligned_msize_dbg(const_cast<void *>(ptr), alignment, offset);
		#else
			return _aligned_msize(const_cast<void *>(ptr), alignment, offset);
		#endif
		} else {
		#if MFW_CONFIGURATION_IS(DEBUG)
			return _msize_dbg(const_cast<void *>(ptr), block);
		#else
			return _msize(const_cast<void *>(ptr));
		#endif
		}
	#else
		return malloc_usable_size(const_cast<void *>(ptr));
	#endif
	}

#if MFW_COMPILER_FLAGGED(MSVC)
	MFW_WARNING_PUSH()
	MFW_WARNING_DISABLE(6320)
#endif
	MFW_STL_API bool MFW_STL_CALL is_valid(const void *ptr
#if MFW_OS_IS(WINDOWS)
	,size_t size
#endif
	) noexcept(false)
	{
		if(ptr == nullptr ||
		#if MFW_PROCESSOR_FLAGGED(64BITS)
			reinterpret_cast<uintptr_t>(ptr) == 0xABABABABABABABAB ||
			reinterpret_cast<uintptr_t>(ptr) == 0xCCCCCCCCCCCCCCCC ||
			reinterpret_cast<uintptr_t>(ptr) == 0xCDCDCDCDCDCDCDCD ||
			reinterpret_cast<uintptr_t>(ptr) == 0xDDDDDDDDDDDDDDDD ||
			reinterpret_cast<uintptr_t>(ptr) == 0xEDEDEDEDEDEDEDED ||
		#else
			reinterpret_cast<uintptr_t>(ptr) == 0xABABABAB ||
			reinterpret_cast<uintptr_t>(ptr) == 0xCCCCCCCC ||
			reinterpret_cast<uintptr_t>(ptr) == 0xCDCDCDCD ||
			reinterpret_cast<uintptr_t>(ptr) == 0xDDDDDDDD ||
			reinterpret_cast<uintptr_t>(ptr) == 0xEDEDEDED ||
		#endif
			reinterpret_cast<uintptr_t>(ptr) == 0xABADCAFE ||
			reinterpret_cast<uintptr_t>(ptr) == 0xBAADF00D ||
			reinterpret_cast<uintptr_t>(ptr) == 0xBADCAB1E ||
			reinterpret_cast<uintptr_t>(ptr) == 0xBEEFCACE ||
			reinterpret_cast<uintptr_t>(ptr) == 0xDEADDEAD ||
			(reinterpret_cast<uintptr_t>(ptr) & 7) == 7 ||
			reinterpret_cast<uintptr_t>(ptr) >= UINTPTR_MAX ||
			reinterpret_cast<intptr_t>(ptr) <= 0
		) {
			return false;
		}

		int8_t prefix{0};
		try {
			prefix = *(reinterpret_cast<const int8_t *>(ptr)-1);
		} catch(...) {
			return false;
		}
		if(//prefix == 0 ||
			prefix == -128 ||
			prefix == -2 ||
			prefix == -35) {
			return false;
		}

	#if MFW_OS_IS(WINDOWS)
		/*__try {
			if(IsBadReadPtr(ptr, size) ||
			IsBadWritePtr(const_cast<void *>(ptr), size)) {
				return false;
			}
		} __except(true) {
			return false;
		}*/
	#endif

		return true;
	}
#if MFW_COMPILER_FLAGGED(MSVC)
	MFW_WARNING_POP()
#endif

	[[nodiscard]] MFW_STL_API _MFW_ALLOC_PRE void * MFW_STL_CALL allocate(
		size_t size,
		size_t alignment, size_t offset,
		bool is_array
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,int32_t block
		#endif
		,const char *file, int32_t line,
		const type_info *type
	#endif
	) noexcept //_MFW_ALIGN_ALLOC_POST(1, 2)
	{
		if(size == 0) {
			return nullptr;
		}

		void *ptr{nullptr};

	#if MFW_OS_IS(WINDOWS)
		if(alignment == 0) {
		#if MFW_CONFIGURATION_IS(DEBUG)
			ptr = _malloc_dbg(size, block, file, line);
		#else
			ptr = malloc(size);
		#endif
		} else {
			if(offset == 0) {
			#if MFW_CONFIGURATION_IS(DEBUG)
				ptr = _aligned_malloc_dbg(size, alignment, file, line);
			#else
				ptr = _aligned_malloc(size, alignment);
			#endif
			} else {
			#if MFW_CONFIGURATION_IS(DEBUG)
				ptr = _aligned_offset_malloc_dbg(size, alignment, offset, file, line);
			#else
				ptr = _aligned_offset_malloc(size, alignment, offset);
			#endif
			}
		}
	#elif MFW_OS == MFW_OS_LINUX
		if(alignment == 0) {
			ptr = malloc(size);
		} else {
			ptr = aligned_alloc(alignment, size);
		}
	#else
		#error
	#endif

		if(ptr) {
			ptr = memset(ptr, 0x0, size);
		}

		return ptr;
	}

	MFW_STL_API _MFW_ALLOC_PRE void * MFW_STL_CALL reallocate(
		void *&ptr, size_t size,
		size_t alignment, size_t offset,
		bool is_array
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,int32_t block
		#endif
		,const char *file, int32_t line,
		const type_info *type
	#endif
	) noexcept //_MFW_ALIGN_REALLOC_POST(2, 3)
	{
		if(size == 0) {
			deallocate(ptr, size, alignment, offset, is_array
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,block
			#endif
			,file, line, type
		#endif
			);
			return nullptr;
		}

	#if MFW_OS_IS(WINDOWS)
		if(alignment == 0) {
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_COMPILER_FLAGGED(MSVC)
				MFW_WARNING_SUPPRESS(6308)
			#endif
			ptr = _realloc_dbg(ptr, size, block, file, line);
		#else
			ptr = realloc(ptr, size);
		#endif
		} else {
			if(offset == 0) {
			#if MFW_CONFIGURATION_IS(DEBUG)
				ptr = _aligned_realloc_dbg(ptr, size, alignment, file, line);
			#else
				ptr = _aligned_realloc(ptr, size, alignment);
			#endif
			} else {
			#if MFW_CONFIGURATION_IS(DEBUG)
				ptr = _aligned_offset_realloc_dbg(ptr, size, alignment, offset, file, line);
			#else
				ptr = _aligned_offset_realloc(ptr, size, alignment, offset);
			#endif
			}
		}
	#elif MFW_OS == MFW_OS_LINUX
		ptr = realloc(ptr, size);
	#else
		#error
	#endif

		if(ptr) {
			ptr = memset(ptr, 0x0, size);
		}

		return ptr;
	}

	MFW_STL_API void MFW_STL_CALL deallocate(
		void *&ptr, size_t size,
		size_t alignment, size_t offset,
		bool is_array
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,int32_t block
		#endif
		,const char *file, int32_t line,
		const type_info *type
	#endif
	) noexcept
	{
		if(!ptr) {
			return;
		}

	#if MFW_COMPILER_FLAGGED(MSVC)
		MFW_WARNING_SUPPRESS(6387)
	#endif
		ptr = memset(ptr, 0x0, size);

	#if MFW_OS_IS(WINDOWS)
		if(alignment == 0 && !is_aligned(ptr)) {
		#if MFW_CONFIGURATION_IS(DEBUG)
			_free_dbg(ptr, block);
		#else
			free(ptr);
		#endif
		} else {
		#if MFW_CONFIGURATION_IS(DEBUG)
			_aligned_free_dbg(ptr);
		#else
			_aligned_free(ptr);
		#endif
		}
	#elif MFW_OS == MFW_OS_LINUX
		free(ptr);
	#else
		#error
	#endif

		ptr = nullptr;
	}
}