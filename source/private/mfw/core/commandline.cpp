#include <public/mfw/core/commandline.hpp>
#include <public/mfw/core/core.hpp>
#include <private/mfw/core/commandline_parser.hpp>
#include <private/mfw/core/commandline_validator.hpp>
#include <private/mfw/core/commandline_internal.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/core/univalue.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/version.hpp>
#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
	#include <public/mfw/core/logging_interface.hpp>
#endif

namespace mfw::core
{
	MFW_CORE_API commandline & MFW_CORE_CALL commandline::instance()
	{
		return commandline_internal::instance();
	}

	MFW_CORE_API const vector<univalue> * MFW_CORE_CALL commandline::values(const ucstring &name) const
	{
		arg_map_t::const_iterator it{arguments.find(name)};
		if(it != arguments.cend()) {
			return &it->second;
		}

		return nullptr;
	}

	MFW_CORE_API const univalue * MFW_CORE_CALL commandline::value(const ucstring &name) const
	{
		const vector<univalue> *vals{values(name)};
		if(!vals || vals->empty()) {
			return nullptr;
		}

		return &(*vals->cbegin());
	}

	MFW_CORE_API bool MFW_CORE_CALL commandline::parse(const ucstring_view &str)
	{
		bool parsed{commandline_parser::instance().parse(str, *this)};
		return parsed;
	}

	MFW_CORE_API bool MFW_CORE_CALL commandline::has(const ucstring &name) const
	{
		arg_map_t::const_iterator it{arguments.find(name)};
		return (it != arguments.cend());
	}

	MFW_CORE_API bool MFW_CORE_CALL commandline::validate(const ucstring_view &str) const
	{
		commandline_validator validator{};
		bool parsed{validator.parse(*this, str, const_cast<serializable &>(help))};
		return parsed;
	}

	MFW_CORE_API void MFW_CORE_CALL commandline::print_help() const
	{
		commandline_validator::print(help);
	}

	MFW_CORE_API void MFW_CORE_CALL commandline::add(const ucstring &name)
	{
		arg_map_t::iterator it{arguments.find(name)};
		if(it == arguments.end()) {
			arguments.insert_or_assign(name, vector<univalue>{});
		}
	}

	MFW_CORE_API void MFW_CORE_CALL commandline::add(const ucstring &name, const vector<univalue> &vals)
	{
		arg_map_t::iterator it{arguments.find(name)};
		if(it != arguments.end()) {
			for(const univalue &val : vals) {
				if(!contains(it->second, val)) {
					it->second.emplace_back(val);
				}
			}
		} else {
			arguments.insert_or_assign(name, vals);
		}
	}

	MFW_CORE_API void MFW_CORE_CALL commandline::remove(const ucstring &name, const vector<univalue> &vals)
	{
		arg_map_t::iterator it{arguments.find(name)};
		if(it != arguments.end()) {
			arg_vals_t &arg_vals{it->second};
			for(const univalue &val : vals) {
				arg_vals_t::iterator vals_it{arg_vals.begin()};
				while(vals_it != arg_vals.end()) {
					if(*vals_it == val) {
						arg_vals.erase(vals_it);
						break;
					}
					vals_it++;
				}
			}
		}
	}

	MFW_CORE_API bool MFW_CORE_CALL commandline::has_any(const ucstring &name, const vector<univalue> &vals) const
	{
		arg_map_t::const_iterator it{arguments.find(name)};
		if(it != arguments.cend()) {
			bool any{false};
			for(const univalue &val : vals) {
				if(contains(it->second, val)) {
					any = true;
				}
			}
			return any;
		} else {
			return false;
		}
	}

	MFW_CORE_API bool MFW_CORE_CALL commandline::has_none(const ucstring &name, const vector<univalue> &vals) const
	{
		arg_map_t::const_iterator it{arguments.find(name)};
		if(it != arguments.cend()) {
			for(const univalue &val : vals) {
				if(contains(it->second, val)) {
					return false;
				}
			}
			return true;
		} else {
			return false;
		}
	}

	MFW_CORE_API bool MFW_CORE_CALL commandline::has_all(const ucstring &name, const vector<univalue> &vals) const
	{
		arg_map_t::const_iterator it{arguments.find(name)};
		if(it != arguments.cend()) {
			for(const univalue &val : vals) {
				if(!contains(it->second, val)) {
					return false;
				}
			}
			return true;
		} else {
			return false;
		}
	}

	MFW_CORE_API void MFW_CORE_CALL commandline::remove(const ucstring &name, const univalue &val)
	{
		vector<univalue> values{};
		values.emplace_back(val);
		remove(name, move(values));
	}

	MFW_CORE_API void MFW_CORE_CALL commandline::add(const ucstring &name, const univalue &val)
	{
		if(val.empty()) {
			add(name);
			return;
		}
		vector<univalue> values{};
		values.emplace_back(val);
		add(name, move(values));
	}

	MFW_CORE_API bool MFW_CORE_CALL commandline::has(const ucstring &name, const univalue &val) const
	{
		vector<univalue> values{};
		values.emplace_back(val);
		return has_any(name, move(values));
	}
}