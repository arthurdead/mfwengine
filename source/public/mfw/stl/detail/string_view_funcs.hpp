#include <public/mfw/stl/stl.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/float.hpp>

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	namespace literals
	{
		constexpr ucstring_view operator""_sv(const ucchar_t *ptr, size_t len);
		constexpr uwstring_view operator""_sv(const uwchar_t *ptr, size_t len);
	}

	MFW_STL_API void MFW_STL_CALL to_string(ucstring_view src, uwstring &dst);
	MFW_STL_API void MFW_STL_CALL to_string(uwstring_view src, ucstring &dst);

	const char *c_str(ucstring_view src);
	const wchar_t *c_str(uwstring_view src);

	bool to_bool(ucstring_view src, bool &dst);
	bool to_bool(uwstring_view src, bool &dst);

	MFW_STL_API bool MFW_STL_CALL to_int(ucstring_view src, int8_t &dst, int32_t base = 10);
	MFW_STL_API bool MFW_STL_CALL to_int(uwstring_view src, int8_t &dst, int32_t base = 10);

	MFW_STL_API bool MFW_STL_CALL to_int(ucstring_view src, uint8_t &dst, int32_t base = 10);
	MFW_STL_API bool MFW_STL_CALL to_int(uwstring_view src, uint8_t &dst, int32_t base = 10);

	MFW_STL_API bool MFW_STL_CALL to_int(ucstring_view src, int32_t &dst, int32_t base = 10);
	MFW_STL_API bool MFW_STL_CALL to_int(uwstring_view src, int32_t &dst, int32_t base = 10);

	MFW_STL_API bool MFW_STL_CALL to_int(ucstring_view src, uint32_t &dst, int32_t base = 10);
	MFW_STL_API bool MFW_STL_CALL to_int(uwstring_view src, uint32_t &dst, int32_t base = 10);

	MFW_STL_API bool MFW_STL_CALL to_int(ucstring_view src, int64_t &dst, int32_t base = 10);
	MFW_STL_API bool MFW_STL_CALL to_int(uwstring_view src, int64_t &dst, int32_t base = 10);

	MFW_STL_API bool MFW_STL_CALL to_int(ucstring_view src, uint64_t &dst, int32_t base = 10);
	MFW_STL_API bool MFW_STL_CALL to_int(uwstring_view src, uint64_t &dst, int32_t base = 10);

	MFW_STL_API bool MFW_STL_CALL to_float(ucstring_view src, float32_t &dst);
	MFW_STL_API bool MFW_STL_CALL to_float(uwstring_view src, float32_t &dst);

	MFW_STL_API bool MFW_STL_CALL to_float(ucstring_view src, float64_t &dst);
	MFW_STL_API bool MFW_STL_CALL to_float(uwstring_view src, float64_t &dst);

	MFW_STL_API bool MFW_STL_CALL to_float(ucstring_view src, float80_t &dst);
	MFW_STL_API bool MFW_STL_CALL to_float(uwstring_view src, float80_t &dst);

	MFW_STL_API void MFW_STL_CALL to_lower(ucstring_view src, ucstring &dst);
	MFW_STL_API void MFW_STL_CALL to_lower(uwstring_view src, uwstring &dst);

	MFW_STL_API void MFW_STL_CALL to_upper(ucstring_view src, ucstring &dst);
	MFW_STL_API void MFW_STL_CALL to_upper(uwstring_view src, uwstring &dst);
	
	MFW_STL_API bool MFW_STL_CALL matches_pattern(ucstring_view str, ucstring_view pattern);
#else
	#error
#endif
}

#include <public/mfw/stl/impl/string_view_funcs.ipp>