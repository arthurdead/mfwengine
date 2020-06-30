namespace mfw::stl
{
	template <typename _Tp, typename _Alloc>
	void ptr_list<_Tp, _Alloc>::push_front(const value_type &__value) noexcept
	{
		super::push_front({});
		super::front().reset(new value_type{__value});
	}

#if MFW_CPP_COMPARE(>=, 11)
	template <typename _Tp, typename _Alloc> template <typename... _Args>
	#if MFW_CPP_COMPARE(>=, 17)
	typename ptr_list<_Tp, _Alloc>::reference
	#else
	void
	#endif
	ptr_list<_Tp, _Alloc>::emplace_front(_Args &&... __args) noexcept
	{
		unique_ptr<value_type> &__ptr{super::emplace_front()};
		__ptr.reset(new value_type{forward<_Args>(__args)...});
	#if MFW_CPP_COMPARE(>=, 17)
		return *__ptr.get();
	#endif
	}

	template <typename _Tp, typename _Alloc>
	void ptr_list<_Tp, _Alloc>::push_front(value_type &&__value) noexcept
	{
		super::push_front({});
		super::front().reset(new value_type{__value});
	}
#endif
}