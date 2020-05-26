#include <public/mfw/stl/type_traits.hpp>

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	namespace __tuple_funcs_internal
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

		#define __MFW_TUPLE_TO_VECTOR_CONVERT(name) \
			if constexpr(test_funcs_##name<__T>::member::template has_to_v<__V>) { \
				T str{}; \
				tup_val.to_##name(str); \
				vec.emplace_back(move(str)); \
			} else if constexpr(test_funcs_##name<__T>::template global<__V>::has_to_v) { \
				T str{}; \
				to_##name(tup_val, str); \
				vec.emplace_back(move(str)); \
			}

		#define __MFW_TUPLE_TO_VECTOR_IMPLEMENT_START(name) \
			template <size_t i = 0, typename T, typename ...Args> \
			bool to_vector##name(const tuple<Args...> &tup, vector<T> &vec) \
			{ \
				if constexpr(i == sizeof...(Args)) { \
					return true; \
				} else { \
					const auto &tup_val{get<i>(tup)}; \
					using V = decltype(tup_val); \
					using __V = remove_cvref_t<V>; \
					using __T = remove_cvref_t<T>;

		#define __MFW_TUPLE_TO_VECTOR_IMPLEMENT_END(name) \
					if constexpr(is_same_v<__T, __V>) { \
						vec.emplace_back(tup_val); \
					} else if constexpr(is_constructible_v<__T, __V>) { \
						vec.emplace_back(tup_val); \
					} else if constexpr(is_convertible_v<__V, __T>) { \
						vec.emplace_back(static_cast<__T>(tup_val)); \
					} else if constexpr(is_assignable_v<__T, __V>) { \
						__T vec_val{}; \
						vec_val = tup_val; \
						vec.emplace_back(move(vec_val)); \
					} else { \
						MFW_DEBUGBREAK(); \
						return false; \
					} \
					return to_vector##name<i + 1>(tup, vec); \
				} \
			}

		#define __MFW_DECLARE_TUPLE_TO_VECTOR(name) \
			template <typename T, typename ...Args> \
			bool to_vector##name(const tuple<Args...> &tup, vector<T> &vec);

		#define __MFW_IMPLEMENT_TUPLE_TO_VECTOR(name, space) \
			template <typename T, typename ...Args> \
			bool to_vector##name(const tuple<Args...> &tup, vector<T> &vec) \
			{ return space to_vector##name(tup, vec); }

		__MFW_TUPLE_TO_VECTOR_IMPLEMENT_START(MFW_NOTHING)
		__MFW_TUPLE_TO_VECTOR_IMPLEMENT_END(MFW_NOTHING)

		#define MFW_IMPLEMENT_TUPLE_TO_VECTOR_INTERNAL(name) \
			__MFW_TUPLE_TO_VECTOR_IMPLEMENT_START(_##name) \
			__MFW_TUPLE_TO_VECTOR_CONVERT(name) \
			else \
			__MFW_TUPLE_TO_VECTOR_IMPLEMENT_END(_##name)

		#define MFW_IMPLEMENT_TUPLE_TO_VECTOR(name, space) \
			__MFW_IMPLEMENT_TUPLE_TO_VECTOR(_##name, space)

		#define MFW_DECLARE_TUPLE_TO_VECTOR(name) \
			__MFW_DECLARE_TUPLE_TO_VECTOR(_##name)
	}

	__MFW_IMPLEMENT_TUPLE_TO_VECTOR(MFW_NOTHING, __tuple_funcs_internal::)
#else
	#error
#endif
}