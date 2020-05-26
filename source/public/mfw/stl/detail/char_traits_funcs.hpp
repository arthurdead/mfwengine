namespace mfw::stl
{
	char *&c_str(ucchar_t *&src);
	const char *c_str(const ucchar_t *src);
	wchar_t *&c_str(uwchar_t *&src);
	const wchar_t *c_str(const uwchar_t *src);

	const ucchar_t *uc_str(const char *src);
	const uwchar_t *uc_str(const wchar_t *src);
	ucchar_t *&uc_str(char *&src);
	uwchar_t *&uc_str(wchar_t *&src);
}

#include <public/mfw/stl/impl/char_traits_funcs.ipp>