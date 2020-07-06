#ifndef _MFW_PUBLIC_STL_DEFINES_HPP
#define _MFW_PUBLIC_STL_DEFINES_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

#define MFW_BIT _MFW_BIT
#define MFW_STRINGFY _MFW_STRINGFY
#define MFW_FILE_LINE_STRING _MFW_FILE_LINE_STRING
#define MFW_PRAGMA _MFW_PRAGMA
#define MFW_MESSAGE _MFW_MESSAGE
#define MFW_MACRO_CONCATENATE _MFW_MACRO_CONCATENATE

#define _MFW_MACRO_EXPAND_IMPL(x) x
#define MFW_MACRO_EXPAND(x) _MFW_MACRO_EXPAND_IMPL(x)

#define MFW_NOTHING

#ifdef MFW_CPP_CHAR8_SUPPORTED
	#define _MFW_U8STRINGFY_IMPL(x) MFW_MACRO_CONCATENATE(u8, #x)
	#define MFW_U8STRINGFY(x) _MFW_U8STRINGFY_IMPL(x)
#endif
#define _MFW_U16STRINGFY_IMPL(x) MFW_MACRO_CONCATENATE(u, #x)
#define MFW_U16STRINGFY(x) _MFW_U16STRINGFY_IMPL(x)
#define _MFW_U32STRINGFY_IMPL(x) MFW_MACRO_CONCATENATE(U, #x)
#define MFW_U32STRINGFY(x) _MFW_U32STRINGFY_IMPL(x)
#define _MFW_WSTRINGFY_IMPL(x) MFW_MACRO_CONCATENATE(L, #x)
#define MFW_WSTRINGFY(x) _MFW_WSTRINGFY_IMPL(x)

#include <public/mfw/stl/internal/compiler_defines.hpp>

#define _MFW_DEBUGBREAK MFW_MACRO_EXPAND(MFW_DEBUGBREAK)
#define _MFW_WARNING_PUSH MFW_MACRO_EXPAND(MFW_WARNING_PUSH)
#define _MFW_WARNING_POP MFW_MACRO_EXPAND(MFW_WARNING_POP)
#define _MFW_WARNING_DISABLE MFW_MACRO_EXPAND(MFW_WARNING_DISABLE)
#define _MFW_WARNING_SUPPRESS MFW_MACRO_EXPAND(MFW_WARNING_SUPPRESS)
#define _MFW_WARNING_DISABLE_UNIX MFW_MACRO_EXPAND(MFW_WARNING_DISABLE_UNIX)

#if MFW_CONFIGURATION_IS(RELEASE)
	#undef MFW_DEBUGBREAK
	#define MFW_DEBUGBREAK please remove the debugbreak
	#undef MFW_WARNING_PUSH
	#undef MFW_WARNING_POP
	#undef MFW_WARNING_DISABLE
	#undef MFW_WARNING_SUPPRESS
	#undef MFW_WARNING_DISABLE_UNIX
	#define MFW_WARNING_PUSH please fix the warning
	#define MFW_WARNING_POP please fix the warning
	#define MFW_WARNING_DISABLE please fix the warning
	#define MFW_WARNING_SUPPRESS please fix the warning
	#define MFW_WARNING_DISABLE_UNIX please fix the warning
#endif

#if MFW_OS_IS(WINDOWS)
	#define MFW_CALL_SHARED MFW_CALL_STDCALL
#elif MFW_OS_IS(LINUX)
	#define MFW_CALL_SHARED MFW_CALL_CDECL
#else
	#error
#endif

#ifdef MFW_CPP
	#define _MFW_ENUM_SHARED_EQUAL_IMPL(acessor, name, arg, type) \
		acessor constexpr inline bool operator==(arg rhs, name lhs) noexcept \
		{ return static_cast<type>(rhs) == static_cast<type>(lhs); } \
		acessor constexpr inline bool operator!=(arg rhs, name lhs) noexcept \
		{ return static_cast<type>(rhs) != static_cast<type>(lhs); } \
		acessor constexpr inline bool operator>(arg rhs, name lhs) noexcept \
		{ return static_cast<type>(rhs) > static_cast<type>(lhs); } \
		acessor constexpr inline bool operator<(arg rhs, name lhs) noexcept \
		{ return static_cast<type>(rhs) < static_cast<type>(lhs); } \
		acessor constexpr inline bool operator>=(arg rhs, name lhs) noexcept \
		{ return static_cast<type>(rhs) >= static_cast<type>(lhs); } \
		acessor constexpr inline bool operator<=(arg rhs, name lhs) noexcept \
		{ return static_cast<type>(rhs) <= static_cast<type>(lhs); }

	#define _MFW_ENUM_SHARED_EQUAL_ADDS_IMPL(acessor, name, arg, type) \
		acessor constexpr inline name operator+=(name &rhs, arg lhs) noexcept \
		{ rhs = static_cast<name>(static_cast<type>(rhs) + static_cast<type>(lhs)); return rhs; } \
		acessor constexpr inline name operator-=(name &rhs, arg lhs) noexcept \
		{ rhs = static_cast<name>(static_cast<type>(rhs) + static_cast<type>(lhs)); return rhs; }

	#define _MFW_ENUM_SHARED_AND_BIT_IMPL(acessor, name, arg, type) \
		acessor constexpr inline name operator&(name rhs, arg lhs) noexcept \
		{ return static_cast<name>(static_cast<type>(rhs) & static_cast<type>(lhs)); }

	#define _MFW_ENUM_SHARED_AND_FLAG_IMPL(acessor, name, arg, type) \
		acessor constexpr inline bool operator&(name rhs, arg lhs) noexcept \
		{ return ((static_cast<type>(rhs) & static_cast<type>(lhs)) == (static_cast<type>(lhs))); }

	#define _MFW_ENUM_SHARED_FLAGS_IMPL(acessor, name, arg, type) \
		acessor constexpr inline name operator|(name rhs, arg lhs) noexcept \
		{ return static_cast<name>(static_cast<type>(rhs) | static_cast<type>(lhs)); } \
		acessor constexpr inline name operator^(name rhs, arg lhs) noexcept \
		{ return static_cast<name>(static_cast<type>(rhs) ^ static_cast<type>(lhs)); } \
		acessor constexpr inline name &operator&=(name &rhs, arg lhs) noexcept \
		{ rhs = static_cast<name>(static_cast<type>(rhs) & static_cast<type>(lhs)); return rhs; } \
		acessor constexpr inline name &operator|=(name &rhs, arg lhs) noexcept \
		{ rhs = static_cast<name>(static_cast<type>(rhs) | static_cast<type>(lhs)); return rhs; } \

	#define _MFW_ENUM_SHARED_UNARY_ADD_IMPL(acessor, name, type) \
		acessor constexpr inline name &operator++(name &rhs) noexcept \
		{ rhs = static_cast<name>(static_cast<type>(rhs) + static_cast<type>(1)); return rhs; } \
		acessor constexpr inline name &operator--(name &rhs) noexcept \
		{ rhs = static_cast<name>(static_cast<type>(rhs) - static_cast<type>(1)); return rhs; } \
		acessor constexpr inline name &operator++(name &rhs,int) noexcept \
		{ rhs = static_cast<name>(static_cast<type>(rhs) + static_cast<type>(1)); return rhs; } \
		acessor constexpr inline name &operator--(name &rhs,int) noexcept \
		{ rhs = static_cast<name>(static_cast<type>(rhs) - static_cast<type>(1)); return rhs; }

	#define _MFW_ENUM_SHARED_UNARY_FLAGS_IMPL(acessor, name, type) \
		acessor constexpr inline name operator~(name rhs) { return static_cast<name>(~static_cast<type>(rhs)); }

	#define _MFW_ENUM_SHARED_UNARY_FLAGS(acessor, name) \
		_MFW_ENUM_SHARED_UNARY_FLAGS_IMPL(acessor, name, ::mfw::stl::underlying_type_t<name>)

	#define _MFW_ENUM_SHARED_UNARY_ADD(acessor, name) \
		_MFW_ENUM_SHARED_UNARY_ADD_IMPL(acessor, name, ::mfw::stl::underlying_type_t<name>)

	#define _MFW_ENUM_SHARED_BIT_AND(acessor, name) \
		_MFW_ENUM_SHARED_AND_BIT_IMPL(acessor, name, int, ::mfw::stl::underlying_type_t<name>) \
		_MFW_ENUM_SHARED_AND_BIT_IMPL(acessor, name, name, ::mfw::stl::underlying_type_t<name>)
		
	#define _MFW_ENUM_SHARED_FLAG_AND(acessor, name) \
		_MFW_ENUM_SHARED_AND_FLAG_IMPL(acessor, name, int, ::mfw::stl::underlying_type_t<name>) \
		_MFW_ENUM_SHARED_AND_FLAG_IMPL(acessor, name, name, ::mfw::stl::underlying_type_t<name>)

	#define _MFW_ENUM_SHARED_FLAGS(acessor, name) \
		_MFW_ENUM_SHARED_FLAGS_IMPL(acessor, name, int, ::mfw::stl::underlying_type_t<name>) \
		_MFW_ENUM_SHARED_FLAGS_IMPL(acessor, name, name, ::mfw::stl::underlying_type_t<name>)

	#define _MFW_ENUM_SHARED_EQUAL_ADDS(acessor, name) \
		_MFW_ENUM_SHARED_EQUAL_ADDS_IMPL(acessor, name, int, ::mfw::stl::underlying_type_t<name>) \
		_MFW_ENUM_SHARED_EQUAL_ADDS_IMPL(acessor, name, name, ::mfw::stl::underlying_type_t<name>)

	#define _MFW_ENUM_SHARED_EQUAL(acessor, name) \
		_MFW_ENUM_SHARED_EQUAL_IMPL(acessor, name, int, ::mfw::stl::underlying_type_t<name>) \
		_MFW_ENUM_SHARED_EQUAL_IMPL(acessor, name, name, ::mfw::stl::underlying_type_t<name>)

	#define _MFW_ENUM_FLAGS_IMPL_BASE(acessor, name) \
		_MFW_ENUM_SHARED_EQUAL(acessor, name) \
		_MFW_ENUM_SHARED_FLAGS(acessor, name) \
		_MFW_ENUM_SHARED_UNARY_FLAGS(acessor, name)

	#define _MFW_ENUM_FLAGS_V2_IMPL(acessor, name) \
		_MFW_ENUM_FLAGS_IMPL_BASE(acessor, name) \
		_MFW_ENUM_SHARED_FLAG_AND(acessor, name)
		
	#define _MFW_ENUM_FLAGS_V1_IMPL(acessor, name) \
		_MFW_ENUM_FLAGS_IMPL_BASE(acessor, name) \
		_MFW_ENUM_SHARED_BIT_AND(acessor, name)

	#define _MFW_ENUM_IMPL(acessor, name) \
		_MFW_ENUM_SHARED_EQUAL(acessor, name) \
		_MFW_ENUM_SHARED_UNARY_ADD(acessor, name)

	#define _MFW_ENUM_INT_IMPL(acessor, name) \
		_MFW_ENUM_SHARED_EQUAL(acessor, name) \
		_MFW_ENUM_SHARED_EQUAL_ADDS(acessor, name) \
		_MFW_ENUM_SHARED_BIT_AND(acessor, name) \
		_MFW_ENUM_SHARED_FLAGS(acessor, name) \
		_MFW_ENUM_SHARED_UNARY_FLAGS(acessor, name) \
		_MFW_ENUM_SHARED_UNARY_ADD(acessor, name)

	#define MFW_CLASS_ENUM_FLAGS_V1(name) _MFW_ENUM_FLAGS_V1_IMPL(friend, name)
	#define MFW_ENUM_FLAGS_V1(name) _MFW_ENUM_FLAGS_V1_IMPL(MFW_NOTHING, name)

	#define MFW_CLASS_ENUM_FLAGS_V2(name) _MFW_ENUM_FLAGS_V2_IMPL(friend, name)
	#define MFW_ENUM_FLAGS_V2(name) _MFW_ENUM_FLAGS_V2_IMPL(MFW_NOTHING, name)

	#define MFW_CLASS_ENUM_FLAGS(name) MFW_CLASS_ENUM_FLAGS_V1(name)
	#define MFW_ENUM_FLAGS(name) MFW_ENUM_FLAGS_V1(name)

	#define MFW_CLASS_ENUM(name) _MFW_ENUM_IMPL(friend, name)
	#define MFW_ENUM(name) _MFW_ENUM_IMPL(MFW_NOTHING, name)

	#define MFW_CLASS_ENUM_INT(name) _MFW_ENUM_INT_IMPL(friend, name)
	#define MFW_ENUM_INT(name) _MFW_ENUM_INT_IMPL(MFW_NOTHING, name)

	#define MFW_EXECUTE_N_TIMES(num, ...) \
		static ::mfw::stl::uint8_t __exec_##num##_times{0}; \
		if(__exec_##num##_times < num) { \
			__VA_ARGS__ \
			__exec_##num##_times++; \
		} \

	#define _MFW_TO_STRING_HELPER(var, vartype, dst, dsttype) \
		if constexpr(::mfw::stl::test_funcs_string<dsttype>::member::template has_to_v<vartype>) { \
			dsttype __tmp{}; \
			var.to_string(var, __tmp); \
			dst.append(::mfw::stl::move(__tmp)); \
		} else if constexpr(::mfw::stl::test_funcs_string<dsttype>::template global<vartype>::has_to_v) { \
			dsttype __tmp{}; \
			::mfw::stl::to_string(var, __tmp); \
			dst.append(::mfw::stl::move(__tmp)); \
		} else if constexpr(::mfw::stl::is_same_v<vartype, dsttype>) { \
			dst.append(var); \
		} else if constexpr(::mfw::stl::is_constructible_v<dsttype, vartype>) { \
			dst.append(dsttype{var}); \
		} else if constexpr(::mfw::stl::is_convertible_v<vartype, dsttype>) { \
			dst.append(static_cast<dsttype>(var)); \
		} else if constexpr(::mfw::stl::is_assignable_v<dsttype, vartype>) { \
			dsttype __tmp{}; \
			__tmp = var; \
			dst.append(::mfw::stl::move(__tmp)); \
		} else { \
			static_assert(false); \
		}

	#if MFW_COMPILER_FLAGGED(CLANG)
		MFW_WARNING_DISABLE_UNIX("-Wgnu-zero-variadic-macro-arguments")
		MFW_MESSAGE("remove this later")
	#endif

	MFW_MESSAGE("TODO!!! below does not work with templates")
	#define MFW_DECLARE_FUNC_CHECK_MEMBER(name, ret, func, cnst, ...) \
		template <typename _Tp> \
		class name final \
		{ \
			template <typename _Rp, typename _Cp, typename... _Args> \
			static constexpr auto check(::mfw::stl::nullptr_t) noexcept -> decltype(::mfw::stl::declval<_Cp>().func(::mfw::stl::declval<_Args>()...)); \
			template <typename _Rp, typename _Cp, typename... _Args> \
			static constexpr ::mfw::stl::false_type check(...) noexcept; \
		public: \
			static constexpr bool value{::mfw::stl::is_same_v<decltype(check<ret, cnst _Tp, ##__VA_ARGS__>(nullptr)), ret>}; \
			template <typename _Rp, typename... _Args> \
			static constexpr bool value_overload{::mfw::stl::is_same_v<decltype(check<_Rp, cnst _Tp, _Args...>(nullptr)), _Rp>}; \
		}; \
		template <typename _Tp> \
		static constexpr bool name##_v{name<_Tp>::value}; \
		template <typename _Tp, typename _Rp, typename... _Args> \
		static constexpr bool name##_overload_v{name<_Tp>::template value_overload<_Rp, _Args...>};

	MFW_MESSAGE("TODO!!! below does not work with templates")
	#define MFW_DECLARE_FUNC_CHECK_GLOBAL(name, ret, func, ...) \
		class name final \
		{ \
			template <typename _Rp, typename... _Args> \
			static constexpr auto check(::mfw::stl::nullptr_t) noexcept -> decltype(func(::mfw::stl::declval<_Args>()...)); \
			template <typename _Rp, typename... _Args> \
			static constexpr ::mfw::stl::false_type check(...) noexcept; \
		public: \
			static constexpr bool value{::mfw::stl::is_same_v<decltype(check<ret, ##__VA_ARGS__>(nullptr)), ret>}; \
			template <typename _Rp, typename... _Args> \
			static constexpr bool value_overload{::mfw::stl::is_same_v<decltype(check<_Rp, _Args...>(nullptr)), _Rp>}; \
		}; \
		static constexpr bool name##_v{name::value}; \
		template <typename _Rp, typename... _Args> \
		static constexpr bool name##_overload_v{name::template value_overload<_Rp, _Args...>};

	#define MFW_DECLARE_AS_TO_CHECK_FUNCS_BEGIN(name) \
		template <typename _Vp> \
		struct test_funcs_##name final \
		{ \
			struct member final \
			{
				
	#define MFW_DECLARE_AS_TO_CHECK_FUNCS_END(name) \
				MFW_DECLARE_FUNC_CHECK_MEMBER(has_to, void, to_##name, const, _Vp &) \
			}; \
			template <typename _Tp> \
			struct global final \
			{ \
				MFW_DECLARE_FUNC_CHECK_GLOBAL(has_as, _Vp, as_##name, const _Tp &) \
				MFW_DECLARE_FUNC_CHECK_GLOBAL(has_to, void, to_##name, const _Tp &, _Vp &) \
			}; \
		};

	#define MFW_DECLARE_AS_TO_CHECK_FUNCS_V2(name) \
		MFW_DECLARE_AS_TO_CHECK_FUNCS_BEGIN(name) \
		MFW_DECLARE_FUNC_CHECK_MEMBER(has_as, _Vp, as_##name, const, void) \
		MFW_DECLARE_AS_TO_CHECK_FUNCS_END(name)

	#define MFW_DECLARE_AS_TO_CHECK_FUNCS_V1(name) \
		MFW_DECLARE_AS_TO_CHECK_FUNCS_BEGIN(name) \
		MFW_DECLARE_FUNC_CHECK_MEMBER(has_as, _Vp, as_##name, const) \
		MFW_DECLARE_AS_TO_CHECK_FUNCS_END(name)

	#define MFW_DECLARE_AS_TO_CHECK_FUNCS MFW_DECLARE_AS_TO_CHECK_FUNCS_V2

	#define MFW_DECLARE_TO_FUNC_GLOBAL(name)

	#define MFW_DECLARE_AS_FUNC_GLOBAL(name) \
		template <typename _Dp, typename _Sp> \
		_Dp as_##name(const _Sp &__src) noexcept;

	MFW_MESSAGE("TODO!! remake both below once MFW_DECLARE_FUNC_CHECK_MEMBER/GLOBAL supports templates")
	#define MFW_IMPLEMENT_TO_FUNC_GLOBAL(name)

	#define _MFW_IMPLEMENT_AS_FUNC_GLOBAL_START(name) \
		template <typename _Dp, typename _Sp> \
		_Dp as_##name(const _Sp &__src) noexcept \
		{ \
			using __D = ::mfw::stl::remove_cvref_t<_Dp>; \
			using __S = ::mfw::stl::remove_cvref_t<_Sp>; \
			__D __tmp{}; \

	#define _MFW_IMPLEMENT_AS_FUNC_GLOBAL_END(name) \
			if constexpr(::mfw::stl::is_same_v<__D, __S>) { \
				__tmp = __src; \
			} else if constexpr(::mfw::stl::is_constructible_v<__D, __S>) { \
				__tmp = __D{__src}; \
			} else if constexpr(::mfw::stl::is_convertible_v<__S, __D>) { \
				__tmp = static_cast<__D>(__src); \
			} else if constexpr(::mfw::stl::is_assignable_v<__D, __S>) { \
				__tmp = __src; \
			}

	#define MFW_IMPLEMENT_AS_FUNC_GLOBAL_V1(name) \
		_MFW_IMPLEMENT_AS_FUNC_GLOBAL_START(name) \
		_MFW_IMPLEMENT_AS_FUNC_GLOBAL_END(name) \
			else if constexpr(::mfw::stl::test_funcs_##name<__D>::member::template has_to_v<__S>) { \
				__src.to_##name(__tmp); \
			} else if constexpr(::mfw::stl::test_funcs_##name<__D>::template global<__S>::has_to_v) { \
				::mfw::stl::to_##name(__src, __tmp); \
			} else { \
				static_assert(false); \
			} \
			return __tmp; \
		}

	#define MFW_IMPLEMENT_AS_FUNC_GLOBAL_V2(name) \
		_MFW_IMPLEMENT_AS_FUNC_GLOBAL_START(name) \
			if constexpr(::mfw::stl::test_funcs_##name<__D>::member::template has_to_v<__S>) { \
				__src.to_##name(__tmp); \
			} else if constexpr(::mfw::stl::test_funcs_##name<__D>::template global<__S>::has_to_v) { \
				to_##name(__src, __tmp); \
			} else \
		_MFW_IMPLEMENT_AS_FUNC_GLOBAL_END(name) \
			else { \
				MFW_DEBUGBREAK(); \
			} \
			return __tmp; \
		}

	#define MFW_IMPLEMENT_AS_FUNC_GLOBAL MFW_IMPLEMENT_AS_FUNC_GLOBAL_V2

	#define MFW_DECLARE_AS_TO_FUNC_GLOBAL(name) \
		MFW_DECLARE_TO_FUNC_GLOBAL(name) \
		MFW_DECLARE_AS_FUNC_GLOBAL(name)

	#define MFW_IMPLEMENT_AS_TO_FUNC_GLOBAL(name) \
		MFW_IMPLEMENT_TO_FUNC_GLOBAL(name) \
		MFW_IMPLEMENT_AS_FUNC_GLOBAL(name)

	#define MFW_DISABLE_COPY(name) \
		name &operator=(const name &other) noexcept = delete; \
		name(const name &other) noexcept = delete;

	#ifdef MFW_CPP_IF_CONSTEXPR_SUPPORTED
		#define MFW_IF_CONSTEXPR if constexpr
	#else
		#define MFW_IF_CONSTEXPR if
	#endif
#endif

#define type_cast(type, what) ((type)(what))
#define bool_cast(x) (static_cast<bool>(x))
#define int_cast(x) (bool_cast(x) ? 1 : 0)
#ifdef MFW_CPP
	#define overload_cast_static(ret, conv, name, ...) static_cast<ret (conv *)(__VA_ARGS__)>(name)
	#define overload_cast_member(ret, conv, type, name, cnst, ...) static_cast<ret (conv type::*)(__VA_ARGS__) cnst>(&type::name)
#endif

#endif