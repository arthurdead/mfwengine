#include <public/mfw/stl/stl.hpp>
#include <public/mfw/stl/string_view.hpp>

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	namespace literals
	{
		ucstring operator""_s(const ucchar_t *ptr, size_t len);
		uwstring operator""_s(const uwchar_t *ptr, size_t len);
		pstring operator""_p(const upchar_t *ptr, size_t len);
	}

	MFW_STL_API size_t MFW_STL_CALL replace_all(ucstring &str, ucstring_view from, ucstring_view to);
	MFW_STL_API size_t MFW_STL_CALL replace_all(ucstring &str, ucchar_t from, ucchar_t to);

	MFW_STL_API size_t MFW_STL_CALL replace_all(uwstring &str, uwstring_view from, uwstring_view to);
	MFW_STL_API size_t MFW_STL_CALL replace_all(uwstring &str, uwchar_t from, uwchar_t to);

	char *c_str(ucstring &src);
	wchar_t *c_str(uwstring &src);

	const char *c_str(const ucstring &src);
	const wchar_t *c_str(const uwstring &src);

	#ifdef __MFW_STD_FILESYSTEM_WIDE_CHAR
	const wchar_t *c_str(const pstring &src);
	#else
	const char *c_str(const pstring &src);
	#endif

	const upchar_t *uc_str(const pstring &src);

	void to_string(const pstring &src, ucstring &dst);
	void to_string(const pstring &src, uwstring &dst);

	#ifdef __MFW_STD_FILESYSTEM_WIDE_CHAR
	void to_string(const pstring &src, uwstring &dst);
	#else
	void to_string(const pstring &src, ucstring &dst);
	#endif

	void to_string(const ucstring &src, pstring &dst);
#else
	#error
#endif
}

#include <public/mfw/stl/impl/string_funcs.ipp>

namespace mfw::stl
{
	MFW_DECLARE_AS_FUNC_GLOBAL(string)
}

#ifdef MFW_DEPENDS_ON_CORE
	#include <public/mfw/stl/detail/string_funcs_core.hpp>
#endif