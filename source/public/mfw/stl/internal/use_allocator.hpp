namespace mfw::stl
{
	template <typename _Tp>
	class use_allocator
	{
#ifdef _MFW_ENABLE_CUSTOM_ALLOCATORS
	public:
		#pragma push_macro("new")
		#undef new

		[[nodiscard]] static _MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(size_t size) noexcept _MFW_NEW_POST;
		[[nodiscard]] static _MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(size_t size, align_val_t alignment) noexcept _MFW_ALIGN_NEW_POST;

		[[nodiscard]] static _MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](size_t size) noexcept _MFW_NEW_POST;
		[[nodiscard]] static _MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](size_t size, align_val_t alignment) noexcept _MFW_ALIGN_NEW_POST;

		[[nodiscard]] static _MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new(size_t size
	#if MFW_OS_IS(WINDOWS)
		,int32_t block
	#endif
		,const char *file, int32_t line) noexcept _MFW_NEW_POST
	#if MFW_CONFIGURATION_IS(RELEASE)
		= delete
	#endif
		;

		[[nodiscard]] static _MFW_ALLOC_PRE void * MFW_CALL_CDECL operator new[](size_t size
	#if MFW_OS_IS(WINDOWS)
		,int32_t block
	#endif
		,const char *file, int32_t line) noexcept _MFW_NEW_POST
	#if MFW_CONFIGURATION_IS(RELEASE)
		= delete
	#endif
		;

		#pragma pop_macro("new")

		#pragma push_macro("delete")
		#undef delete

		static void MFW_CALL_CDECL operator delete(void *ptr) noexcept;
		static void MFW_CALL_CDECL operator delete(void *ptr, size_t size) noexcept;
		static void MFW_CALL_CDECL operator delete(void *ptr, size_t size, align_val_t alignment) noexcept;

		static void MFW_CALL_CDECL operator delete[](void *ptr) noexcept;
		static void MFW_CALL_CDECL operator delete[](void *ptr, size_t size) noexcept;
		static void MFW_CALL_CDECL operator delete[](void *ptr, size_t size, align_val_t alignment) noexcept;

		static void MFW_CALL_CDECL operator delete(void *ptr
	#if MFW_OS_IS(WINDOWS)
		,int32_t block
	#endif
		,const char *file, int32_t line) noexcept
	#if MFW_CONFIGURATION_IS(RELEASE)
		= delete;
	#endif
		;

		static void MFW_CALL_CDECL operator delete[](void *ptr
	#if MFW_OS_IS(WINDOWS)
		,int32_t block
	#endif
		,const char *file, int32_t line) noexcept
	#if MFW_CONFIGURATION_IS(RELEASE)
		= delete;
	#endif
		;

		#pragma pop_macro("delete")

	private:
		[[nodiscard]] static _MFW_ALLOC_PRE _Tp *allocate(
			size_t size,
			size_t alignment, size_t offset,
			bool isarray
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,int32_t block
			#endif
			,const char *file, int32_t line
		#endif
		) noexcept _MFW_ALIGN_ALLOC_POST(1, 2);

		static void deallocate(
			_Tp *&ptr, size_t size,
			size_t alignment, size_t offset,
			bool isarray
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,int32_t block
			#endif
			,const char *file, int32_t line
		#endif
		) noexcept;
#endif
	};
}

#include <public/mfw/stl/impl/use_allocator.ipp>