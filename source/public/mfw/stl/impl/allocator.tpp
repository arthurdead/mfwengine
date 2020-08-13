namespace mfw::stl
{
#ifdef _MFW_ENABLE_CUSTOM_ALLOCATORS
	template <typename _Tp>
	[[nodiscard]] MFW_VISIBILITY_LOCAL _MFW_ALLOC_PRE _MFW_ALLOC_POST(1) _Tp *allocator<_Tp>::allocate(size_t __size) noexcept
	{
		__size *= sizeof(value_type);
		return reinterpret_cast<value_type *>(::mfw::stl::allocate(
			__size,
			alignof(__value_type), 0,
			false
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,_MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__,
			&get_typeid<value_type>()
		#endif
		));
	}

	template <typename _Tp>
	void allocator<_Tp>::deallocate(value_type *&__ptr, size_t __size) noexcept
	{
		_deallocate_impl(__ptr, __size);
	}

	template <typename _Tp>
	void allocator<_Tp>::deallocate(value_type *const &__ptr, size_t __size) noexcept
	{
		value_type *__tmp{__ptr};
		_deallocate_impl(__tmp, __size);
	}

	template <typename _Tp>
	void allocator<_Tp>::_deallocate_impl(value_type *&__ptr, size_t __size) noexcept
	{
		__size *= sizeof(value_type);
		::mfw::stl::deallocate(
			reinterpret_cast<void *&>(__ptr), __size,
			alignof(value_type), 0,
			false
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,_MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__,
			&get_typeid<value_type>()
		#endif
		);
	}
#endif
}