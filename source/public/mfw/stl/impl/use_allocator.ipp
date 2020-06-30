namespace mfw::stl
{
#ifdef _MFW_ENABLE_CUSTOM_ALLOCATORS
	#pragma push_macro("new")
	#undef new

	template <typename _Tp>
	[[nodiscard]] _MFW_ALLOC_PRE void * MFW_CALL_CDECL use_allocator<_Tp>::operator new(size_t __size) noexcept //_MFW_NEW_POST
	{
		return reinterpret_cast<void *>(allocate(
			__size,
			alignof(_Tp), 0,
			false
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,_MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__
		#endif
		));
	}

	template <typename _Tp>
	[[nodiscard]] _MFW_ALLOC_PRE void * MFW_CALL_CDECL use_allocator<_Tp>::operator new(size_t __size, align_val_t __alignment) noexcept //_MFW_ALIGN_NEW_POST
	{
		return reinterpret_cast<void *>(allocate(
			__size,
			__alignment, 0,
			false
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,_MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__
		#endif
		));
	}

	template <typename _Tp>
	[[nodiscard]] _MFW_ALLOC_PRE void * MFW_CALL_CDECL use_allocator<_Tp>::operator new[](size_t __size) noexcept //_MFW_NEW_POST
	{
		return reinterpret_cast<void *>(allocate(
			__size,
			alignof(_Tp), 0,
			true
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,_MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__
		#endif
		));
	}

	template <typename _Tp>
	[[nodiscard]] _MFW_ALLOC_PRE void * MFW_CALL_CDECL use_allocator<_Tp>::operator new[](size_t __size, align_val_t __alignment) noexcept //_MFW_ALIGN_NEW_POST
	{
		return reinterpret_cast<void *>(allocate(
			__size,
			__alignment, 0,
			true
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,_MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__
		#endif
		));
	}

#if MFW_CONFIGURATION_IS(DEBUG)
	template <typename _Tp>
	[[nodiscard]] _MFW_ALLOC_PRE void * MFW_CALL_CDECL use_allocator<_Tp>::operator new(size_t __size
	#if MFW_OS_IS(WINDOWS)
	,int32_t __block
	#endif
	,const char *__file, int32_t __line) noexcept //_MFW_NEW_POST
	{
		return reinterpret_cast<void *>(allocate(
			__size,
			alignof(_Tp), 0,
			false
		#if MFW_OS_IS(WINDOWS)
			,__block
		#endif
			,__file, __line
		));
	}

	template <typename _Tp>
	[[nodiscard]] _MFW_ALLOC_PRE void * MFW_CALL_CDECL use_allocator<_Tp>::operator new[](size_t __size
	#if MFW_OS_IS(WINDOWS)
	,int32_t __block
	#endif
	,const char *__file, int32_t __line) noexcept //_MFW_NEW_POST
	{
		return reinterpret_cast<void *>(allocate(
			__size,
			alignof(_Tp), 0,
			true,
		#if MFW_OS_IS(WINDOWS)
			__block,
		#endif
			__file, __line
		));
	}
#endif

	#pragma pop_macro("new")

	#pragma push_macro("delete")
	#undef delete

	template <typename _Tp>
	void MFW_CALL_CDECL use_allocator<_Tp>::operator delete(void *__ptr) noexcept {
		_Tp *__tmp{reinterpret_cast<_Tp *>(__ptr)};
		deallocate(
			__tmp, sizeof(_Tp),
			alignof(_Tp), 0,
			false
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,_MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__
		#endif
		);
	}

	template <typename _Tp>
	void MFW_CALL_CDECL use_allocator<_Tp>::operator delete(void *__ptr, size_t __size) noexcept {
		_Tp *__tmp{reinterpret_cast<_Tp *>(__ptr)};
		deallocate(
			__tmp, __size,
			alignof(_Tp), 0,
			false
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,_MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__
		#endif
		);
	}

	template <typename _Tp>
	void MFW_CALL_CDECL use_allocator<_Tp>::operator delete(void *__ptr, size_t __size, align_val_t __alignment) noexcept {
		_Tp *__tmp{reinterpret_cast<_Tp *>(__ptr)};
		deallocate(
			__tmp, __size,
			__alignment, 0,
			false
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,_MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__
		#endif
		);
	}

	template <typename _Tp>
	void MFW_CALL_CDECL use_allocator<_Tp>::operator delete[](void *__ptr) noexcept {
		_Tp *__tmp{reinterpret_cast<_Tp *>(ptr)};
		deallocate(
			__tmp, sizeof(_Tp),
			alignof(_Tp), 0,
			true
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,_MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__
		#endif
		);
	}

	template <typename _Tp>
	void MFW_CALL_CDECL use_allocator<_Tp>::operator delete[](void *__ptr, size_t __size) noexcept {
		_Tp *__tmp{reinterpret_cast<_Tp *>(__ptr)};
		deallocate(
			__tmp, __size,
			alignof(_Tp), 0,
			true
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,_MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__
		#endif
		);
	}

	template <typename _Tp>
	void MFW_CALL_CDECL use_allocator<_Tp>::operator delete[](void *__ptr, size_t __size, align_val_t __alignment) noexcept {
		_Tp *__tmp{reinterpret_cast<_Tp *>(__ptr)};
		deallocate(
			__tmp, __size,
			__alignment, 0,
			true
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,_MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__
		#endif
		);
	}

#if MFW_CONFIGURATION_IS(DEBUG)
	template <typename _Tp>
	void MFW_CALL_CDECL use_allocator<_Tp>::operator delete(void *__ptr
	#if MFW_OS_IS(WINDOWS)
	,int32_t __block
	#endif
	,const char *__file, int32_t __line) noexcept
	{
		_Tp *__tmp{reinterpret_cast<_Tp *>(__ptr)};
		deallocate(
			__tmp, sizeof(_Tp),
			alignof(_Tp), 0,
			false
		#if MFW_OS_IS(WINDOWS)
			,__block
		#endif
			,__file, __line
		);
	}

	template <typename _Tp>
	void MFW_CALL_CDECL use_allocator<_Tp>::operator delete[](void *__ptr
	#if MFW_OS_IS(WINDOWS)
	,int32_t __block
	#endif
	,const char *__file, int32_t __line) noexcept
	{
		_Tp *__tmp{reinterpret_cast<_Tp *>(__ptr)};
		deallocate(
			__tmp, sizeof(_Tp),
			alignof(_Tp), 0,
			true
		#if MFW_OS_IS(WINDOWS)
			,__block
		#endif
			,__file, __line
		);
	}
#endif

	#pragma pop_macro("delete")

	template <typename _Tp>
	[[nodiscard]] _MFW_ALLOC_PRE _Tp *use_allocator<_Tp>::allocate(
		size_t __size,
		size_t __alignment, size_t __offset,
		bool __isarray
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,int32_t __block
		#endif
		,const char *__file, int32_t __line
	#endif
	) noexcept //_MFW_ALIGN_ALLOC_POST(1, 2)
	{
		return reinterpret_cast<_Tp *>(::mfw::stl::allocate(
			__size,
			__alignment, __offset,
			__isarray
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,__block
			#endif
			,__file, __line,
			&typeid(_Tp)
		#endif
		));
	}

	template <typename _Tp>
	void use_allocator<_Tp>::deallocate(
		_Tp *&__ptr, size_t __size,
		size_t __alignment, size_t __offset,
		bool __isarray
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,int32_t __block
		#endif
		,const char *__file, int32_t __line
	#endif
	) noexcept
	{
		::mfw::stl::deallocate(
			reinterpret_cast<void *&>(__ptr), __size,
			__alignment, __offset,
			__isarray
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,__block
			#endif
			,__file, __line,
			&typeid(_Tp)
		#endif
		);
	}
#endif
}