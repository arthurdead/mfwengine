#ifndef __MFW_PUBLIC_STL_TYPE_TRAITS_H
#define __MFW_PUBLIC_STL_TYPE_TRAITS_H

#pragma once

#include <public/mfw/stl/version.hpp>
//#include <public/mfw/stl/tuple.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <type_traits>
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	using ::MFW_STD_NAMESPACE::declval;
	using ::MFW_STD_NAMESPACE::integral_constant;
	using ::MFW_STD_NAMESPACE::bool_constant;
	using ::MFW_STD_NAMESPACE::true_type;
	using ::MFW_STD_NAMESPACE::false_type;
	using ::MFW_STD_NAMESPACE::enable_if;
	using ::MFW_STD_NAMESPACE::enable_if_t;
	using ::MFW_STD_NAMESPACE::nullptr_t;
	using ::MFW_STD_NAMESPACE::underlying_type;
	using ::MFW_STD_NAMESPACE::underlying_type_t;
	using ::MFW_STD_NAMESPACE::remove_reference;
	using ::MFW_STD_NAMESPACE::remove_reference_t;
	using ::MFW_STD_NAMESPACE::remove_const;
	using ::MFW_STD_NAMESPACE::remove_const_t;
	using ::MFW_STD_NAMESPACE::remove_cv;
	using ::MFW_STD_NAMESPACE::remove_cv_t;
	using ::MFW_STD_NAMESPACE::remove_pointer;
	using ::MFW_STD_NAMESPACE::remove_pointer_t;
	using ::MFW_STD_NAMESPACE::remove_all_extents;
	using ::MFW_STD_NAMESPACE::remove_all_extents_t;
	using ::MFW_STD_NAMESPACE::add_lvalue_reference;
	using ::MFW_STD_NAMESPACE::add_lvalue_reference_t;
	using ::MFW_STD_NAMESPACE::add_rvalue_reference;
	using ::MFW_STD_NAMESPACE::add_rvalue_reference_t;
	using ::MFW_STD_NAMESPACE::add_pointer;
	using ::MFW_STD_NAMESPACE::add_pointer_t;
	using ::MFW_STD_NAMESPACE::add_const;
	using ::MFW_STD_NAMESPACE::add_const_t;
	using ::MFW_STD_NAMESPACE::make_signed;
	using ::MFW_STD_NAMESPACE::make_signed_t;
	using ::MFW_STD_NAMESPACE::make_unsigned;
	using ::MFW_STD_NAMESPACE::make_unsigned_t;
	using ::MFW_STD_NAMESPACE::decay_t;
	using ::MFW_STD_NAMESPACE::void_t;
	using ::MFW_STD_NAMESPACE::is_void;
	using ::MFW_STD_NAMESPACE::rank;
	using ::MFW_STD_NAMESPACE::extent;
	using ::MFW_STD_NAMESPACE::is_class;
	using ::MFW_STD_NAMESPACE::is_pointer;
	using ::MFW_STD_NAMESPACE::is_array;
	using ::MFW_STD_NAMESPACE::is_signed;
	using ::MFW_STD_NAMESPACE::is_unsigned;
	using ::MFW_STD_NAMESPACE::is_const;
	using ::MFW_STD_NAMESPACE::is_lvalue_reference;
	using ::MFW_STD_NAMESPACE::is_rvalue_reference;
	using ::MFW_STD_NAMESPACE::is_arithmetic;
	using ::MFW_STD_NAMESPACE::is_floating_point;
	using ::MFW_STD_NAMESPACE::is_fundamental;
	using ::MFW_STD_NAMESPACE::is_constructible;
	using ::MFW_STD_NAMESPACE::is_assignable;
	using ::MFW_STD_NAMESPACE::is_convertible;
	using ::MFW_STD_NAMESPACE::is_same;
	using ::MFW_STD_NAMESPACE::is_standard_layout;

	#if MFW_CPP_COMPARE(>=, 17)
	using ::MFW_STD_NAMESPACE::is_convertible_v;
	using ::MFW_STD_NAMESPACE::is_same_v;
	using ::MFW_STD_NAMESPACE::is_void_v;
	using ::MFW_STD_NAMESPACE::rank_v;
	using ::MFW_STD_NAMESPACE::extent_v;
	using ::MFW_STD_NAMESPACE::is_class_v;
	using ::MFW_STD_NAMESPACE::is_pointer_v;
	using ::MFW_STD_NAMESPACE::is_array_v;
	using ::MFW_STD_NAMESPACE::is_signed_v;
	using ::MFW_STD_NAMESPACE::is_unsigned_v;
	using ::MFW_STD_NAMESPACE::is_const_v;
	using ::MFW_STD_NAMESPACE::is_lvalue_reference_v;
	using ::MFW_STD_NAMESPACE::is_rvalue_reference_v;
	using ::MFW_STD_NAMESPACE::is_arithmetic_v;
	using ::MFW_STD_NAMESPACE::is_floating_point_v;
	using ::MFW_STD_NAMESPACE::is_fundamental_v;
	using ::MFW_STD_NAMESPACE::is_constructible_v;
	using ::MFW_STD_NAMESPACE::is_assignable_v;
	using ::MFW_STD_NAMESPACE::is_standard_layout_v;
	#else
	template <typename F, typename T>
	constexpr bool is_convertible_v{is_convertible<F, T>::value};
	template <typename T, typename U>
	constexpr bool is_same_v{is_same<T, U>::value};
	template <typename T>
	constexpr bool is_void_v{is_void<T>::value};
	template <typename T>
	constexpr size_t rank_v{rank<T>::value};
	template <typename T, uint32_t N = 0>
	constexpr size_t extent_v{extent<T, N>::value};
	template <typename T>
	constexpr bool is_class_v{is_class<T>::value};
	template <typename T>
	constexpr bool is_pointer_v{is_pointer<T>::value};
	template <typename T>
	constexpr bool is_array_v{is_array<T>::value};
	template <typename T>
	constexpr bool is_signed_v{is_signed<T>::value};
	template <typename T>
	constexpr bool is_unsigned_v{is_unsigned<T>::value};
	template <typename T>
	constexpr bool is_const_v{is_const<T>::value};
	template <typename T>
	constexpr bool is_lvalue_reference_v{is_lvalue_reference<T>::value};
	template <typename T>
	constexpr bool is_rvalue_reference_v{is_rvalue_reference<T>::value};
	template <typename T>
	constexpr bool is_arithmetic_v{is_arithmetic<T>::value};
	template <typename T>
	constexpr bool is_floating_point_v{is_floating_point<T>::value};
	template <typename T>
	constexpr bool is_fundamental_v{is_fundamental<T>::value};
	template <typename T, typename ...Args>
	constexpr bool is_constructible_v{is_constructible<T, Args...>::value};
	template <typename T, typename U>
	constexpr bool is_assignable_v{is_assignable<T, U>::value};
	template <typename T>
	constexpr bool is_standard_layout_v{is_standard_layout<T>::value};
	#endif
	
	#if MFW_CPP_COMPARE(>, 17)
	using ::MFW_STD_NAMESPACE::type_identity;
	using ::MFW_STD_NAMESPACE::type_identity_t;
	using ::MFW_STD_NAMESPACE::remove_cvref_t;
	#else
	template <typename T>
	struct type_identity { using type = T; };
	template <typename T>
	using type_identity_t = typename type_identity<T>::type;
		#if MFW_LIBCPP_FLAGGED(UNIX)
	template <typename T>
	using remove_cvref_t = ::MFW_STD_NAMESPACE::__remove_cvref_t<T>;
		#else
	template <typename T>
	struct remove_cvref
	{
		using type = remove_cv_t<remove_reference_t<T>>;
	};
	template <typename T>
	using remove_cvref_t = typename remove_cvref<T>::type;
		#endif
	#endif
#else
	#error
#endif

	template <typename ...Args>
	struct type_identity_multiple
	{
		//using type = tuple<Args...>;
		using type = void_t<>;
	};

	template <typename T>
	using type_identity_multiple_t = typename type_identity_multiple<T>::type;
}

#endif