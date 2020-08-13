#include <public/mfw/stl/type_traits.hpp>

namespace mfw::stl
{
	namespace __public_impl_tuple_funcs_tpp_internal
	{
		MFW_MESSAGE("TODO!! add back below to __MFW_TUPLE_TO_VECTOR_CONVERT once MFW_DECLARE_FUNC_CHECK_MEMBER/GLOBAL supports templates")
		/*
		else if constexpr(test_funcs_##name<V>::member::template has_as_v<T>) { \
			vec.emplace_back(val.template as_##name<V>()); \
			return true; \
		} else if constexpr(test_funcs_##name<V>::template global<T>::has_as_v) { \
			vec.emplace_back(as_##name<V>(val)); \
			return true; \
		}
		*/

		#define _MFW_TUPLE_TO_VECTOR_CONVERT(name) \
			if constexpr(test_funcs_##name<__T>::member::template has_to_v<__V>) { \
				__T __str{}; \
				__tup_val.to_##name(__str); \
				__vec.emplace_back(move(__str)); \
			} else if constexpr(test_funcs_##name<__T>::template global<__V>::has_to_v) { \
				__T __str{}; \
				to_##name(__tup_val, __str); \
				__vec.emplace_back(move(__str)); \
			}

		#define _MFW_TUPLE_TO_VECTOR_IMPLEMENT_START(name) \
			template <size_t _Ip = 0, typename _Tp, typename... _Args> \
			MFW_VISIBILITY_LOCAL static bool to_vector##name(tuple_view<_Args...> __tup, vector<_Tp> &__vec) noexcept \
			{ \
				if constexpr(_Ip == sizeof...(_Args)) { \
					return true; \
				} else { \
					const auto &__tup_val{get<_Ip>(__tup)}; \
					using __tup_val_t = decltype(__tup_val); \
					using __V = remove_cvref_t<__tup_val_t>; \
					using __T = remove_cvref_t<_Tp>;

		#define _MFW_TUPLE_TO_VECTOR_IMPLEMENT_END(name) \
					if constexpr(is_same_v<__T, __V>) { \
						__vec.emplace_back(__tup_val); \
					} else if constexpr(is_constructible_v<__T, __V>) { \
						__vec.emplace_back(__tup_val); \
					} else if constexpr(is_convertible_v<__V, __T>) { \
						__vec.emplace_back(static_cast<__T>(__tup_val)); \
					} else if constexpr(is_assignable_v<__T, __V>) { \
						__T __vec_val{}; \
						__vec_val = __tup_val; \
						__vec.emplace_back(move(__vec_val)); \
					} else { \
						static_assert(false); \
						return false; \
					} \
					return to_vector##name<_Ip + 1>(__tup, __vec); \
				} \
			}

		#define _MFW_DECLARE_TUPLE_TO_VECTOR_IMPL(name) \
			template <typename _Tp, typename... _Args> \
			MFW_VISIBILITY_LOCAL bool to_vector##name(tuple_view<_Args...> __tup, vector<_Tp> &__vec) noexcept;

		#define _MFW_IMPLEMENT_TUPLE_TO_VECTOR_IMPL(name, space) \
			template <typename _Tp, typename... _Args> \
			MFW_VISIBILITY_LOCAL bool to_vector##name(tuple_view<_Args...> __tup, vector<_Tp> &__vec) noexcept \
			{ return space to_vector##name(__tup, __vec); }

		_MFW_TUPLE_TO_VECTOR_IMPLEMENT_START(MFW_NOTHING)
		_MFW_TUPLE_TO_VECTOR_IMPLEMENT_END(MFW_NOTHING)

		#define MFW_IMPLEMENT_TUPLE_TO_VECTOR_INTERNAL(name) \
			_MFW_TUPLE_TO_VECTOR_IMPLEMENT_START(_##name) \
			_MFW_TUPLE_TO_VECTOR_CONVERT(name) \
			else \
			_MFW_TUPLE_TO_VECTOR_IMPLEMENT_END(_##name)

		#define MFW_IMPLEMENT_TUPLE_TO_VECTOR(name, space) \
			_MFW_IMPLEMENT_TUPLE_TO_VECTOR_IMPL(_##name, space)

		#define MFW_DECLARE_TUPLE_TO_VECTOR(name) \
			_MFW_DECLARE_TUPLE_TO_VECTOR_IMPL(_##name)
	}

	_MFW_IMPLEMENT_TUPLE_TO_VECTOR_IMPL(MFW_NOTHING, __public_impl_tuple_funcs_tpp_internal::)
}