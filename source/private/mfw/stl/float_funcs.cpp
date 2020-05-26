#include <public/mfw/stl/float.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <cstdio>
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	namespace __float_funcs_internal
	{
		MFW_MESSAGE("replace both with tochars on msvc")
		template <typename S>
		void to_string(S src, ucstring &dst, const char *fmt)
		{
			dst.clear();
			int32_t size{snprintf(nullptr, 0, fmt, src)};
			dst.resize(size);
			snprintf(c_str(dst), size+1, fmt, src);
		}

		template <typename S>
		void to_string(S src, uwstring &dst, const wchar_t *fmt)
		{
			dst.clear();
			int32_t size{swprintf(nullptr, 0, fmt, src)};
			dst.resize(size);
			swprintf(c_str(dst), size+1, fmt, src);
		}
	}

	MFW_STL_API void MFW_STL_CALL to_string(float32_t src, ucstring &dst)
	{
		__float_funcs_internal::to_string(src, dst, "%f");
	}

	MFW_STL_API void MFW_STL_CALL to_string(float32_t src, uwstring &dst)
	{
		__float_funcs_internal::to_string(src, dst, L"%f");
	}

	MFW_STL_API void MFW_STL_CALL to_string(float64_t src, ucstring &dst)
	{
		__float_funcs_internal::to_string(src, dst, "%lf");
	}

	MFW_STL_API void MFW_STL_CALL to_string(float64_t src, uwstring &dst)
	{
		__float_funcs_internal::to_string(src, dst, L"%lf");
	}

	MFW_STL_API void MFW_STL_CALL to_string(float80_t src, ucstring &dst)
	{
		__float_funcs_internal::to_string(src, dst, "%Lf");
	}

	MFW_STL_API void MFW_STL_CALL to_string(float80_t src, uwstring &dst)
	{
		__float_funcs_internal::to_string(src, dst, L"%Lf");
	}
#else
	#error
#endif
}