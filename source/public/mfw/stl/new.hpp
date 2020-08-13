#ifndef MFW_PUBLIC_STL_NEW_HPP
#define MFW_PUBLIC_STL_NEW_HPP

#pragma once

#ifdef _MFW_ENABLE_CUSTOM_ALLOCATORS
	#ifndef _NEW
		#define _NEW
	#endif
#endif

#include <public/mfw/stl/defines.hpp>

#if MFW_CONFIGURATION_IS(DEBUG)
	#if MFW_OS_IS(WINDOWS)
		#define new new(__MFW_MEM_BLOCK, __FILE__, __LINE__)
		#define _CRTDBG_MAP_ALLOC
		#include <crtdbg.h>
	#else
		#ifdef _MFW_ENABLE_CUSTOM_ALLOCATORS
			#if MFW_COMPILER_FLAGGED(CLANG)
				MFW_WARNING_PUSH()
				MFW_WARNING_DISABLE_UNIX("-Wkeyword-macro")
			#endif
			#define new new(__FILE__, __LINE__)
			#if MFW_COMPILER_FLAGGED(CLANG)
				MFW_WARNING_POP()
			#endif
		#endif
	#endif
#endif

#include <public/mfw/stl/shared/allocation_defines.hpp>

#ifdef _MFW_ENABLE_CUSTOM_ALLOCATORS
	#if MFW_COMPILER_FLAGGED(MSVC)
		MFW_WARNING_PUSH()
		MFW_WARNING_DISABLE(28251)
	#endif

	MFW_VISIBILITY_LOCAL_PUSH()

	#pragma push_macro("new")
	#undef new

extern [[nodiscard]] _MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(::mfw::stl::size_t size) _MFW_NEW_NOEXCEPT _MFW_NEW_POST;
extern [[nodiscard]] _MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(::mfw::stl::size_t size, ::mfw::stl::align_val_t alignment) _MFW_NEW_NOEXCEPT _MFW_ALIGN_NEW_POST;

extern [[nodiscard]] _MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(::mfw::stl::size_t size, const ::mfw::stl::nothrow_t &) noexcept _MFW_NEW_POST;
extern [[nodiscard]] _MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(::mfw::stl::size_t size, ::mfw::stl::align_val_t alignment, const ::mfw::stl::nothrow_t &) noexcept _MFW_ALIGN_NEW_POST;

extern [[nodiscard]] _MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t size) _MFW_NEW_NOEXCEPT _MFW_NEW_POST;
extern [[nodiscard]] _MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t size, ::mfw::stl::align_val_t alignment) _MFW_NEW_NOEXCEPT _MFW_ALIGN_NEW_POST;

extern [[nodiscard]] _MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t size, const ::mfw::stl::nothrow_t &) noexcept _MFW_NEW_POST;
extern [[nodiscard]] _MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t size, ::mfw::stl::align_val_t alignment, const ::mfw::stl::nothrow_t &) noexcept _MFW_ALIGN_NEW_POST;

extern [[nodiscard]] _MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(::mfw::stl::size_t size
	#if MFW_OS_IS(WINDOWS)
,::mfw::stl::int32_t block
	#endif
,const char *file, ::mfw::stl::int32_t line) noexcept _MFW_NEW_POST
	#if MFW_CONFIGURATION_IS(RELEASE)
= delete
	#endif
;

extern [[nodiscard]] _MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t size
	#if MFW_OS_IS(WINDOWS)
,::mfw::stl::int32_t block
	#endif
,const char *file, ::mfw::stl::int32_t line) noexcept _MFW_NEW_POST
	#if MFW_CONFIGURATION_IS(RELEASE)
= delete
	#endif
;

extern [[nodiscard]] _MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t size, const char *name, ::mfw::stl::int32_t flags, ::mfw::stl::uint32_t debugflags, const char *file, ::mfw::stl::int32_t line) noexcept _MFW_NEW_POST
	#if !MFW_STDCPP_IS(EA)
= delete
	#endif
;

extern [[nodiscard]] _MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t size, ::mfw::stl::size_t alignment, ::mfw::stl::size_t offset, const char *name, ::mfw::stl::int32_t flags, ::mfw::stl::uint32_t debugflags, const char *file, ::mfw::stl::int32_t line) noexcept _MFW_ALIGN_NEW_POST MFW_ATTRIBUTE(__alloc_align__(2))
	#if !MFW_STDCPP_IS(EA)
= delete
	#endif
;

extern /*[[nodiscard]]*/ _MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(::mfw::stl::size_t size, void *ptr) noexcept /*_MFW_NEW_POST*/;
extern /*[[nodiscard]]*/ _MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::mfw::stl::size_t size, void *ptr) noexcept /*_MFW_NEW_POST*/;

	#pragma pop_macro("new")

	#if MFW_COMPILER_FLAGGED(MSVC)
		MFW_WARNING_POP()
	#endif

	#pragma push_macro("delete")
	#undef delete

extern void MFW_CALL_CDECL operator delete(void *ptr) noexcept _MFW_EXTERNALLY_VISIBLE;
extern void MFW_CALL_CDECL operator delete(void *ptr, ::mfw::stl::align_val_t alignment) noexcept _MFW_EXTERNALLY_VISIBLE;
extern void MFW_CALL_CDECL operator delete(void *ptr, ::mfw::stl::size_t size) noexcept _MFW_EXTERNALLY_VISIBLE;
extern void MFW_CALL_CDECL operator delete(void *ptr, ::mfw::stl::size_t size, ::mfw::stl::align_val_t alignment) noexcept _MFW_EXTERNALLY_VISIBLE;

extern void MFW_CALL_CDECL operator delete(void *ptr, const ::mfw::stl::nothrow_t &) noexcept _MFW_EXTERNALLY_VISIBLE;
extern void MFW_CALL_CDECL operator delete(void *ptr, ::mfw::stl::align_val_t alignment, const ::mfw::stl::nothrow_t &) noexcept _MFW_EXTERNALLY_VISIBLE;

extern void MFW_CALL_CDECL operator delete[](void *ptr) noexcept _MFW_EXTERNALLY_VISIBLE;
extern void MFW_CALL_CDECL operator delete[](void *ptr, ::mfw::stl::align_val_t alignment) noexcept _MFW_EXTERNALLY_VISIBLE;
extern void MFW_CALL_CDECL operator delete[](void *ptr, ::mfw::stl::size_t size) noexcept _MFW_EXTERNALLY_VISIBLE;
extern void MFW_CALL_CDECL operator delete[](void *ptr, ::mfw::stl::size_t size, ::mfw::stl::align_val_t alignment) noexcept _MFW_EXTERNALLY_VISIBLE;

extern void MFW_CALL_CDECL operator delete[](void *ptr, const ::mfw::stl::nothrow_t &) noexcept _MFW_EXTERNALLY_VISIBLE;
extern void MFW_CALL_CDECL operator delete[](void *ptr, ::mfw::stl::align_val_t alignment, const ::mfw::stl::nothrow_t &) noexcept _MFW_EXTERNALLY_VISIBLE;

extern void MFW_CALL_CDECL operator delete(void *ptr
	#if MFW_OS_IS(WINDOWS)
,::mfw::stl::int32_t block
	#endif
,const char *file, ::mfw::stl::int32_t line) noexcept _MFW_EXTERNALLY_VISIBLE
	#if MFW_CONFIGURATION_IS(RELEASE)
= delete
	#endif
;

extern void MFW_CALL_CDECL operator delete[](void *ptr
	#if MFW_OS_IS(WINDOWS)
,::mfw::stl::int32_t block
	#endif
,const char *file, ::mfw::stl::int32_t line) noexcept _MFW_EXTERNALLY_VISIBLE
	#if MFW_CONFIGURATION_IS(RELEASE)
= delete
	#endif
;

extern void MFW_CALL_CDECL operator delete(void *ptr, void *) noexcept _MFW_EXTERNALLY_VISIBLE;
extern void MFW_CALL_CDECL operator delete[](void *ptr, void *) noexcept _MFW_EXTERNALLY_VISIBLE;

	MFW_VISIBILITY_LOCAL_POP()

	#pragma pop_macro("delete")
#else
	#pragma push_macro("new")
	#undef new
	#include <new>
	#pragma pop_macro("new")

namespace mfw::stl
{
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