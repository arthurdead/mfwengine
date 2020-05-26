#ifndef __MFW_PUBLIC_STL_VECTOR_H
#define __MFW_PUBLIC_STL_VECTOR_H

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/memory.hpp>
#include <public/mfw/stl/iterator.hpp>
#include <public/mfw/stl/initializer_list.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <vector>
#else
	#error
#endif

#include <public/mfw/stl/shared/vector_like_container.hpp>

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	template <typename T, typename A = allocator<T>>
	using vector = ::MFW_STD_NAMESPACE::vector<T, A>;

	template <typename T, typename A = allocator<unique_ptr<T>>>
	class ptr_vector : public ptr_vector_like<vector<unique_ptr<T>, A>>
	{
	public:
		using super = ptr_vector_like<vector<unique_ptr<T>, A>>;

		using reference = typename super::reference;
		using const_reference = typename super::const_reference;
		using size_type = typename super::size_type;
		using value_type = typename super::value_type;

		reference at(size_type pos);
		const_reference at(size_type pos) const;

		reference operator[](size_type pos);
		const_reference operator[](size_type pos) const;
	};
#else
	#error
#endif
}

#include <public/mfw/stl/impl/vector.ipp>

#define __MFW_VECTOR_LIKE_CONTAINER vector
#include <public/mfw/stl/detail/vector_like_funcs.hpp>
#undef __MFW_VECTOR_LIKE_CONTAINER

#endif