namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	namespace literals
	{
		inline ucstring operator""_s(const ucchar_t *ptr, size_t len)
		{ return ucstring{ptr, len}; }
		inline uwstring operator""_s(const uwchar_t *ptr, size_t len)
		{ return uwstring{ptr, len}; }
		inline pstring operator""_p(const upchar_t *ptr, size_t len) {
			const char *c_ptr{c_str(ptr)};
			return pstring{c_ptr, c_ptr+len};
		}
	}

	#ifdef __MFW_STD_FILESYSTEM_WIDE_CHAR
	inline void to_string(const pstring &src, uwstring &dst)
	{ dst.assign(uc_str(src), src.native().length()); }
	inline const wchar_t *c_str(const pstring &src)
	{ return reinterpret_cast<const wchar_t *>(src.c_str()); }
	#else
	inline void to_string(const pstring &src, ucstring &dst)
	{ dst.assign(uc_str(src), src.native().length()); }
	inline const char *c_str(const pstring &src)
	{ return reinterpret_cast<const char *>(src.c_str()); }
	#endif

	inline char *c_str(ucstring &src)
	{ return reinterpret_cast<char *>(src.data()); }
	inline wchar_t *c_str(uwstring &src)
	{ return reinterpret_cast<wchar_t *>(src.data()); }

	inline const char *c_str(const ucstring &src)
	{ return reinterpret_cast<const char *>(src.c_str()); }
	inline const wchar_t *c_str(const uwstring &src)
	{ return reinterpret_cast<const wchar_t *>(src.c_str()); }

	inline const upchar_t *uc_str(const pstring &src)
	{ return reinterpret_cast<const upchar_t *>(src.c_str()); }
#else
	#error
#endif
}