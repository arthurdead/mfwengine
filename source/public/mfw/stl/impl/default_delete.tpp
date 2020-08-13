namespace mfw::stl
{
#ifdef _MFW_ENABLE_CUSTOM_ALLOCATORS
	namespace __public_impl_default_delete_tpp
	{
		template <typename _Tp>
		static MFW_VISIBILITY_LOCAL void _default_delete_impl(_Tp *&__ptr, bool __is_array) noexcept
		{
			::mfw::stl::deallocate(
				reinterpret_cast<void *&>(__ptr), sizeof(_Tp),
				alignof(_Tp), 0,
				__is_array
			#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
				#if MFW_OS == MFW_OS_WINDOWS
				,_MFW_MEM_BLOCK
				#endif
				,__FILE__, __LINE__,
				&get_typeid<_Tp>()
			#endif
			);
		}
	}

	template <typename _Tp>
	void default_delete<_Tp>::operator()(_Tp *&__ptr) const noexcept
	{
		__public_impl_default_delete_tpp::_default_delete_impl(__ptr, false);
	}

	template <typename _Tp>
	void default_delete<_Tp>::operator()(_Tp *const &__ptr) const noexcept
	{
		_Tp *__tmp{__ptr};
		__public_impl_default_delete_tpp::_default_delete_impl(__tmp, false);
	}

	template <typename _Tp>
	void default_delete<_Tp[]>::operator()(_Tp *&__ptr) const noexcept
	{
		__public_impl_default_delete_tpp::_default_delete_impl(__ptr, true);
	}

	template <typename _Tp>
	void default_delete<_Tp[]>::operator()(_Tp *const &__ptr) const noexcept
	{
		_Tp *__tmp{__ptr};
		__public_impl_default_delete_tpp::_default_delete_impl(__tmp, true);
	}

	template <typename _Tp> template <size_t _Sp>
	void default_delete<_Tp[]>::operator()(_Tp (&__array)[_Sp]) const noexcept
	{
		_Tp *__tmp{__array};
		__public_impl_default_delete_tpp::_default_delete_impl(__tmp, true);
	}
#endif
}