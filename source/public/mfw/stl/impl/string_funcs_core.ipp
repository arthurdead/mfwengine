namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	inline void to_string(const ucstring &src, core::univalue &dst)
	{
		dst.set(src);
	}

	inline void to_string(const pstring &src, core::univalue &dst)
	{
		MFW_MESSAGE("fix this")
		u8npstring str{src.u8string()};
		dst.set_cstr(uc_str(str));
	}
#else
	#error
#endif
}