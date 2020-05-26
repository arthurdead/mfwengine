#include <public/mfw/core/environment.hpp>
#if MFW_OS == MFW_OS_LINUX
	#include <cstdlib>
#endif

namespace mfw::core
{
	MFW_CORE_API void MFW_CORE_CALL environment_var::reset(ucstring_view name_, ucchar_t sep_)
	{
		name = name_;
		sep = sep_;
		values_.clear();

	#if MFW_OS == MFW_OS_LINUX
		char *value{getenv(c_str(name))};
		if(!value) {
			return;
		}

		ucstring tmp{};
		while(true) {
			if(*value == u8'\0') {
				if(!tmp.empty()) {
					values_.emplace_back(move(tmp));
				}
				break;
			} else if(*value == sep) {
				values_.emplace_back(move(tmp));
				tmp.clear();
			} else {
				tmp += *value;
			}
			value++;
		}
	#else
		#error
	#endif
	}

	MFW_CORE_API void MFW_CORE_CALL environment_var::remove(ucstring_view value)
	{
		values_vec_t::iterator it{values_.begin()};
		while(it != values_.end()) {
			if(*it == value) {
				values_.erase(it);
				break;
			}
			it++;
		}
	}

	MFW_CORE_API void MFW_CORE_CALL environment_var::commit()
	{
	#if MFW_OS == MFW_OS_LINUX
		if(!values_.empty()) {
			ucstring value{};
			for(const ucstring &i : values_) {
				value += i;
				value += sep;
			}
			value.pop_back();
			setenv(c_str(name), c_str(value), 1);
		} else {
			unsetenv(c_str(name));
		}
	#else
		#error
	#endif
	}
}