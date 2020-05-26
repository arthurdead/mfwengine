#if MFW_STD_FLAGGED(API_CONFORMING)
namespace mfw::stl
{
	template <typename B>
	typename ptr_vector_like<B>::value_type &ptr_vector_like<B>::iterator::operator*()
	{ return super::operator*().operator*(); }

	template <typename B>
	typename ptr_vector_like<B>::value_type *ptr_vector_like<B>::iterator::operator->()
	{ return super::operator*().operator->(); }

	template <typename B>
	const typename ptr_vector_like<B>::value_type &ptr_vector_like<B>::const_iterator::operator*() const
	{ return super::operator*().operator*(); }

	template <typename B>
	const typename ptr_vector_like<B>::value_type *ptr_vector_like<B>::const_iterator::operator->() const
	{ return super::operator*().operator->(); }

	template <typename B>
	void ptr_vector_like<B>::erase(iterator it) { super::erase(force_cast<typename super::iterator>(it)); }

	template <typename B>
	void ptr_vector_like<B>::erase(const_iterator it) { super::erase(force_cast<typename super::const_iterator>(it)); }

	template <typename B>
	typename ptr_vector_like<B>::const_iterator ptr_vector_like<B>::cbegin() const { return force_cast<const_iterator>(super::cbegin()); }

	template <typename B>
	typename ptr_vector_like<B>::const_iterator ptr_vector_like<B>::cend() const { return force_cast<const_iterator>(super::cend()); }

	template <typename B>
	typename ptr_vector_like<B>::const_reverse_iterator ptr_vector_like<B>::crbegin() const { return force_cast<const_reverse_iterator>(super::crbegin()); }

	template <typename B>
	typename ptr_vector_like<B>::const_reverse_iterator ptr_vector_like<B>::crend() const { return force_cast<const_reverse_iterator>(super::crend()); }

	template <typename B>
	typename ptr_vector_like<B>::iterator ptr_vector_like<B>::begin() { return force_cast<iterator>(super::begin()); }

	template <typename B>
	typename ptr_vector_like<B>::iterator ptr_vector_like<B>::end() { return force_cast<iterator>(super::end()); }

	template <typename B>
	typename ptr_vector_like<B>::reverse_iterator ptr_vector_like<B>::rbegin() { return force_cast<reverse_iterator>(super::rbegin()); }

	template <typename B>
	typename ptr_vector_like<B>::reverse_iterator ptr_vector_like<B>::rend() { return force_cast<reverse_iterator>(super::rend()); }

	template <typename B>
	typename ptr_vector_like<B>::const_iterator ptr_vector_like<B>::begin() const { return cbegin(); }

	template <typename B>
	typename ptr_vector_like<B>::const_iterator ptr_vector_like<B>::end() const { return cend(); }

	template <typename B>
	typename ptr_vector_like<B>::const_reverse_iterator ptr_vector_like<B>::rbegin() const { return crbegin(); }

	template <typename B>
	typename ptr_vector_like<B>::const_reverse_iterator ptr_vector_like<B>::rend() const { return crend(); }

	template <typename B>
	typename ptr_vector_like<B>::reference ptr_vector_like<B>::back()
	{
		unique_ptr<value_type> &ptr{super::back()};
		return *ptr.get();
	}

	template <typename B>
	typename ptr_vector_like<B>::const_reference ptr_vector_like<B>::back() const
	{
		const unique_ptr<value_type> &ptr{super::back()};
		return *ptr.get();
	}

	template <typename B>
	typename ptr_vector_like<B>::reference ptr_vector_like<B>::front()
	{
		unique_ptr<value_type> &ptr{super::front()};
		return *ptr.get();
	}

	template <typename B>
	typename ptr_vector_like<B>::const_reference ptr_vector_like<B>::front() const
	{
		const unique_ptr<value_type> &ptr{super::front()};
		return *ptr.get();
	}

	template <typename B>
	void ptr_vector_like<B>::push_back(const value_type &value)
	{
		super::push_back({});
		unique_ptr<value_type> &ptr{super::back()};
		ptr.reset(new value_type{value});
	}

	#if MFW_CPP_COMPARE(>=, 11)
	template <typename B> template <typename ...Args>
		#if MFW_CPP_COMPARE(>=, 17)
	typename ptr_vector_like<B>::reference
		#else
	void
		#endif
	ptr_vector_like<B>::emplace_back(Args &&... args)
	{
		unique_ptr<value_type> &ptr{super::emplace_back()};
		ptr.reset(new value_type{forward<Args>(args)...});
		#if MFW_CPP_COMPARE(>=, 17)
		return *ptr.get();
		#endif
	}

	template <typename B>
	void ptr_vector_like<B>::push_back(value_type &&value)
	{
		super::push_back({});
		unique_ptr<value_type> &ptr{super::back()};
		ptr.reset(new value_type{value});
	}

	template <typename B> template <typename ...Args>
	typename ptr_vector_like<B>::iterator ptr_vector_like<B>::emplace(const_iterator pos, Args &&... args)
	{
		typename super::iterator it{super::emplace(pos, {})};
		it->reset(new value_type{forward<Args>(args)...});
		return force_cast<iterator>(it);
	}

	template <typename B>
	typename ptr_vector_like<B>::iterator ptr_vector_like<B>::insert(const_iterator pos, const value_type &value)
	{
		typename super::iterator it{super::insert(pos, {})};
		it->reset(new value_type{value});
		return force_cast<iterator>(it);
	}

	template <typename B>
	typename ptr_vector_like<B>::iterator ptr_vector_like<B>::insert(const_iterator pos, value_type &&value)
	{
		typename super::iterator it{super::insert(pos, {})};
		it->reset(new value_type{value});
		return force_cast<iterator>(it);
	}
	#else
	template <typename B>
	typename ptr_vector_like<B>::iterator ptr_vector_like<B>::insert(iterator pos, const value_type &value)
	{
		typename super::iterator it{super::insert(pos, {})};
		it->reset(new value_type{value});
		return force_cast<iterator>(it);
	}
	#endif
}
#else
	#error
#endif