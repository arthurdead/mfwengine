#ifndef __MFW_PUBLIC_STL_NEW_H
#define __MFW_PUBLIC_STL_NEW_H

#pragma once

#if defined MFW_CPP && defined __MFW_ENABLE_CUSTOM_ALLOCATORS
	#ifndef _NEW
		#define _NEW
	#endif
#endif

#include <public/mfw/stl/defines.hpp>

#if MFW_CONFIGURATION_IS(DEBUG)
	#if MFW_OS_IS(WINDOWS)
		#ifdef MFW_CPP
			#define new new(__MFW_MEM_BLOCK, __FILE__, __LINE__)
		#endif
		#define _CRTDBG_MAP_ALLOC
		#include <crtdbg.h>
	#else
		#if defined MFW_CPP && defined __MFW_ENABLE_CUSTOM_ALLOCATORS
			#if MFW_COMPILER_IS(CLANG)
				MFW_WARNING_PUSH()
				MFW_WARNING_DISABLE("-Wkeyword-macro")
			#endif
			#define new new(__FILE__, __LINE__)
			#if MFW_COMPILER_IS(CLANG)
				MFW_WARNING_POP()
			#endif
		#endif
	#endif
#endif

#include <public/mfw/stl/shared/allocation_defines.hpp>

#ifdef MFW_CPP
	#ifdef __MFW_ENABLE_CUSTOM_ALLOCATORS
		#if MFW_COMPILER_IS(MSVC)
			MFW_WARNING_PUSH()
			MFW_WARNING_DISABLE(28251)
		#endif

		#pragma push_macro("new")
		#undef new

[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(::std::size_t size) __MFW_NEW_NOEXCEPT __MFW_NEW_POST;
[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(::std::size_t size, ::std::align_val_t alignment) __MFW_NEW_NOEXCEPT __MFW_ALIGN_NEW_POST;

[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(::std::size_t size, const ::std::nothrow_t &) noexcept __MFW_NEW_POST;
[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(::std::size_t size, ::std::align_val_t alignment, const ::std::nothrow_t &) noexcept __MFW_ALIGN_NEW_POST;

[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::std::size_t size) __MFW_NEW_NOEXCEPT __MFW_NEW_POST;
[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::std::size_t size, ::std::align_val_t alignment) __MFW_NEW_NOEXCEPT __MFW_ALIGN_NEW_POST;

[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::std::size_t size, const ::std::nothrow_t &) noexcept __MFW_NEW_POST;
[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::std::size_t size, ::std::align_val_t alignment, const ::std::nothrow_t &) noexcept __MFW_ALIGN_NEW_POST;

[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(::std::size_t size
		#if MFW_OS_IS(WINDOWS)
,::std::int32_t block
		#endif
,const char *file, ::std::int32_t line) noexcept __MFW_NEW_POST
		#if MFW_CONFIGURATION_IS(RELEASE)
= delete
		#endif
;

[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::std::size_t size
		#if MFW_OS_IS(WINDOWS)
,::std::int32_t block
		#endif
,const char *file, ::std::int32_t line) noexcept __MFW_NEW_POST
		#if MFW_CONFIGURATION_IS(RELEASE)
= delete
		#endif
;

[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::std::size_t size, const char *name, ::std::int32_t flags, ::std::uint32_t debugflags, const char *file, ::std::int32_t line) noexcept __MFW_NEW_POST
		#if MFW_STD_IS(EA)
= delete
		#endif
;

[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::std::size_t size, ::std::size_t alignment, ::std::size_t offset, const char *name, ::std::int32_t flags, ::std::uint32_t debugflags, const char *file, ::std::int32_t line) noexcept __MFW_ALIGN_NEW_POST MFW_ATTRIBUTE(__alloc_align__(2))
		#if MFW_STD_IS(EA)
= delete
		#endif
;

/*[[nodiscard]]*/ __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(::std::size_t size, void *ptr) noexcept /*__MFW_NEW_POST*/;
/*[[nodiscard]]*/ __MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](::std::size_t size, void *ptr) noexcept /*__MFW_NEW_POST*/;

		#pragma pop_macro("new")

		#if MFW_COMPILER_IS(MSVC)
			MFW_WARNING_POP()
		#endif

		#pragma push_macro("delete")
		#undef delete

void MFW_CALL_CDECL operator delete(void *ptr) noexcept __MFW_EXTERNALLY_VISIBLE;
void MFW_CALL_CDECL operator delete(void *ptr, ::std::align_val_t alignment) noexcept __MFW_EXTERNALLY_VISIBLE;
void MFW_CALL_CDECL operator delete(void *ptr, ::std::size_t size) noexcept __MFW_EXTERNALLY_VISIBLE;
void MFW_CALL_CDECL operator delete(void *ptr, ::std::size_t size, ::std::align_val_t alignment) noexcept __MFW_EXTERNALLY_VISIBLE;

void MFW_CALL_CDECL operator delete(void *ptr, const ::std::nothrow_t &) noexcept __MFW_EXTERNALLY_VISIBLE;
void MFW_CALL_CDECL operator delete(void *ptr, ::std::align_val_t alignment, const ::std::nothrow_t &) noexcept __MFW_EXTERNALLY_VISIBLE;

void MFW_CALL_CDECL operator delete[](void *ptr) noexcept __MFW_EXTERNALLY_VISIBLE;
void MFW_CALL_CDECL operator delete[](void *ptr, ::std::align_val_t alignment) noexcept __MFW_EXTERNALLY_VISIBLE;
void MFW_CALL_CDECL operator delete[](void *ptr, ::std::size_t size) noexcept __MFW_EXTERNALLY_VISIBLE;
void MFW_CALL_CDECL operator delete[](void *ptr, ::std::size_t size, ::std::align_val_t alignment) noexcept __MFW_EXTERNALLY_VISIBLE;

void MFW_CALL_CDECL operator delete[](void *ptr, const ::std::nothrow_t &) noexcept __MFW_EXTERNALLY_VISIBLE;
void MFW_CALL_CDECL operator delete[](void *ptr, ::std::align_val_t alignment, const ::std::nothrow_t &) noexcept __MFW_EXTERNALLY_VISIBLE;

void MFW_CALL_CDECL operator delete(void *ptr
		#if MFW_OS_IS(WINDOWS)
,::std::int32_t block
		#endif
,const char *file, ::std::int32_t line) noexcept __MFW_EXTERNALLY_VISIBLE
		#if MFW_CONFIGURATION_IS(RELEASE)
= delete
		#endif
;

void MFW_CALL_CDECL operator delete[](void *ptr
		#if MFW_OS_IS(WINDOWS)
,::std::int32_t block
		#endif
,const char *file, ::std::int32_t line) noexcept __MFW_EXTERNALLY_VISIBLE
		#if MFW_CONFIGURATION_IS(RELEASE)
= delete
		#endif
;

void MFW_CALL_CDECL operator delete(void *ptr, void *) noexcept __MFW_EXTERNALLY_VISIBLE;
void MFW_CALL_CDECL operator delete[](void *ptr, void *) noexcept __MFW_EXTERNALLY_VISIBLE;

		#pragma pop_macro("delete")
	#else
		#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
			#pragma push_macro("new")
			#undef new
			#include <new>
			#pragma pop_macro("new")
		#else
			#error
		#endif

		namespace mfw::stl
		{
		#if MFW_STD_FLAGGED(API_CONFORMING)
			using ::MFW_STD_NAMESPACE::align_val_t;
			using ::MFW_STD_NAMESPACE::nothrow_t;
			using ::MFW_STD_NAMESPACE::nothrow;
			#if MFW_CPP_COMPARE(>, 17)
			using ::MFW_STD_NAMESPACE::destroying_delete_t;
			using ::MFW_STD_NAMESPACE::destroying_delete;
			#endif
		#else
			#error
		#endif
		}
	#endif
#endif

#endif