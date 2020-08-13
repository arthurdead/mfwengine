#ifndef MFW_PUBLIC_STL_FUNCTIONAL_HPP
#define MFW_PUBLIC_STL_FUNCTIONAL_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

#pragma push_macro("new")
#undef new
#if MFW_STDCPP_IS(DEFAULT)
	#include <functional>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/functional.h>
#else
	#error
#endif
#pragma pop_macro("new")

namespace mfw::stl
{
	using ::MFW_STD_NAMESPACE::equal_to;
	using ::MFW_STD_NAMESPACE::hash;
	using ::MFW_STD_NAMESPACE::function;
	using ::MFW_STD_NAMESPACE::bind;

	template <typename _Tp>
	class reference_wrapper final : public ::MFW_STD_NAMESPACE::reference_wrapper<_Tp>
	{
	public:
		using super = ::MFW_STD_NAMESPACE::reference_wrapper<_Tp>;

		using super::reference_wrapper;

		constexpr _Tp *operator->() const
		{ return &get(); }
	};

	using ::MFW_STD_NAMESPACE::ref;
	using ::MFW_STD_NAMESPACE::cref;
}

namespace MFW_STD_NAMESPACE
{
	template <typename _Tp>
	struct add_const<::mfw::stl::reference_wrapper<_Tp>>
	{ using type = ::mfw::stl::reference_wrapper<const _Tp>; };
}

#endif