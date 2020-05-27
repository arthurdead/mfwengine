#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/string_view.hpp>
#include <public/mfw/stl/limits.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <charconv>
	#include <cctype>
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	namespace __string_view_funcs_internal
	{
		template <typename T, typename T2>
		bool __to_int_impl(ucstring_view src, T &dst, int32_t base = 10)
		{
			if(src == u8"true"_sv) {
				dst = static_cast<T>(1);
				return true;
			} else if(src == u8"false"_sv) {
				dst = static_cast<T>(0);
				return true;
			} else if(src.empty()) {
				dst = static_cast<T>(numeric_limits<T2>::max());
				return false;
			}
			
			using ::MFW_STD_NAMESPACE::from_chars_result;
			using ::MFW_STD_NAMESPACE::from_chars;
			MFW_MESSAGE("move errc elsewhere")
			using ::MFW_STD_NAMESPACE::errc;

			const char *begin{reinterpret_cast<const char *>(&(*src.cbegin()))};
			const char *end{reinterpret_cast<const char *>(&(*src.cend()))};
			from_chars_result res{from_chars(begin, end, reinterpret_cast<T2 &>(dst), base)};

			if(res.ec == errc::invalid_argument || res.ptr == begin) {
				dst = static_cast<T>(numeric_limits<T2>::max());
				return false;
			} else {
				if(res.ptr != end) {
					MFW_MESSAGE("TODO")
				}
				return true;
			}
		}

		template <typename T>
		bool to_int(ucstring_view src, T &dst, int32_t base = 10)
		{ return __to_int_impl<T, T>(src, dst, base); }

		template <>
		bool to_int<int8_t>(ucstring_view src, int8_t &dst, int32_t base)
		{ return __to_int_impl<int8_t, int16_t>(src, dst, base); }

		template <>
		bool to_int<uint8_t>(ucstring_view src, uint8_t &dst, int32_t base)
		{ return __to_int_impl<uint8_t, uint16_t>(src, dst, base); }

		template <typename T, typename F>
		bool to_float(ucstring_view src, T &dst, F func)
		{
			if(src == u8"true"_sv) {
				dst = static_cast<T>(1.0f);
				return true;
			} else if(src == u8"false"_sv) {
				dst = static_cast<T>(0.0f);
				return true;
			} else if(src.empty()) {
				dst = numeric_limits<T>::max();
				return false;
			}
			
			ucchar_t *end{nullptr};
			const ucchar_t *start{&(*src.cbegin())};
			T ff{func(c_str(start), &c_str(end))};
			if(end == start) {
				dst = numeric_limits<T>::max();
				return false;
			} else {
				if(end != &(*src.cend())) {
					MFW_MESSAGE("TODO")
				}
				dst = ff;
				return true;
			}
		}
	}

	MFW_STL_API bool MFW_STL_CALL to_int(ucstring_view src, int8_t &dst, int32_t base)
	{
		return __string_view_funcs_internal::to_int(src, dst, base);
	}

	MFW_STL_API bool MFW_STL_CALL to_int(ucstring_view src, uint8_t &dst, int32_t base)
	{
		return __string_view_funcs_internal::to_int(src, dst, base);
	}

	MFW_STL_API bool MFW_STL_CALL to_int(ucstring_view src, int16_t &dst, int32_t base)
	{
		return __string_view_funcs_internal::to_int(src, dst, base);
	}

	MFW_STL_API bool MFW_STL_CALL to_int(ucstring_view src, uint16_t &dst, int32_t base)
	{
		return __string_view_funcs_internal::to_int(src, dst, base);
	}

	MFW_STL_API bool MFW_STL_CALL to_int(ucstring_view src, int32_t &dst, int32_t base)
	{
		return __string_view_funcs_internal::to_int(src, dst, base);
	}

	MFW_STL_API bool MFW_STL_CALL to_int(ucstring_view src, uint32_t &dst, int32_t base)
	{
		return __string_view_funcs_internal::to_int(src, dst, base);
	}

	MFW_STL_API bool MFW_STL_CALL to_int(ucstring_view src, int64_t &dst, int32_t base)
	{
		return __string_view_funcs_internal::to_int(src, dst, base);
	}

	MFW_STL_API bool MFW_STL_CALL to_int(ucstring_view src, uint64_t &dst, int32_t base)
	{
		return __string_view_funcs_internal::to_int(src, dst, base);
	}

	MFW_STL_API bool MFW_STL_CALL to_float(ucstring_view src, float32_t &dst)
	{
		return __string_view_funcs_internal::to_float(src, dst, strtof);
	}

	MFW_STL_API bool MFW_STL_CALL to_float(ucstring_view src, float64_t &dst)
	{
		return __string_view_funcs_internal::to_float(src, dst, strtod);
	}

	MFW_STL_API bool MFW_STL_CALL to_float(ucstring_view src, float80_t &dst)
	{
		return __string_view_funcs_internal::to_float(src, dst, strtold);
	}

	MFW_STL_API void MFW_STL_CALL to_upper(ucstring_view src, ucstring &dst)
	{
		dst = src;
		transform(dst.begin(), dst.end(), dst.begin(), static_cast<int32_t(*)(int32_t)>(::MFW_STD_NAMESPACE::toupper));
	}
#else
	#error
#endif
}