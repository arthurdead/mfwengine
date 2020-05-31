#ifndef __MFW_PUBLIC_STL_DEFINES_H
#define __MFW_PUBLIC_STL_DEFINES_H

#pragma once

#include <public/mfw/stl/version.hpp>

#define MFW_BIT __MFW_BIT
#define __MFW_STRINGFY_IMPL ____MFW_STRINGFY_IMPL
#define MFW_STRINGFY __MFW_STRINGFY
#define MFW_FILE_LINE_STRING __MFW_FILE_LINE_STRING
#define MFW_PRAGMA __MFW_PRAGMA
#define MFW_MESSAGE __MFW_MESSAGE
#define MFW_MACRO_CONCATENATE __MFW_MACRO_CONCATENATE

#define MFW_NOTHING

#define __MFW_u8STRINGFY_IMPL(x) MFW_MACRO_CONCATENATE(u8, #x)
#define __MFW_u16STRINGFY_IMPL(x) MFW_MACRO_CONCATENATE(u, #x)
#define __MFW_u32STRINGFY_IMPL(x) MFW_MACRO_CONCATENATE(U, #x)
#define __MFW_wSTRINGFY_IMPL(x) MFW_MACRO_CONCATENATE(L, #x)

#define MFW_u8STRINGFY(x) __MFW_u8STRINGFY_IMPL(x)
#define MFW_u16STRINGFY(x) __MFW_u16STRINGFY_IMPL(x)
#define MFW_u32STRINGFY(x) __MFW_u32STRINGFY_IMPL(x)
#define MFW_wSTRINGFY(x) __MFW_wSTRINGFY_IMPL(x)

#if MFW_COMPILER_FLAGGED(MSVC)
	#define MFW_DECLSPEC(x) __declspec(x)
	#define MFW_ATTRIBUTE(x)
	#define MFW_SHARED_EXPORT MFW_DECLSPEC(dllexport)
	#define MFW_SHARED_IMPORT MFW_DECLSPEC(dllimport)
	#define MFW_SHARED_LOCAL 
	#define MFW_VISIBILITY_PUSH(x) 
	#define MFW_VISIBILITY_POP() 
	#define MFW_CALL_STDCALL __stdcall
	#define MFW_CALL_CDECL __cdecl
	#define MFW_OPTIMIZE(x) MFW_PRAGMA(optimize(x))
	#define MFW_RESET_OPTIONS()
	#define MFW_WARNING_DISABLE(...) MFW_PRAGMA(warning(disable: __VA_ARGS__))
	#define MFW_WARNING_SUPPRESS(...) MFW_PRAGMA(warning(suppress: __VA_ARGS__))
	#define __MFW_WARNING_PUSH_MSVC() MFW_PRAGMA(warning(push))
	#define __MFW_WARNING_POP_MSVC() MFW_PRAGMA(warning(pop))
	#if MFW_COMPILER_FLAGGED(CLANG)
		#define MFW_WARNING_DISABLE_UNIX(x) MFW_PRAGMA_UNIX(clang diagnostic ignored x)
		#define __MFW_WARNING_PUSH_UNIX() MFW_PRAGMA_UNIX(clang diagnostic push)
		#define __MFW_WARNING_POP_UNIX() MFW_PRAGMA_UNIX(clang diagnostic pop)
		#define MFW_WARNING_PUSH() __MFW_WARNING_PUSH_UNIX() __MFW_WARNING_PUSH_MSVC()
		#define MFW_WARNING_POP() __MFW_WARNING_POP_UNIX() __MFW_WARNING_POP_MSVC()
	#else
		#define MFW_WARNING_PUSH __MFW_WARNING_PUSH_MSVC
		#define MFW_WARNING_POP __MFW_WARNING_POP_MSVC
	#endif
	#define MFW_EXTENSION
#elif MFW_COMPILER_FLAGGED(UNIX)
	#define MFW_DECLSPEC(x)
	#if MFW_C_COMPARE(>=, 99)
		#define MFW_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
	#else
		#define MFW_ATTRIBUTE(x) __attribute__((x))
	#endif
	#define __MFW_VISIBILITY_DEFAULT MFW_ATTRIBUTE(__visibility__("default"))
	#if MFW_OS_IS(WINDOWS)
		#define MFW_SHARED_EXPORT MFW_ATTRIBUTE(__dllexport__) __MFW_VISIBILITY_DEFAULT
		#define MFW_SHARED_IMPORT MFW_ATTRIBUTE(__dllimport__) __MFW_VISIBILITY_DEFAULT
		#define __MFW_SYSTEM_ABI MFW_ATTRIBUTE(__ms_abi__)
	#elif MFW_OS_IS(LINUX)
		#define MFW_SHARED_EXPORT __MFW_VISIBILITY_DEFAULT
		#define MFW_SHARED_IMPORT __MFW_VISIBILITY_DEFAULT
		#define __MFW_SYSTEM_ABI MFW_ATTRIBUTE(__sysv_abi__)
	#else
		#error
	#endif
	#define MFW_SHARED_LOCAL MFW_ATTRIBUTE(__visibility__("hidden"))
	#define MFW_CALL_STDCALL MFW_ATTRIBUTE(__stdcall__, __ms_abi__)
	#if MFW_COMPILER_FLAGGED(CLANG)
		#define MFW_CALL_CDECL __MFW_SYSTEM_ABI MFW_ATTRIBUTE(__cdecl__)
		#define __MFW_PRAGMA_ID clang
	#elif MFW_COMPILER_IS(GCC)
		#define MFW_CALL_CDECL __MFW_SYSTEM_ABI
		#define __MFW_PRAGMA_ID GCC
	#else
		#error
	#endif
	#define MFW_VISIBILITY_PUSH(x) MFW_PRAGMA(__MFW_PRAGMA_ID visibility push(x))
	#define MFW_VISIBILITY_POP() MFW_PRAGMA(__MFW_PRAGMA_ID visibility pop)
	#define MFW_OPTIMIZE(x) MFW_PRAGMA(__MFW_PRAGMA_ID optimize(x))
	#define MFW_RESET_OPTIONS() MFW_PRAGMA(__MFW_PRAGMA_ID reset_options)
	#define MFW_WARNING_PUSH() MFW_PRAGMA(__MFW_PRAGMA_ID diagnostic push)
	#define MFW_WARNING_POP() MFW_PRAGMA(__MFW_PRAGMA_ID diagnostic pop)
	#define MFW_WARNING_DISABLE(x) MFW_PRAGMA(__MFW_PRAGMA_ID diagnostic ignored x)
	#define MFW_WARNING_DISABLE_UNIX MFW_WARNING_DISABLE
	#define MFW_EXTENSION __extension__
#else
	#error
#endif

#if MFW_OS_IS(WINDOWS)
	#define MFW_CALL_SHARED MFW_CALL_STDCALL
#elif MFW_OS_IS(LINUX)
	#define MFW_CALL_SHARED MFW_CALL_CDECL
#else
	#error
#endif

#define __MFW_ENABLE_DEBUGBREAK
#if MFW_CONFIGURATION_IS(DEBUG) || defined __MFW_ENABLE_DEBUGBREAK
	#if MFW_COMPILER_FLAGGED(MSVC)
		#define MFW_DEBUGBREAK() __debugbreak()
	#elif MFW_COMPILER_FLAGGED(CLANG)
		#define MFW_DEBUGBREAK() __builtin_debugtrap()
	#elif MFW_COMPILER_IS(GCC)
		#if MFW_PROCESSOR_FLAGGED(X86)
			#define MFW_DEBUGBREAK() __asm__("int3")
		#else
			#define MFW_DEBUGBREAK() __builtin_trap()
		#endif
	#else
		#error
	#endif
#else
	#define MFW_DEBUGBREAK()
#endif

#ifdef MFW_CPP
	#define __MFW_ENUM_SHARED_EQUAL_IMPL(acessor, name, arg, type) \
		acessor constexpr inline bool operator==(arg rhs, name lhs) { return static_cast<type>(rhs) == static_cast<type>(lhs); } \
		acessor constexpr inline bool operator!=(arg rhs, name lhs) { return static_cast<type>(rhs) != static_cast<type>(lhs); } \
		acessor constexpr inline bool operator>(arg rhs, name lhs) { return static_cast<type>(rhs) > static_cast<type>(lhs); } \
		acessor constexpr inline bool operator<(arg rhs, name lhs) { return static_cast<type>(rhs) < static_cast<type>(lhs); } \
		acessor constexpr inline bool operator>=(arg rhs, name lhs) { return static_cast<type>(rhs) >= static_cast<type>(lhs); } \
		acessor constexpr inline bool operator<=(arg rhs, name lhs) { return static_cast<type>(rhs) <= static_cast<type>(lhs); }

	#define __MFW_ENUM_SHARED_EQUAL_ADDS_IMPL(acessor, name, arg, type) \
		acessor constexpr inline name operator+=(name &rhs, arg lhs) { rhs = static_cast<name>(static_cast<type>(rhs) + static_cast<type>(lhs)); return rhs; } \
		acessor constexpr inline name operator-=(name &rhs, arg lhs) { rhs = static_cast<name>(static_cast<type>(rhs) + static_cast<type>(lhs)); return rhs; }

	#define __MFW_ENUM_SHARED_AND_BIT_IMPL(acessor, name, arg, type) \
		acessor constexpr inline name operator&(name rhs, arg lhs) { return static_cast<name>(static_cast<type>(rhs) & static_cast<type>(lhs)); }

	#define __MFW_ENUM_SHARED_AND_FLAG_IMPL(acessor, name, arg, type) \
		acessor constexpr inline bool operator&(name rhs, arg lhs) { return ((static_cast<type>(rhs) & static_cast<type>(lhs)) == (static_cast<type>(lhs))); }

	#define __MFW_ENUM_SHARED_FLAGS_IMPL(acessor, name, arg, type) \
		acessor constexpr inline name operator|(name rhs, arg lhs) { return static_cast<name>(static_cast<type>(rhs) | static_cast<type>(lhs)); } \
		acessor constexpr inline name operator^(name rhs, arg lhs) { return static_cast<name>(static_cast<type>(rhs) ^ static_cast<type>(lhs)); } \
		acessor constexpr inline name &operator&=(name &rhs, arg lhs) { rhs = static_cast<name>(static_cast<type>(rhs) & static_cast<type>(lhs)); return rhs; } \
		acessor constexpr inline name &operator|=(name &rhs, arg lhs) { rhs = static_cast<name>(static_cast<type>(rhs) | static_cast<type>(lhs)); return rhs; } \

	#define __MFW_ENUM_SHARED_UNARY_ADD_IMPL(acessor, name, type) \
		acessor constexpr inline name &operator++(name &rhs) { rhs = static_cast<name>(static_cast<type>(rhs) + static_cast<type>(1)); return rhs; } \
		acessor constexpr inline name &operator--(name &rhs) { rhs = static_cast<name>(static_cast<type>(rhs) - static_cast<type>(1)); return rhs; } \
		acessor constexpr inline name &operator++(name &rhs,int) { rhs = static_cast<name>(static_cast<type>(rhs) + static_cast<type>(1)); return rhs; } \
		acessor constexpr inline name &operator--(name &rhs,int) { rhs = static_cast<name>(static_cast<type>(rhs) - static_cast<type>(1)); return rhs; }

	#define __MFW_ENUM_SHARED_UNARY_FLAGS_IMPL(acessor, name, type) \
		acessor constexpr inline name operator~(name rhs) { return static_cast<name>(~static_cast<type>(rhs)); }

	#define __MFW_ENUM_SHARED_UNARY_FLAGS(acessor, name) \
		__MFW_ENUM_SHARED_UNARY_FLAGS_IMPL(acessor, name, ::mfw::stl::underlying_type_t<name>)

	#define __MFW_ENUM_SHARED_UNARY_ADD(acessor, name) \
		__MFW_ENUM_SHARED_UNARY_ADD_IMPL(acessor, name, ::mfw::stl::underlying_type_t<name>)

	#define __MFW_ENUM_SHARED_BIT_AND(acessor, name) \
		__MFW_ENUM_SHARED_AND_BIT_IMPL(acessor, name, int, ::mfw::stl::underlying_type_t<name>) \
		__MFW_ENUM_SHARED_AND_BIT_IMPL(acessor, name, name, ::mfw::stl::underlying_type_t<name>)
		
	#define __MFW_ENUM_SHARED_FLAG_AND(acessor, name) \
		__MFW_ENUM_SHARED_AND_FLAG_IMPL(acessor, name, int, ::mfw::stl::underlying_type_t<name>) \
		__MFW_ENUM_SHARED_AND_FLAG_IMPL(acessor, name, name, ::mfw::stl::underlying_type_t<name>)

	#define __MFW_ENUM_SHARED_FLAGS(acessor, name) \
		__MFW_ENUM_SHARED_FLAGS_IMPL(acessor, name, int, ::mfw::stl::underlying_type_t<name>) \
		__MFW_ENUM_SHARED_FLAGS_IMPL(acessor, name, name, ::mfw::stl::underlying_type_t<name>)

	#define __MFW_ENUM_SHARED_EQUAL_ADDS(acessor, name) \
		__MFW_ENUM_SHARED_EQUAL_ADDS_IMPL(acessor, name, int, ::mfw::stl::underlying_type_t<name>) \
		__MFW_ENUM_SHARED_EQUAL_ADDS_IMPL(acessor, name, name, ::mfw::stl::underlying_type_t<name>)

	#define __MFW_ENUM_SHARED_EQUAL(acessor, name) \
		__MFW_ENUM_SHARED_EQUAL_IMPL(acessor, name, int, ::mfw::stl::underlying_type_t<name>) \
		__MFW_ENUM_SHARED_EQUAL_IMPL(acessor, name, name, ::mfw::stl::underlying_type_t<name>)

	#define __MFW_ENUM_FLAGS_IMPL_BASE(acessor, name) \
		__MFW_ENUM_SHARED_EQUAL(acessor, name) \
		__MFW_ENUM_SHARED_FLAGS(acessor, name) \
		__MFW_ENUM_SHARED_UNARY_FLAGS(acessor, name)

	#define __MFW_ENUM_FLAGS_IMPL_V2(acessor, name) \
		__MFW_ENUM_FLAGS_IMPL_BASE(acessor, name) \
		__MFW_ENUM_SHARED_FLAG_AND(acessor, name)
		
	#define __MFW_ENUM_FLAGS_IMPL_V1(acessor, name) \
		__MFW_ENUM_FLAGS_IMPL_BASE(acessor, name) \
		__MFW_ENUM_SHARED_BIT_AND(acessor, name)

	#define __MFW_ENUM_IMPL(acessor, name) \
		__MFW_ENUM_SHARED_EQUAL(acessor, name) \
		__MFW_ENUM_SHARED_UNARY_ADD(acessor, name)

	#define __MFW_ENUM_INT_IMPL(acessor, name) \
		__MFW_ENUM_SHARED_EQUAL(acessor, name) \
		__MFW_ENUM_SHARED_EQUAL_ADDS(acessor, name) \
		__MFW_ENUM_SHARED_BIT_AND(acessor, name) \
		__MFW_ENUM_SHARED_FLAGS(acessor, name) \
		__MFW_ENUM_SHARED_UNARY_FLAGS(acessor, name) \
		__MFW_ENUM_SHARED_UNARY_ADD(acessor, name)

	#define MFW_CLASS_ENUM_FLAGS_V1(name) __MFW_ENUM_FLAGS_IMPL_V1(friend, name)
	#define MFW_ENUM_FLAGS_V1(name) __MFW_ENUM_FLAGS_IMPL_V1(MFW_NOTHING, name)

	#define MFW_CLASS_ENUM_FLAGS_V2(name) __MFW_ENUM_FLAGS_IMPL_V2(friend, name)
	#define MFW_ENUM_FLAGS_V2(name) __MFW_ENUM_FLAGS_IMPL_V2(MFW_NOTHING, name)

	#define MFW_CLASS_ENUM_FLAGS(name) MFW_CLASS_ENUM_FLAGS_V1(name)
	#define MFW_ENUM_FLAGS(name) MFW_ENUM_FLAGS_V1(name)

	#define MFW_CLASS_ENUM(name) __MFW_ENUM_IMPL(friend, name)
	#define MFW_ENUM(name) __MFW_ENUM_IMPL(MFW_NOTHING, name)

	#define MFW_CLASS_ENUM_INT(name) __MFW_ENUM_INT_IMPL(friend, name)
	#define MFW_ENUM_INT(name) __MFW_ENUM_INT_IMPL(MFW_NOTHING, name)

	#define MFW_EXECUTE_N_TIMES(num, ...) \
		static ::mfw::stl::uint8_t __exec_##num##_times{0}; \
		if(__exec_##num##_times < num) { \
			__VA_ARGS__ \
			__exec_##num##_times++; \
		} \

	#if MFW_COMPILER_FLAGGED(CLANG)
		MFW_WARNING_DISABLE_UNIX("-Wgnu-zero-variadic-macro-arguments")
		MFW_MESSAGE("remove this later")
	#endif

	MFW_MESSAGE("TODO!!! below does not work with templates")
	#define MFW_DECLARE_FUNC_CHECK_MEMBER(name, ret, func, cnst, ...) \
		template <typename T> \
		class name \
		{ \
			template <typename R, typename C, typename ...Args> \
			static constexpr auto check(::mfw::stl::nullptr_t) -> decltype(::mfw::stl::declval<C>().func(::mfw::stl::declval<Args>()...)); \
			template <typename R, typename C, typename ...Args> \
			static constexpr ::mfw::stl::false_type check(...); \
		public: \
			static inline constexpr bool value{::mfw::stl::is_same_v<decltype(check<ret, cnst T, ##__VA_ARGS__>(nullptr)), ret>}; \
			template <typename R, typename ...Args> \
			static inline constexpr bool value_overload{::mfw::stl::is_same_v<decltype(check<R, cnst T, Args...>(nullptr)), R>}; \
		}; \
		template <typename T> \
		static inline constexpr bool name##_v = name<T>::value; \
		template <typename T, typename R, typename ...Args> \
		static inline constexpr bool name##_overload_v = name<T>::template value_overload<R, Args...>;

	MFW_MESSAGE("TODO!!! below does not work with templates")
	#define MFW_DECLARE_FUNC_CHECK_GLOBAL(name, ret, func, ...) \
		class name \
		{ \
			template <typename R, typename ...Args> \
			static constexpr auto check(::mfw::stl::nullptr_t) -> decltype(func(::mfw::stl::declval<Args>()...)); \
			template <typename R, typename ...Args> \
			static constexpr ::mfw::stl::false_type check(...); \
		public: \
			static inline constexpr bool value{::mfw::stl::is_same_v<decltype(check<ret, ##__VA_ARGS__>(nullptr)), ret>}; \
			template <typename R, typename ...Args> \
			static inline constexpr bool value_overload{::mfw::stl::is_same_v<decltype(check<R, Args...>(nullptr)), R>}; \
		}; \
		static inline constexpr bool name##_v = name::value; \
		template <typename R, typename ...Args> \
		static inline constexpr bool name##_overload_v = name::template value_overload<R, Args...>;

	#define MFW_DECLARE_AS_TO_CHECK_FUNCS_BEGIN(name) \
		template <typename V> \
		struct test_funcs_##name \
		{ \
			struct member \
			{
				
	#define MFW_DECLARE_AS_TO_CHECK_FUNCS_END(name) \
				MFW_DECLARE_FUNC_CHECK_MEMBER(has_to, void, to_##name, const, V &) \
			}; \
			template <typename T> \
			struct global \
			{ \
				MFW_DECLARE_FUNC_CHECK_GLOBAL(has_as, V, as_##name, const T &) \
				MFW_DECLARE_FUNC_CHECK_GLOBAL(has_to, void, to_##name, const T &, V &) \
			}; \
		};

	#define MFW_DECLARE_AS_TO_CHECK_FUNCS_V2(name) \
		MFW_DECLARE_AS_TO_CHECK_FUNCS_BEGIN(name) \
		MFW_DECLARE_FUNC_CHECK_MEMBER(has_as, V, as_##name, const, void) \
		MFW_DECLARE_AS_TO_CHECK_FUNCS_END(name)

	#define MFW_DECLARE_AS_TO_CHECK_FUNCS_V1(name) \
		MFW_DECLARE_AS_TO_CHECK_FUNCS_BEGIN(name) \
		MFW_DECLARE_FUNC_CHECK_MEMBER(has_as, V, as_##name, const) \
		MFW_DECLARE_AS_TO_CHECK_FUNCS_END(name)

	#define MFW_DECLARE_AS_TO_CHECK_FUNCS MFW_DECLARE_AS_TO_CHECK_FUNCS_V2

	#define MFW_DECLARE_TO_FUNC_GLOBAL(name)

	#define MFW_DECLARE_AS_FUNC_GLOBAL(name) \
		template <typename D, typename S> \
		D as_##name(const S &src);

	MFW_MESSAGE("TODO!! remake both below once MFW_DECLARE_FUNC_CHECK_MEMBER/GLOBAL supports templates")
	#define MFW_IMPLEMENT_TO_FUNC_GLOBAL(name)

	#define __MFW_IMPLEMENT_AS_FUNC_GLOBAL_START(name) \
		template <typename D, typename S> \
		D as_##name(const S &src) \
		{ \
			using __D = ::mfw::stl::remove_cvref_t<D>; \
			using __S = ::mfw::stl::remove_cvref_t<S>; \
			__D dst{}; \

	#define __MFW_IMPLEMENT_AS_FUNC_GLOBAL_END(name) \
			if constexpr(::mfw::stl::is_same_v<__D, __S>) { \
				dst = src; \
			} else if constexpr(::mfw::stl::is_constructible_v<__D, __S>) { \
				dst = __D{src}; \
			} else if constexpr(::mfw::stl::is_convertible_v<__S, __D>) { \
				dst = static_cast<D>(src); \
			} else if constexpr(::mfw::stl::is_assignable_v<__D, __S>) { \
				dst = src; \
			}

	#define MFW_IMPLEMENT_AS_FUNC_GLOBAL_V1(name) \
		__MFW_IMPLEMENT_AS_FUNC_GLOBAL_START(name) \
		__MFW_IMPLEMENT_AS_FUNC_GLOBAL_END(name) \
			else if constexpr(test_funcs_##name<__D>::member::template has_to_v<__S>) { \
				src.to_##name(dst); \
			} else if constexpr(test_funcs_##name<__D>::template global<__S>::has_to_v) { \
				to_##name(src, dst); \
			} else { \
				MFW_DEBUGBREAK(); \
			} \
			return dst; \
		}

	#define MFW_IMPLEMENT_AS_FUNC_GLOBAL_V2(name) \
		__MFW_IMPLEMENT_AS_FUNC_GLOBAL_START(name) \
			if constexpr(test_funcs_##name<__D>::member::template has_to_v<__S>) { \
				src.to_##name(dst); \
			} else if constexpr(test_funcs_##name<__D>::template global<__S>::has_to_v) { \
				to_##name(src, dst); \
			} else \
		__MFW_IMPLEMENT_AS_FUNC_GLOBAL_END(name) \
			else { \
				MFW_DEBUGBREAK(); \
			} \
			return dst; \
		}

	#define MFW_IMPLEMENT_AS_FUNC_GLOBAL MFW_IMPLEMENT_AS_FUNC_GLOBAL_V2

	#define MFW_DECLARE_AS_TO_FUNC_GLOBAL(name) \
		MFW_DECLARE_TO_FUNC_GLOBAL(name) \
		MFW_DECLARE_AS_FUNC_GLOBAL(name)

	#define MFW_IMPLEMENT_AS_TO_FUNC_GLOBAL(name) \
		MFW_IMPLEMENT_TO_FUNC_GLOBAL(name) \
		MFW_IMPLEMENT_AS_FUNC_GLOBAL(name)

	#define MFW_DISABLE_COPY(name) \
		name &operator=(const name &other) = delete; \
		name(const name &other) = delete;

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