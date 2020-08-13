#include <public/mfw/stl/new.hpp>
#include <public/mfw/stl/memory.hpp>

#ifdef _MFW_ENABLE_CUSTOM_ALLOCATORS
	#if MFW_COMPILER_FLAGGED(MSVC)
		MFW_WARNING_PUSH()
		MFW_WARNING_DISABLE(28251)
	#endif

	#pragma push_macro("new")
	#undef new

	MFW_VISIBILITY_LOCAL_PUSH()

[[nodiscard]] _MFW_ALLOC_PRE _MFW_NEW_POST void * MFW_CALL_CDECL operator new(::mfw::stl::size_t __size) _MFW_NEW_NOEXCEPT
{
	return ::mfw::stl::allocate(
		__size,
		_MFW_MEM_ALIGN, 0,
		false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

[[nodiscard]] _MFW_ALLOC_PRE _MFW_ALIGN_NEW_POST void * MFW_CALL_CDECL operator new(::mfw::stl::size_t __size, ::mfw::stl::align_val_t __alignment) _MFW_NEW_NOEXCEPT
{
	return ::mfw::stl::allocate(
		__size,
		static_cast<size_t>(__alignment), 0,
		false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

[[nodiscard]] _MFW_ALLOC_PRE _MFW_NEW_POST void * MFW_CALL_CDECL operator new(::mfw::stl::size_t __size, const ::mfw::stl::nothrow_t &) noexcept
{
	return ::mfw::stl::allocate(
		__size,
		_MFW_MEM_ALIGN, 0,
		false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

[[nodiscard]] _MFW_ALLOC_PRE _MFW_ALIGN_NEW_POST void * MFW_CALL_CDECL operator new(::mfw::stl::size_t __size, ::mfw::stl::align_val_t __alignment, const ::mfw::stl::nothrow_t &) noexcept
{
	return ::mfw::stl::allocate(
		__size,
		static_cast<size_t>(__alignment), 0,
		false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

[[nodiscard]] _MFW_ALLOC_PRE _MFW_NEW_POST void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t __size) _MFW_NEW_NOEXCEPT
{
	return ::mfw::stl::allocate(
		__size,
		_MFW_MEM_ALIGN, 0,
		true
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

[[nodiscard]] _MFW_ALLOC_PRE _MFW_ALIGN_NEW_POST void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t __size, ::mfw::stl::align_val_t __alignment) _MFW_NEW_NOEXCEPT
{
	return ::mfw::stl::allocate(
		__size,
		static_cast<size_t>(__alignment), 0,
		true
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

[[nodiscard]] _MFW_ALLOC_PRE _MFW_NEW_POST void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t __size, const ::mfw::stl::nothrow_t &) noexcept
{
	return ::mfw::stl::allocate(
		__size,
		_MFW_MEM_ALIGN, 0,
		true
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

[[nodiscard]] _MFW_ALLOC_PRE _MFW_ALIGN_NEW_POST void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t __size, ::mfw::stl::align_val_t __alignment, const ::mfw::stl::nothrow_t &) noexcept
{
	return ::mfw::stl::allocate(
		__size,
		static_cast<size_t>(__alignment), 0,
		true
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

	#if MFW_CONFIGURATION_IS(DEBUG)
[[nodiscard]] _MFW_ALLOC_PRE _MFW_NEW_POST void * MFW_CALL_CDECL operator new(::mfw::stl::size_t __size
		#if MFW_OS_IS(WINDOWS)
,::mfw::stl::int32_t __block
		#endif
,const char *__file, ::mfw::stl::int32_t __line) noexcept
{
	return ::mfw::stl::allocate(
		__size,
		_MFW_MEM_ALIGN, 0,
		false
		#if MFW_OS_IS(WINDOWS)
		,__block
		#endif
		,__file, __line,
		nullptr
	);
}

[[nodiscard]] _MFW_ALLOC_PRE _MFW_NEW_POST void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t __size
		#if MFW_OS_IS(WINDOWS)
,::mfw::stl::int32_t __block
		#endif
,const char *__file, ::mfw::stl::int32_t __line) noexcept
{
	return ::mfw::stl::allocate(
		__size,
		_MFW_MEM_ALIGN, 0,
		true
		#if MFW_OS_IS(WINDOWS)
		,__block
		#endif
		,__file, __line,
		nullptr
	);
}
	#endif

	#if MFW_STDCPP_IS(EA)
[[nodiscard]] _MFW_ALLOC_PRE _MFW_NEW_POST void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t __size, const char *, ::mfw::stl::int32_t, ::mfw::stl::uint32_t, const char *__file, ::mfw::stl::int32_t __line) noexcept
{
	return ::mfw::stl::allocate(
		__size,
		_MFW_MEM_ALIGN, 0,
		true
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
			#endif
		,__file, __line,
		nullptr
		#endif
	);
}

[[nodiscard]] __MFW_ALIGN_ALLOC_PRE _MFW_ALIGN_NEW_POST MFW_ATTRIBUTE(__alloc_align__(2)) void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t __size, ::mfw::stl::size_t __alignment, ::mfw::stl::size_t __offset, const char *, ::mfw::stl::int32_t, ::mfw::stl::uint32_t, const char *__file, ::mfw::stl::int32_t __line) noexcept
{
	return ::mfw::stl::allocate(
		__size,
		__alignment, __offset,
		true
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
			#endif
		,__file, __line,
		nullptr
		#endif
	);
}
	#endif

//#if MFW_COMPILER_FLAGGED(MSVC)
[[nodiscard]] _MFW_ALLOC_PRE _MFW_NEW_POST void * MFW_CALL_CDECL operator new(::mfw::stl::size_t, void *__ptr) noexcept
{
	return __ptr;
}

[[nodiscard]] _MFW_ALLOC_PRE _MFW_NEW_POST void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t, void *__ptr) noexcept
{
	return __ptr;
}
//#endif

	#pragma pop_macro("new")

	#if MFW_COMPILER_FLAGGED(MSVC)
		MFW_WARNING_POP()
	#endif

	#pragma push_macro("delete")
	#undef delete

_MFW_EXTERNALLY_VISIBLE void MFW_CALL_CDECL operator delete(void *__ptr) noexcept
{
	::mfw::stl::deallocate(
		__ptr, 0,
		_MFW_MEM_ALIGN, 0,
		false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

_MFW_EXTERNALLY_VISIBLE void MFW_CALL_CDECL operator delete(void *__ptr, ::mfw::stl::align_val_t __alignment) noexcept
{
	::mfw::stl::deallocate(
		__ptr, 0,
		static_cast<size_t>(__alignment), 0,
		false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

_MFW_EXTERNALLY_VISIBLE void MFW_CALL_CDECL operator delete(void *__ptr, ::mfw::stl::size_t __size) noexcept
{
	::mfw::stl::deallocate(
		__ptr, __size,
		_MFW_MEM_ALIGN, 0,
		false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

_MFW_EXTERNALLY_VISIBLE void MFW_CALL_CDECL operator delete(void *__ptr, ::mfw::stl::size_t __size, ::mfw::stl::align_val_t __alignment) noexcept
{
	::mfw::stl::deallocate(
		__ptr, __size,
		static_cast<size_t>(__alignment), 0,
		false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

_MFW_EXTERNALLY_VISIBLE void MFW_CALL_CDECL operator delete(void *__ptr, const ::mfw::stl::nothrow_t &) noexcept
{
	::mfw::stl::deallocate(
		__ptr, 0,
		_MFW_MEM_ALIGN, 0,
		false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

_MFW_EXTERNALLY_VISIBLE void MFW_CALL_CDECL operator delete(void *__ptr, ::mfw::stl::align_val_t __alignment, const ::mfw::stl::nothrow_t &) noexcept
{
	::mfw::stl::deallocate(
		__ptr, 0,
		static_cast<size_t>(__alignment), 0,
		false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

_MFW_EXTERNALLY_VISIBLE void MFW_CALL_CDECL operator delete[](void *__ptr) noexcept
{
	::mfw::stl::deallocate(
		__ptr, 0,
		_MFW_MEM_ALIGN, 0,
		true
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

_MFW_EXTERNALLY_VISIBLE void MFW_CALL_CDECL operator delete[](void *__ptr, ::mfw::stl::align_val_t __alignment) noexcept
{
	::mfw::stl::deallocate(
		__ptr, 0,
		static_cast<size_t>(__alignment), 0,
		true
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

_MFW_EXTERNALLY_VISIBLE void MFW_CALL_CDECL operator delete[](void *__ptr, ::mfw::stl::size_t __size) noexcept
{
	::mfw::stl::deallocate(
		__ptr, __size,
		_MFW_MEM_ALIGN, 0,
		true
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

_MFW_EXTERNALLY_VISIBLE void MFW_CALL_CDECL operator delete[](void *__ptr, ::mfw::stl::size_t __size, ::mfw::stl::align_val_t __alignment) noexcept
{
	::mfw::stl::deallocate(
		__ptr, __size,
		static_cast<size_t>(__alignment), 0,
		true
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

_MFW_EXTERNALLY_VISIBLE void MFW_CALL_CDECL operator delete[](void *__ptr, const ::mfw::stl::nothrow_t &) noexcept
{
	::mfw::stl::deallocate(
		__ptr, 0,
		_MFW_MEM_ALIGN, 0,
		true
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

_MFW_EXTERNALLY_VISIBLE void MFW_CALL_CDECL operator delete[](void *__ptr, ::mfw::stl::align_val_t __alignment, const ::mfw::stl::nothrow_t &) noexcept
{
	::mfw::stl::deallocate(
		__ptr, 0,
		static_cast<size_t>(__alignment), 0,
		true
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,_MFW_MEM_BLOCK
		#endif
		,__FILE__, __LINE__,
		nullptr
	#endif
	);
}

	#if MFW_CONFIGURATION_IS(DEBUG)
_MFW_EXTERNALLY_VISIBLE void MFW_CALL_CDECL operator delete(void *__ptr
		#if MFW_OS_IS(WINDOWS)
,::mfw::stl::int32_t __block
		#endif
,const char *__file, ::mfw::stl::int32_t __line) noexcept
{
	::mfw::stl::deallocate(
		__ptr, 0,
		_MFW_MEM_ALIGN, 0,
		false
		#if MFW_OS_IS(WINDOWS)
		,__block
		#endif
		,__file, __line,
		nullptr
	);
}

_MFW_EXTERNALLY_VISIBLE void MFW_CALL_CDECL operator delete[](void *__ptr
		#if MFW_OS_IS(WINDOWS)
,::mfw::stl::int32_t __block
		#endif
,const char *__file, ::mfw::stl::int32_t __line) noexcept
{
	::mfw::stl::deallocate(
		__ptr, 0,
		_MFW_MEM_ALIGN, 0,
		true
		#if MFW_OS_IS(WINDOWS)
		,__block
		#endif
		,__file, __line,
		nullptr
	);
}
	#endif

	//#if MFW_COMPILER_FLAGGED(MSVC)
_MFW_EXTERNALLY_VISIBLE void MFW_CALL_CDECL operator delete(void *, void *) noexcept
{

}
_MFW_EXTERNALLY_VISIBLE void MFW_CALL_CDECL operator delete[](void *, void *) noexcept
{

}
	//#endif

	MFW_VISIBILITY_LOCAL_POP()

	#pragma pop_macro("delete")
#endif