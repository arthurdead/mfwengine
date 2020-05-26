namespace mfw::stl
{
	inline char *&c_str(ucchar_t *&src)
	{ return reinterpret_cast<char *&>(src); }
	inline const char *c_str(const ucchar_t *src)
	{ return reinterpret_cast<const char *>(src); }
	inline wchar_t *&c_str(uwchar_t *&src)
	{ return reinterpret_cast<wchar_t *&>(src); }
	inline const wchar_t *c_str(const uwchar_t *src)
	{ return reinterpret_cast<const wchar_t *>(src); }

	inline const ucchar_t *uc_str(const char *src)
	{ return reinterpret_cast<const ucchar_t *>(src); }
	inline const uwchar_t *uc_str(const wchar_t *src)
	{ return reinterpret_cast<const uwchar_t *>(src); }
	inline ucchar_t *&uc_str(char *&src)
	{ return reinterpret_cast<ucchar_t *&>(src); }
	inline uwchar_t *&uc_str(wchar_t *&src)
	{ return reinterpret_cast<uwchar_t *&>(src); }
}