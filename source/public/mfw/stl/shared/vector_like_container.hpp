#ifndef MFW_PUBLIC_STL_SHARED_VECTOR_LIKE_CONTAINER_HPP
#define MFW_PUBLIC_STL_SHARED_VECTOR_LIKE_CONTAINER_HPP

#pragma once

namespace mfw::stl
{
	template <typename _Base>
	class MFW_VISIBILITY_LOCAL __ptr_vector_like : public _Base
	{
	public:
		using super = _Base;

		using value_type = typename super::value_type::element_type;
		using allocator_type = typename super::allocator_type;
		using reference = value_type &;
		using const_reference = const value_type &;
		using pointer = value_type *;
		using const_pointer = const value_type *;
		using size_type = typename super::size_type;

		class iterator : public super::iterator
		{
		public:
			using super = typename _Base::iterator;

			value_type &operator*() noexcept;
			value_type *operator->() noexcept;
		};

		class const_iterator : public super::const_iterator
		{
		public:
			using super = typename _Base::const_iterator;

			const value_type &operator*() const noexcept;
			const value_type *operator->() const noexcept;
		};

		using const_reverse_iterator = stl::reverse_iterator<const_iterator>;
		using reverse_iterator = stl::reverse_iterator<iterator>;

		template <typename _InputIt>
		void assign(_InputIt, _InputIt) noexcept = delete;
		void assign(size_type, const value_type &) noexcept = delete;

		template <class _InputIt>
		ptr_vector_like(_InputIt, _InputIt, const allocator_type &) noexcept = delete;

		ptr_vector_like() noexcept = default;
		ptr_vector_like(const ptr_vector_like &) noexcept = default;
		ptr_vector_like(ptr_vector_like &&) noexcept = default;

		ptr_vector_like &operator=(const ptr_vector_like &) noexcept = default;
		ptr_vector_like &operator=(ptr_vector_like &&) noexcept = default;

		void erase(iterator __it) noexcept;
		void erase(const_iterator __it) noexcept;

		const_iterator cbegin() const noexcept;
		const_iterator cend() const noexcept;

		const_reverse_iterator crbegin() const noexcept;
		const_reverse_iterator crend() const noexcept;

		iterator begin() noexcept;
		iterator end() noexcept;

		reverse_iterator rbegin() noexcept;
		reverse_iterator rend() noexcept;

		const_iterator begin() const noexcept;
		const_iterator end() const noexcept;

		const_reverse_iterator rbegin() const noexcept;
		const_reverse_iterator rend() const noexcept;

		reference back() noexcept;
		const_reference back() const noexcept;

		reference front() noexcept;
		const_reference front() const noexcept;

		void push_back(const value_type &value) noexcept;

	#if MFW_CPP_COMPARE(>=, 11)
		template <typename... _Args>
		#if MFW_CPP_COMPARE(>=, 17)
		reference
		#else
		void
		#endif
		emplace_back(_Args &&... __args) noexcept;
		void push_back(value_type &&__value) noexcept;
		template <typename... _Args>
		iterator emplace(const_iterator __pos, _Args &&... __args) noexcept;
		iterator insert(const_iterator __pos, const value_type &__value) noexcept;
		iterator insert(const_iterator __pos, value_type &&__value) noexcept;
		iterator insert(const_iterator, size_type, const value_type &) noexcept = delete;
		template <typename _InputIt>
		iterator insert(const_iterator, _InputIt, _InputIt) noexcept = delete;
		iterator insert(const_iterator, initializer_list<value_type>) noexcept = delete;
		ptr_vector_like &operator=(initializer_list<value_type>) noexcept = delete;
		void assign(initializer_list<value_type>) = delete;
		ptr_vector_like(initializer_list<value_type>, const allocator_type &) noexcept = delete;
		ptr_vector_like(size_type, const value_type &, const allocator_type &) noexcept = delete;
		#if MFW_CPP_COMPARE(>=, 14)
		explicit ptr_vector_like(size_type, const allocator_type &) noexcept = delete;
		#else
		explicit ptr_vector_like(size_type) noexcept = delete;
		#endif
	#else
		explicit ptr_vector_like(size_type, const value_type &, const allocator_type &) noexcept = delete;
		iterator insert(iterator __pos, const value_type &__value) noexcept;
		void insert(iterator, size_type, const value_type &) noexcept = delete;
		template <typename _InputIt>
		void insert(iterator, _InputIt, _InputIt) noexcept = delete;
	#endif
	};
}

#include <public/mfw/stl/impl/vector_like_container.ipp>

#endif