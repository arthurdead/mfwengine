#ifndef _MFW_PUBLIC_STL_VECTOR_HPP
#define _MFW_PUBLIC_STL_VECTOR_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/memory.hpp>
#include <public/mfw/stl/iterator.hpp>
#include <public/mfw/stl/initializer_list.hpp>

#if MFW_STDCPP_IS(DEFAULT)
	#include <vector>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/vector.h>
#else
	#error
#endif

#include <public/mfw/stl/shared/vector_like_container.hpp>

namespace mfw::stl
{
	template <typename _Tp, typename _Alloc = allocator<_Tp>>
	using vector = ::MFW_STD_NAMESPACE::vector<_Tp, _Alloc>;

	template <typename _Tp, typename _Alloc = allocator<unique_ptr<_Tp>>>
	class ptr_vector final : public ptr_vector_like<vector<unique_ptr<_Tp>, _Alloc>>
	{
	public:
		using super = ptr_vector_like<vector<unique_ptr<_Tp>, _Alloc>>;

		using reference = typename super::reference;
		using const_reference = typename super::const_reference;
		using size_type = typename super::size_type;
		using value_type = typename super::value_type;

		reference at(size_type __pos) noexcept;
		const_reference at(size_type __pos) const noexcept;

		reference operator[](size_type __pos) noexcept;
		const_reference operator[](size_type __pos) const noexcept;
	};
}

#include <public/mfw/stl/impl/vector.ipp>

#define _MFW_VECTOR_LIKE_CONTAINER vector
#include <public/mfw/stl/internal/vector_like_funcs.hpp>
#undef _MFW_VECTOR_LIKE_CONTAINER

#endif