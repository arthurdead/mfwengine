namespace mfw::stl
{
#ifdef __MFW_ENABLE_CUSTOM_ALLOCATORS
	#pragma push_macro("new")
	#undef new

	template <typename T>
	[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL use_allocator<T>::operator new(size_t size) noexcept //__MFW_NEW_POST
	{
		return reinterpret_cast<void *>(allocate(
			size,
			alignof(T), 0,
			false
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,__MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__
		#endif
		));
	}

	template <typename T>
	[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL use_allocator<T>::operator new(size_t size, align_val_t alignment) noexcept //__MFW_ALIGN_NEW_POST
	{
		return reinterpret_cast<void *>(allocate(
			size,
			alignment, 0,
			false
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,__MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__
		#endif
		));
	}

	template <typename T>
	[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL use_allocator<T>::operator new[](size_t size) noexcept //__MFW_NEW_POST
	{
		return reinterpret_cast<void *>(allocate(
			size,
			alignof(T), 0,
			true
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,__MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__
		#endif
		));
	}

	template <typename T>
	[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL use_allocator<T>::operator new[](size_t size, align_val_t alignment) noexcept //__MFW_ALIGN_NEW_POST
	{
		return reinterpret_cast<void *>(allocate(
			size,
			alignment, 0,
			true
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,__MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__
		#endif
		));
	}

#if MFW_CONFIGURATION_IS(DEBUG)
	template <typename T>
	[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL use_allocator<T>::operator new(size_t size
	#if MFW_OS_IS(WINDOWS)
	,int32_t block
	#endif
	,const char *file, int32_t line) noexcept //__MFW_NEW_POST
	{
		return reinterpret_cast<void *>(allocate(
			size,
			alignof(T), 0,
			false
		#if MFW_OS_IS(WINDOWS)
			,block
		#endif
			,file, line
		));
	}

	template <typename T>
	[[nodiscard]] __MFW_ALLOC_PRE void * MFW_CALL_CDECL use_allocator<T>::operator new[](size_t size
	#if MFW_OS_IS(WINDOWS)
	,int32_t block
	#endif
	,const char *file, int32_t line) noexcept //__MFW_NEW_POST
	{
		return reinterpret_cast<void *>(allocate(
			size,
			alignof(T), 0,
			true,
		#if MFW_OS_IS(WINDOWS)
			block,
		#endif
			file, line
		));
	}
#endif

	#pragma pop_macro("new")

	#pragma push_macro("delete")
	#undef delete

	template <typename T>
	void MFW_CALL_CDECL use_allocator<T>::operator delete(void *ptr) noexcept {
		T *tmp{reinterpret_cast<T *>(ptr)};
		deallocate(
			tmp, sizeof(T),
			alignof(T), 0,
			false
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,__MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__
		#endif
		);
	}

	template <typename T>
	void MFW_CALL_CDECL use_allocator<T>::operator delete(void *ptr, size_t size) noexcept {
		T *tmp{reinterpret_cast<T *>(ptr)};
		deallocate(
			tmp, size,
			alignof(T), 0,
			false
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,__MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__
		#endif
		);
	}

	template <typename T>
	void MFW_CALL_CDECL use_allocator<T>::operator delete(void *ptr, size_t size, align_val_t alignment) noexcept {
		T *tmp{reinterpret_cast<T *>(ptr)};
		deallocate(
			tmp, size,
			alignment, 0,
			false
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,__MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__
		#endif
		);
	}

	template <typename T>
	void MFW_CALL_CDECL use_allocator<T>::operator delete[](void *ptr) noexcept {
		T *tmp{reinterpret_cast<T *>(ptr)};
		deallocate(
			tmp, sizeof(T),
			alignof(T), 0,
			true
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,__MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__
		#endif
		);
	}

	template <typename T>
	void MFW_CALL_CDECL use_allocator<T>::operator delete[](void *ptr, size_t size) noexcept {
		T *tmp{reinterpret_cast<T *>(ptr)};
		deallocate(
			tmp, size,
			alignof(T), 0,
			true
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,__MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__
		#endif
		);
	}

	template <typename T>
	void MFW_CALL_CDECL use_allocator<T>::operator delete[](void *ptr, size_t size, align_val_t alignment) noexcept {
		T *tmp{reinterpret_cast<T *>(ptr)};
		deallocate(
			tmp, size,
			alignment, 0,
			true
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,__MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__
		#endif
		);
	}

#if MFW_CONFIGURATION_IS(DEBUG)
	template <typename T>
	void MFW_CALL_CDECL use_allocator<T>::operator delete(void *ptr
	#if MFW_OS_IS(WINDOWS)
	,int32_t block
	#endif
	,const char *file, int32_t line) noexcept
	{
		T *tmp{reinterpret_cast<T *>(ptr)};
		deallocate(
			tmp, sizeof(T),
			alignof(T), 0,
			false
		#if MFW_OS_IS(WINDOWS)
			,block
		#endif
			,file, line
		);
	}

	template <typename T>
	void MFW_CALL_CDECL use_allocator<T>::operator delete[](void *ptr
	#if MFW_OS_IS(WINDOWS)
	,int32_t block
	#endif
	,const char *file, int32_t line) noexcept
	{
		T *tmp{reinterpret_cast<T *>(ptr)};
		deallocate(
			tmp, sizeof(T),
			alignof(T), 0,
			true
		#if MFW_OS_IS(WINDOWS)
			,block
		#endif
			,file, line
		);
	}
#endif

	#pragma pop_macro("delete")

	template <typename T>
	[[nodiscard]] __MFW_ALLOC_PRE T *use_allocator<T>::allocate(
		size_t size,
		size_t alignment, size_t offset,
		bool isarray
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,int32_t block
		#endif
		,const char *file, int32_t line
	#endif
	) noexcept //__MFW_ALIGN_ALLOC_POST(1, 2)
	{
		return reinterpret_cast<T *>(::mfw::stl::allocate(
			size,
			alignment, offset,
			isarray
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,block
			#endif
			,file, line,
			&typeid(T)
		#endif
		));
	}

	template <typename T>
	void use_allocator<T>::deallocate(
		T *&ptr, size_t size,
		size_t alignment, size_t offset,
		bool isarray
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,int32_t block
		#endif
		,const char *file, int32_t line
	#endif
	) noexcept
	{
		::mfw::stl::deallocate(
			reinterpret_cast<void *&>(ptr), size,
			alignment, offset,
			isarray
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,block
			#endif
			,file, line,
			&typeid(T)
		#endif
		);
	}
#endif
}