#include <public/mfw/stl/new.hpp>
#include <public/mfw/stl/memory.hpp>

#ifdef __MFW_ENABLE_CUSTOM_ALLOCATORS
	#if MFW_COMPILER_IS(MSVC)
		MFW_WARNING_PUSH()
		MFW_WARNING_DISABLE(28251)
	#endif

	#pragma push_macro("new")
	#undef new

[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(::mfw::stl::size_t size) __MFW_NEW_NOEXCEPT //__MFW_NEW_POST
{
	return ::mfw::stl::allocate(
		size,
		__MFW_MEM_ALIGN, 0,
		false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(::mfw::stl::size_t size, ::mfw::stl::align_val_t alignment) __MFW_NEW_NOEXCEPT //__MFW_ALIGN_NEW_POST
{
	return ::mfw::stl::allocate(
		size,
		static_cast<size_t>(alignment), 0,
		false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(::mfw::stl::size_t size, const ::mfw::stl::nothrow_t &) noexcept //__MFW_NEW_POST
{
	return ::mfw::stl::allocate(
		size,
		__MFW_MEM_ALIGN, 0,
		false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(::mfw::stl::size_t size, ::mfw::stl::align_val_t alignment, const ::mfw::stl::nothrow_t &) noexcept //__MFW_ALIGN_NEW_POST
{
	return ::mfw::stl::allocate(
		size,
		static_cast<size_t>(alignment), 0,
		false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t size) __MFW_NEW_NOEXCEPT //__MFW_NEW_POST
{
	return ::mfw::stl::allocate(
		size,
		__MFW_MEM_ALIGN, 0,
		true
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t size, ::mfw::stl::align_val_t alignment) __MFW_NEW_NOEXCEPT //__MFW_ALIGN_NEW_POST
{
	return ::mfw::stl::allocate(
		size,
		static_cast<size_t>(alignment), 0,
		true
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t size, const ::mfw::stl::nothrow_t &) noexcept //__MFW_NEW_POST
{
	return ::mfw::stl::allocate(
		size,
		__MFW_MEM_ALIGN, 0,
		true
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t size, ::mfw::stl::align_val_t alignment, const ::mfw::stl::nothrow_t &) noexcept //__MFW_ALIGN_NEW_POST
{
	return ::mfw::stl::allocate(
		size,
		static_cast<size_t>(alignment), 0,
		true
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

	#if MFW_CONFIGURATION_IS(DEBUG)
[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(::mfw::stl::size_t size
		#if MFW_OS_IS(WINDOWS)
,::mfw::stl::int32_t block
		#endif
,const char *file, ::mfw::stl::int32_t line) noexcept //__MFW_NEW_POST
{
	return ::mfw::stl::allocate(
		size,
		__MFW_MEM_ALIGN, 0,
		false
		#if MFW_OS_IS(WINDOWS)
		,block
		#endif
		,file, line,
		nullptr
	);
}

[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t size
		#if MFW_OS_IS(WINDOWS)
,::mfw::stl::int32_t block
		#endif
,const char *file, ::mfw::stl::int32_t line) noexcept //__MFW_NEW_POST
{
	return ::mfw::stl::allocate(
		size,
		__MFW_MEM_ALIGN, 0,
		true
		#if MFW_OS_IS(WINDOWS)
		,block
		#endif
		,file, line,
		nullptr
	);
}
	#endif

	#if MFW_STD_IS(EA)
[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t size, const char *name, ::mfw::stl::int32_t flags, ::mfw::stl::uint32_t debugflags, const char *file, ::mfw::stl::int32_t line) noexcept __MFW_NEW_POST {
	return ::mfw::stl::allocate(
		size,
		__MFW_MEM_ALIGN, 0,
		true
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
			#endif
		,file, line,
		nullptr
		#endif
	);
}

[[nodiscard]] __MFW_ALIGN_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t size, ::mfw::stl::size_t alignment, ::mfw::stl::size_t offset, const char *name, ::mfw::stl::int32_t flags, ::mfw::stl::uint32_t debugflags, const char *file, ::mfw::stl::int32_t line) noexcept //__MFW_ALIGN_NEW_POST MFW_ATTRIBUTE(__alloc_align__(2))
{
	return ::mfw::stl::allocate(
		size,
		alignment, offset,
		true
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
			#endif
		,file, line,
		nullptr
		#endif
	);
}
	#endif

//#if MFW_COMPILER_IS(MSVC)
[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(::mfw::stl::size_t, void *ptr) noexcept //__MFW_NEW_POST
{
	return ptr;
}

[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t, void *ptr) noexcept //__MFW_NEW_POST
{
	return ptr;
}
//#endif

	#pragma pop_macro("new")

	#if MFW_COMPILER_IS(MSVC)
		MFW_WARNING_POP()
	#endif

	#pragma push_macro("delete")
	#undef delete

void MFW_CALL_CDECL operator delete(void *ptr) noexcept //__MFW_EXTERNALLY_VISIBLE
{
	::mfw::stl::deallocate(
		ptr, 0,
		__MFW_MEM_ALIGN, 0,
		false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

void MFW_CALL_CDECL operator delete(void *ptr, ::mfw::stl::align_val_t alignment) noexcept //__MFW_EXTERNALLY_VISIBLE
{
	::mfw::stl::deallocate(
		ptr, 0,
		static_cast<size_t>(alignment), 0,
		false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

void MFW_CALL_CDECL operator delete(void *ptr, ::mfw::stl::size_t size) noexcept //__MFW_EXTERNALLY_VISIBLE
{
	::mfw::stl::deallocate(
		ptr, size,
		__MFW_MEM_ALIGN, 0,
		false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

void MFW_CALL_CDECL operator delete(void *ptr, ::mfw::stl::size_t size, ::mfw::stl::align_val_t alignment) noexcept //__MFW_EXTERNALLY_VISIBLE
{
	::mfw::stl::deallocate(
		ptr, size,
		static_cast<size_t>(alignment), 0,
		false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

void MFW_CALL_CDECL operator delete(void *ptr, const ::mfw::stl::nothrow_t &) noexcept //__MFW_EXTERNALLY_VISIBLE
{
	::mfw::stl::deallocate(
		ptr, 0,
		__MFW_MEM_ALIGN, 0,
		false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

void MFW_CALL_CDECL operator delete(void *ptr, ::mfw::stl::align_val_t alignment, const ::mfw::stl::nothrow_t &) noexcept //__MFW_EXTERNALLY_VISIBLE
{
	::mfw::stl::deallocate(
		ptr, 0,
		static_cast<size_t>(alignment), 0,
		false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

void MFW_CALL_CDECL operator delete[](void *ptr) noexcept //__MFW_EXTERNALLY_VISIBLE
{
	::mfw::stl::deallocate(
		ptr, 0,
		__MFW_MEM_ALIGN, 0,
		true
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

void MFW_CALL_CDECL operator delete[](void *ptr, ::mfw::stl::align_val_t alignment) noexcept //__MFW_EXTERNALLY_VISIBLE
{
	::mfw::stl::deallocate(
		ptr, 0,
		static_cast<size_t>(alignment), 0,
		true
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

void MFW_CALL_CDECL operator delete[](void *ptr, ::mfw::stl::size_t size) noexcept //__MFW_EXTERNALLY_VISIBLE
{
	::mfw::stl::deallocate(
		ptr, size,
		__MFW_MEM_ALIGN, 0,
		true
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

void MFW_CALL_CDECL operator delete[](void *ptr, ::mfw::stl::size_t size, ::mfw::stl::align_val_t alignment) noexcept //__MFW_EXTERNALLY_VISIBLE
{
	::mfw::stl::deallocate(
		ptr, size,
		static_cast<size_t>(alignment), 0,
		true
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

void MFW_CALL_CDECL operator delete[](void *ptr, const ::mfw::stl::nothrow_t &) noexcept //__MFW_EXTERNALLY_VISIBLE
{
	::mfw::stl::deallocate(
		ptr, 0,
		__MFW_MEM_ALIGN, 0,
		true
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

void MFW_CALL_CDECL operator delete[](void *ptr, ::mfw::stl::align_val_t alignment, const ::mfw::stl::nothrow_t &) noexcept //__MFW_EXTERNALLY_VISIBLE
{
	::mfw::stl::deallocate(
		ptr, 0,
		static_cast<size_t>(alignment), 0,
		true
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,__MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

	#if MFW_CONFIGURATION_IS(DEBUG)
void MFW_CALL_CDECL operator delete(void *ptr
		#if MFW_OS_IS(WINDOWS)
,::mfw::stl::int32_t block
		#endif
,const char *file, ::mfw::stl::int32_t line) noexcept //__MFW_EXTERNALLY_VISIBLE
{
	::mfw::stl::deallocate(
		ptr, 0,
		__MFW_MEM_ALIGN, 0,
		false
		#if MFW_OS_IS(WINDOWS)
		,block
		#endif
		,file, line,
		nullptr
	);
}

void MFW_CALL_CDECL operator delete[](void *ptr
		#if MFW_OS_IS(WINDOWS)
,::mfw::stl::int32_t block
		#endif
,const char *file, ::mfw::stl::int32_t line) noexcept //__MFW_EXTERNALLY_VISIBLE
{
	::mfw::stl::deallocate(
		ptr, 0,
		__MFW_MEM_ALIGN, 0,
		true
		#if MFW_OS_IS(WINDOWS)
		,block
		#endif
		,file, line,
		nullptr
	);
}
	#endif

	//#if MFW_COMPILER_IS(MSVC)
void MFW_CALL_CDECL operator delete(void *, void *) noexcept //__MFW_EXTERNALLY_VISIBLE
{

}
void MFW_CALL_CDECL operator delete[](void *, void *) noexcept //__MFW_EXTERNALLY_VISIBLE
{

}
	//#endif

	#pragma pop_macro("delete")
#endif