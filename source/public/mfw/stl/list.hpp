#ifndef __MFW_PUBLIC_STL_LIST_H
#define __MFW_PUBLIC_STL_LIST_H

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/memory.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <list>
#else
	#error
#endif

#include <public/mfw/stl/shared/vector_like_container.hpp>

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	template <typename T, typename A = allocator<T>>
	using list = ::MFW_STD_NAMESPACE::list<T, A>;

	template <typename T, typename A = allocator<unique_ptr<T>>>
	class ptr_list : public ptr_vector_like<list<unique_ptr<T>, A>>
	{
	public:
		using super = ptr_vector_like<list<unique_ptr<T>, A>>;

		using value_type = typename super::value_type;
		using reference = typename super::reference;

		void push_front(const value_type &value);

		#if MFW_CPP_COMPARE(>=, 11)
		template <typename ...Args>
			#if MFW_CPP_COMPARE(>=, 17)
		reference
			#else
		void
			#endif
			emplace_front(Args &&... args);
		void push_front(value_type &&value);
		#endif
	};
#else
	#error
#endif
}

#include <public/mfw/stl/impl/list.ipp>

#define __MFW_VECTOR_LIKE_CONTAINER list
#include <public/mfw/stl/detail/vector_like_funcs.hpp>
#undef __MFW_VECTOR_LIKE_CONTAINER

#endif