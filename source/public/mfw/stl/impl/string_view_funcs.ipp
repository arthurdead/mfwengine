namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	namespace literals
	{
		constexpr inline ucstring_view operator""_sv(const ucchar_t *ptr, size_t len)
		{ return {ptr, len}; }
		constexpr inline uwstring_view operator""_sv(const uwchar_t *ptr, size_t len)
		{ return {ptr, len}; }
	}

	inline const char *c_str(ucstring_view src)
	{ return reinterpret_cast<const char *>(src.data()); }
	inline const wchar_t *c_str(uwstring_view src)
	{ return reinterpret_cast<const wchar_t *>(src.data()); }

	namespace __string_view_funcs_internal
	{
		template <typename S, typename C>
		inline bool to_bool(S src, C strtrue[4], C strfalse[5], bool &dst)
		{
			if(src.empty()) {
				dst = false;
				return false;
			}
			
			if(src.compare(0, S::npos, strtrue, 4) == 0) {
				dst = true;
				return true;
			} else if(src.compare(0, S::npos, strfalse, 5) == 0) {
				dst = false;
				return true;
			} else {
				uint8_t i{0};
				if(!to_int(src, i)) {
					dst = false;
					return false;
				}
				dst = (i > 0);
				return true;
			}
		}
	}

	inline bool to_bool(ucstring_view src, bool &dst)
	{
		return __string_view_funcs_internal::to_bool(src, STR_C("true"), STR_C("false"), dst);
	}

	inline bool to_bool(uwstring_view src, bool &dst)
	{
		return __string_view_funcs_internal::to_bool(src, STR_W("true"), STR_W("false"), dst);
	}
#else
	#error
#endif
}