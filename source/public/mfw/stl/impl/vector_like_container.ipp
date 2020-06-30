namespace mfw::stl
{
	namespace
	{
		template <typename _Base>
		typename ptr_vector_like<_Base>::value_type &ptr_vector_like<_Base>::iterator::operator*() noexcept
		{ return super::operator*().operator*(); }

		template <typename _Base>
		typename ptr_vector_like<_Base>::value_type *ptr_vector_like<_Base>::iterator::operator->() noexcept
		{ return super::operator*().operator->(); }

		template <typename _Base>
		const typename ptr_vector_like<_Base>::value_type &ptr_vector_like<_Base>::const_iterator::operator*() const noexcept
		{ return super::operator*().operator*(); }

		template <typename _Base>
		const typename ptr_vector_like<_Base>::value_type *ptr_vector_like<_Base>::const_iterator::operator->() const noexcept
		{ return super::operator*().operator->(); }

		template <typename _Base>
		void ptr_vector_like<_Base>::erase(iterator __it) noexcept
		{ super::erase(force_cast<typename super::iterator>(__it)); }

		template <typename _Base>
		void ptr_vector_like<_Base>::erase(const_iterator __it) noexcept
		{ super::erase(force_cast<typename super::const_iterator>(__it)); }

		template <typename _Base>
		typename ptr_vector_like<_Base>::const_iterator ptr_vector_like<_Base>::cbegin() const noexcept
		{ return force_cast<const_iterator>(super::cbegin()); }

		template <typename _Base>
		typename ptr_vector_like<_Base>::const_iterator ptr_vector_like<_Base>::cend() const noexcept
		{ return force_cast<const_iterator>(super::cend()); }

		template <typename _Base>
		typename ptr_vector_like<_Base>::const_reverse_iterator ptr_vector_like<_Base>::crbegin() const noexcept
		{ return force_cast<const_reverse_iterator>(super::crbegin()); }

		template <typename _Base>
		typename ptr_vector_like<_Base>::const_reverse_iterator ptr_vector_like<_Base>::crend() const noexcept
		{ return force_cast<const_reverse_iterator>(super::crend()); }

		template <typename _Base>
		typename ptr_vector_like<_Base>::iterator ptr_vector_like<_Base>::begin() noexcept
		{ return force_cast<iterator>(super::begin()); }

		template <typename _Base>
		typename ptr_vector_like<_Base>::iterator ptr_vector_like<_Base>::end() noexcept
		{ return force_cast<iterator>(super::end()); }

		template <typename _Base>
		typename ptr_vector_like<_Base>::reverse_iterator ptr_vector_like<_Base>::rbegin() noexcept
		{ return force_cast<reverse_iterator>(super::rbegin()); }

		template <typename _Base>
		typename ptr_vector_like<_Base>::reverse_iterator ptr_vector_like<_Base>::rend() noexcept
		{ return force_cast<reverse_iterator>(super::rend()); }

		template <typename _Base>
		typename ptr_vector_like<_Base>::const_iterator ptr_vector_like<_Base>::begin() const noexcept
		{ return cbegin(); }

		template <typename _Base>
		typename ptr_vector_like<_Base>::const_iterator ptr_vector_like<_Base>::end() const noexcept
		{ return cend(); }

		template <typename _Base>
		typename ptr_vector_like<_Base>::const_reverse_iterator ptr_vector_like<_Base>::rbegin() const noexcept
		{ return crbegin(); }

		template <typename _Base>
		typename ptr_vector_like<_Base>::const_reverse_iterator ptr_vector_like<_Base>::rend() const noexcept
		{ return crend(); }

		template <typename _Base>
		typename ptr_vector_like<_Base>::reference ptr_vector_like<_Base>::back() noexcept
		{ return *super::back().get(); }

		template <typename _Base>
		typename ptr_vector_like<_Base>::const_reference ptr_vector_like<_Base>::back() const noexcept
		{ return *super::back().get(); }

		template <typename _Base>
		typename ptr_vector_like<_Base>::reference ptr_vector_like<_Base>::front() noexcept
		{ return *super::front().get(); }

		template <typename _Base>
		typename ptr_vector_like<_Base>::const_reference ptr_vector_like<_Base>::front() const noexcept
		{ return *super::front().get(); }

		template <typename _Base>
		void ptr_vector_like<_Base>::push_back(const value_type &__value) noexcept
		{
			super::push_back({});
			super::back().reset(new value_type{__value});
		}

	#if MFW_CPP_COMPARE(>=, 11)
		template <typename _Base> template <typename... _Args>
		#if MFW_CPP_COMPARE(>=, 17)
		typename ptr_vector_like<_Base>::reference
		#else
		void
		#endif
		ptr_vector_like<_Base>::emplace_back(_Args &&... __args) noexcept
		{
			unique_ptr<value_type> &__ptr{super::emplace_back()};
			__ptr.reset(new value_type{forward<_Args>(__args)...});
		#if MFW_CPP_COMPARE(>=, 17)
			return *__ptr.get();
		#endif
		}

		template <typename _Base>
		void ptr_vector_like<_Base>::push_back(value_type &&__value) noexcept
		{
			super::push_back({});
			super::back().reset(new value_type{__value});
		}

		template <typename _Base> template <typename... _Args>
		typename ptr_vector_like<_Base>::iterator ptr_vector_like<_Base>::emplace(const_iterator __pos, _Args &&... __args) noexcept
		{
			typename super::iterator __it{super::emplace(__pos)};
			__it->reset(new value_type{forward<_Args>(__args)...});
			return force_cast<iterator>(__it);
		}

		template <typename _Base>
		typename ptr_vector_like<_Base>::iterator ptr_vector_like<_Base>::insert(const_iterator __pos, const value_type &__value) noexcept
		{
			typename super::iterator __it{super::insert(__pos, {})};
			__it->reset(new value_type{__value});
			return force_cast<iterator>(__it);
		}

		template <typename _Base>
		typename ptr_vector_like<_Base>::iterator ptr_vector_like<_Base>::insert(const_iterator __pos, value_type &&__value) noexcept
		{
			typename super::iterator __it{super::insert(__pos, {})};
			__it->reset(new value_type{__value});
			return force_cast<iterator>(__it);
		}
	#else
		template <typename _Base>
		typename ptr_vector_like<_Base>::iterator ptr_vector_like<_Base>::insert(iterator __pos, const value_type &__value) noexcept
		{
			typename super::iterator __it{super::insert(__pos, {})};
			__it->reset(new value_type{__value});
			return force_cast<iterator>(__it);
		}
	#endif
	}
}