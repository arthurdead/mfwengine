namespace mfw::stl
{
#ifdef __MFW_ENABLE_CUSTOM_ALLOCATORS
	#if MFW_STD_FLAGGED(API_CONFORMING)
	template <typename T>
	[[nodiscard]] __MFW_ALLOC_PRE T *allocator<T>::allocate(size_t size) noexcept //__MFW_ALLOC_POST(1)
	{
		size *= sizeof(value_type);
		return reinterpret_cast<value_type *>(::mfw::stl::allocate(
			size,
			alignof(value_type), 0,
			false
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,__MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__,
			&get_typeid<value_type>()
		#endif
		));
	}

	template <typename T>
	void allocator<T>::deallocate(value_type *&ptr, size_t size) noexcept
	{
		__impl_deallocate(ptr, size);
	}

	template <typename T>
	void allocator<T>::deallocate(value_type *const &ptr, size_t size) noexcept
	{
		value_type *tmp{ptr};
		__impl_deallocate(tmp, size);
	}

	template <typename T>
	void allocator<T>::__impl_deallocate(value_type *&ptr, size_t size) noexcept
	{
		size *= sizeof(value_type);
		::mfw::stl::deallocate(
			reinterpret_cast<void *&>(ptr), size,
			alignof(value_type), 0,
			false
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
			,__MFW_MEM_BLOCK
			#endif
			,__FILE__, __LINE__,
			&get_typeid<value_type>()
		#endif
		);
	}
	#else
		#error
	#endif
#endif
}