#include <public/mfw/stl/format.hpp>

namespace mfw::stl
{
	namespace __format_internal
	{
		template <typename S, typename SV, typename F>
		void format_3(S &buffer, SV fmtstr, F func, va_list args)
		{
			int32_t size{func(nullptr, c_str(fmtstr), args)};
			buffer.resize(static_cast<size_t>(size));
			func(c_str(buffer), c_str(fmtstr), args);
		}

		template <typename S, typename SV, typename F>
		void format_4(S &buffer, SV fmtstr, F func, va_list args)
		{
			int32_t size{func(nullptr, 0, c_str(fmtstr), args)};
			buffer.resize(static_cast<size_t>(size));
			func(c_str(buffer), size+1, c_str(fmtstr), args);
		}
	}

	MFW_STL_API void MFW_STL_CALL format(ucstring &buffer, ucstring_view fmtstr, va_list args)
	{
		__format_internal::format_3(buffer, fmtstr, vsprintf, args);
	}

	MFW_STL_API void MFW_STL_CALL format(uwstring &buffer, uwstring_view fmtstr, va_list args)
	{
		__format_internal::format_4(buffer, fmtstr, vswprintf, args);
	}

	MFW_STL_API bool MFW_STL_CALL format(ucstring &str, ucstring_view fmtstr, const vector<ucstring> &args)
	{
		ucstring_view::const_iterator it{fmtstr.cbegin()};
		size_t arg{0};
		while(it != fmtstr.cend()) {
			bool isfmt{false};
			if(*it == u8'{') {
				isfmt = true;
				if(it != fmtstr.cbegin()) {
					if(*(it - 1) == u8'\\') {
						isfmt = false;
					}
				}
				if(*(it + 1) != u8'}') {
					isfmt = false;
				} else {
					it++;
				}
			}
			if(isfmt) {
				if(arg >= args.size()) {
					return false;
				}
				str += args[arg];
				arg++;
			} else {
				str += *it;
			}
			it++;
		}

		return true;
	}
}