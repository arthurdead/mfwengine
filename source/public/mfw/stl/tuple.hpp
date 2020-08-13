#ifndef MFW_PUBLIC_STL_TUPLE_HPP
#define MFW_PUBLIC_STL_TUPLE_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/type_traits.hpp>

#if MFW_STDCPP_IS(DEFAULT)
	#include <tuple>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/tuple.h>
#else
	#error
#endif

namespace mfw::stl
{
	using ::MFW_STD_NAMESPACE::tuple;
	using ::MFW_STD_NAMESPACE::tuple_element;
	using ::MFW_STD_NAMESPACE::tuple_element_t;
	using ::MFW_STD_NAMESPACE::get;
	using ::MFW_STD_NAMESPACE::tie;

	template <typename... _Args>
	using tuple_view = type_view_t<tuple<_Args...>>;
}

#include <public/mfw/stl/internal/tuple_funcs.hpp>

#endif