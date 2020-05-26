namespace mfw::stl
{
#ifdef __MFW_ENABLE_CUSTOM_ALLOCATORS
	#if MFW_STD_FLAGGED(API_CONFORMING)
	namespace __default_delete_internal
	{
		template <typename T>
		static void __impl_delete(T *&ptr, bool is_array) noexcept;
	}

	template <typename T>
	void default_delete<T>::operator()(T *&ptr) const noexcept
	{
		__default_delete_internal::__impl_delete(ptr, false);
	}

	template <typename T>
	void default_delete<T>::operator()(T *const &ptr) const noexcept
	{
		T *tmp{ptr};
		__default_delete_internal::__impl_delete(tmp, false);
	}

	template <typename T>
	void default_delete<T[]>::operator()(T *&ptr) const noexcept
	{
		__default_delete_internal::__impl_delete(ptr, true);
	}

	template <typename T>
	void default_delete<T[]>::operator()(T *const &ptr) const noexcept
	{
		T *tmp{ptr};
		__default_delete_internal::__impl_delete(tmp, true);
	}

	template <typename T> template <size_t S>
	void default_delete<T[]>::operator()(T (&array)[S]) const noexcept
	{
		T *tmp{array};
		__default_delete_internal::__impl_delete(tmp, true);
	}

	namespace __default_delete_internal
	{
		template <typename T>
		static void __impl_delete(T *&ptr, bool is_array) noexcept
		{
			::mfw::stl::deallocate(
				reinterpret_cast<void *&>(ptr), sizeof(T),
				alignof(T), 0,
				is_array
			#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
				#if MFW_OS == MFW_OS_WINDOWS
				,__MFW_MEM_BLOCK
				#endif
				,__FILE__, __LINE__,
				&get_typeid<T>()
			#endif
			);
		}
	}
	#else
		#error
	#endif
#endif
}