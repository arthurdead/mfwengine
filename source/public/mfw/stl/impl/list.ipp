#if MFW_STD_FLAGGED(API_CONFORMING)
namespace mfw::stl
{
	template <typename T, typename A>
	void ptr_list<T, A>::push_front(const value_type &value)
	{
		super::push_front({});
		unique_ptr<value_type> &ptr{super::front()};
		ptr.reset(new value_type{value});
	}

	#if MFW_CPP_COMPARE(>=, 11)
	template <typename T, typename A> template <typename ...Args>
		#if MFW_CPP_COMPARE(>=, 17)
	typename ptr_list<T, A>::reference
		#else
	void
		#endif
	ptr_list<T, A>::emplace_front(Args &&... args)
	{
		unique_ptr<value_type> &ptr{super::emplace_front()};
		ptr.reset(new value_type{forward<Args>(args)...});
		#if MFW_CPP_COMPARE(>=, 17)
		return *ptr.get();
		#endif
	}

	template <typename T, typename A>
	void ptr_list<T, A>::push_front(value_type &&value)
	{
		super::push_front({});
		unique_ptr<value_type> &ptr{super::front()};
		ptr.reset(new value_type{value});
	}
	#endif
}
#else
	#error
#endif