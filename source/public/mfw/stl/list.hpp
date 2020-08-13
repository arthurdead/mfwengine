#ifndef MFW_PUBLIC_STL_LIST_HPP
#define MFW_PUBLIC_STL_LIST_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/memory.hpp>

#if MFW_STDCPP_IS(DEFAULT)
	#include <list>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/list.h>
#else
	#error
#endif

#include <public/mfw/stl/shared/vector_like_container.hpp>

namespace mfw::stl
{
	template <typename _Tp, typename _Alloc = allocator<T>>
	using list = ::MFW_STD_NAMESPACE::list<T, _Alloc>;

	template <typename _Tp, typename _Alloc = ptr_allocator<_Tp>>
	class MFW_VISIBILITY_LOCAL ptr_list : public __ptr_vector_like<list<unique_ptr<_Tp>, _Alloc>>
	{
	public:
		using super = __ptr_vector_like<list<unique_ptr<_Tp>, _Alloc>>;

		using value_type = typename super::value_type;
		using reference = typename super::reference;

		void push_front(const value_type &__value);

	#if MFW_CPP_COMPARE(>=, 11)
		template <typename... _Args>
		#if MFW_CPP_COMPARE(>=, 17)
		reference
		#else
		void
		#endif
		emplace_front(_Args &&... __args);
		void push_front(value_type &&__value);
	#endif
	};
}

#include <public/mfw/stl/impl/list.tpp>

#define __MFW_VECTOR_LIKE_CONTAINER list
#include <public/mfw/stl/detail/vector_like_funcs.hpp>
#undef __MFW_VECTOR_LIKE_CONTAINER

#endif