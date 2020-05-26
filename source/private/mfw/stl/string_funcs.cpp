#include <public/mfw/stl/string.hpp>

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	namespace __string_funcs_internal
	{
		template <typename S, typename C>
		size_t replace_all_char(S &str, C from, C to)
		{
			size_t count{0};

			replace_if(str.begin(), str.end(), [from, &count](C old) -> bool {
				if(old == from) {
					count++;
					return true;
				}
				return false;
			}, to);

			return count;
		}

		template <typename S, typename V>
		size_t replace_all_view(S &str, V from, V to)
		{
			size_t count{0};

			size_t pos{0};
			while(true) {
				pos = str.find(from, pos);
				if(pos == S::npos) {
					break;
				}

				str.replace(pos, from.length(), to);
				count++;

				pos += to.length();
			}

			return count;
		}
	}

	MFW_STL_API size_t MFW_STL_CALL replace_all(ucstring &str, ucchar_t from, ucchar_t to)
	{
		return __string_funcs_internal::replace_all_char(str, from, to);
	}

	MFW_STL_API size_t MFW_STL_CALL replace_all(uwstring &str, uwchar_t from, uwchar_t to)
	{
		return __string_funcs_internal::replace_all_char(str, from, to);
	}

	MFW_STL_API size_t MFW_STL_CALL replace_all(ucstring &str, ucstring_view from, ucstring_view to)
	{
		return __string_funcs_internal::replace_all_view(str, from, to);
	}

	MFW_STL_API size_t MFW_STL_CALL replace_all(uwstring &str, uwstring_view from, uwstring_view to)
	{
		return __string_funcs_internal::replace_all_view(str, from, to);
	}
#else
	#error
#endif
}