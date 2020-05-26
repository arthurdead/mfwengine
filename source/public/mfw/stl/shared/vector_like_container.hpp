#ifndef __MFW_PUBLIC_STL_SHARED_VECTOR_LIKE_CONTAINER_H
#define __MFW_PUBLIC_STL_SHARED_VECTOR_LIKE_CONTAINER_H

#pragma once

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	template <typename B>
	class ptr_vector_like : public B
	{
	public:
		using super = B;

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
			using super = typename B::iterator;

			value_type &operator*();
			value_type *operator->();
		};

		class const_iterator : public super::const_iterator
		{
		public:
			using super = typename B::const_iterator;

			const value_type &operator*() const;
			const value_type *operator->() const;
		};

		using const_reverse_iterator = stl::reverse_iterator<const_iterator>;
		using reverse_iterator = stl::reverse_iterator<iterator>;

		template <typename InputIt>
		void assign(InputIt first, InputIt last) = delete;
		void assign(size_type count, const value_type &value) = delete;

		template <class InputIt>
		ptr_vector_like(InputIt first, InputIt last, const allocator_type &alloc = allocator_type{}) = delete;

		ptr_vector_like() = default;
		ptr_vector_like(const ptr_vector_like &) = default;
		ptr_vector_like(ptr_vector_like &&) = default;

		ptr_vector_like &operator=(const ptr_vector_like &) = default;
		ptr_vector_like &operator=(ptr_vector_like &&) = default;

		void erase(iterator it);
		void erase(const_iterator it);

		const_iterator cbegin() const;
		const_iterator cend() const;

		const_reverse_iterator crbegin() const;
		const_reverse_iterator crend() const;

		iterator begin();
		iterator end();

		reverse_iterator rbegin();
		reverse_iterator rend();

		const_iterator begin() const;
		const_iterator end() const;

		const_reverse_iterator rbegin() const;
		const_reverse_iterator rend() const;

		reference back();
		const_reference back() const;

		reference front();
		const_reference front() const;

		void push_back(const value_type &value);

		#if MFW_CPP_COMPARE(>=, 11)
		template <typename ...Args>
			#if MFW_CPP_COMPARE(>=, 17)
		reference
			#else
		void
			#endif
			emplace_back(Args &&... args);
		void push_back(value_type &&value);
		template <typename ...Args>
		iterator emplace(const_iterator pos, Args &&... args);
		iterator insert(const_iterator pos, const value_type &value);
		iterator insert(const_iterator pos, value_type &&value);
		iterator insert(const_iterator pos, size_type count, const value_type &value) = delete;
		template <typename InputIt>
		iterator insert(const_iterator pos, InputIt first, InputIt last) = delete;
		iterator insert(const_iterator pos, initializer_list<value_type> ilist) = delete;
		ptr_vector_like &operator=(initializer_list<value_type> ilist) = delete;
		void assign(initializer_list<value_type> ilist) = delete;
		ptr_vector_like(initializer_list<value_type> init, const allocator_type &alloc = allocator_type{}) = delete;
		ptr_vector_like(size_type count, const value_type &value, const allocator_type &alloc = allocator_type{}) = delete;
			#if MFW_CPP_COMPARE(>=, 14)
		explicit ptr_vector_like(size_type count, const allocator_type &alloc = allocator_type{}) = delete;
			#else
		explicit ptr_vector_like(size_type count) = delete;
			#endif
		#else
		explicit ptr_vector_like(size_type count, const value_type &value = value_type{}, const allocator_type &alloc = allocator_type{}) = delete;
		iterator insert(iterator pos, const value_type &value);
		void insert(iterator pos, size_type count, const value_type &value) = delete;
		template <typename InputIt>
		void insert(iterator pos, InputIt first, InputIt last) = delete;
		#endif
	};
#else
	#error
#endif
}

#include <public/mfw/stl/impl/vector_like_container.ipp>

#endif