#ifndef MFW_PUBLIC_CORE_COMMANDLINE_HPP
#define MFW_PUBLIC_CORE_COMMANDLINE_HPP

#pragma once

#include <public/mfw/stl/string.hpp>
#include <public/mfw/core/univalue.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/stl/unordered_map.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/core/core.hpp>
#include <public/mfw/core/serializable.hpp>

namespace mfw::core
{
	class commandline
	{
	public:
		static MFW_CORE_API commandline & MFW_CORE_CALL instance();

		MFW_CORE_API bool MFW_CORE_CALL parse(const ucstring_view &str);

		size_t count() const { return arguments.size(); }

		MFW_CORE_API bool MFW_CORE_CALL has(const ucstring &name) const;
		MFW_CORE_API bool MFW_CORE_CALL has_any(const ucstring &name, const vector<univalue> &vals) const;
		MFW_CORE_API bool MFW_CORE_CALL has_none(const ucstring &name, const vector<univalue> &vals) const;
		MFW_CORE_API bool MFW_CORE_CALL has_all(const ucstring &name, const vector<univalue> &vals) const;
		MFW_CORE_API bool MFW_CORE_CALL has(const ucstring &name, const univalue &val) const;
		bool has(size_t i) const { return count() >= i; }

		MFW_CORE_API const vector<univalue> * MFW_CORE_CALL values(const ucstring &name) const;
		MFW_CORE_API const univalue * MFW_CORE_CALL value(const ucstring &name) const;

		MFW_CORE_API void MFW_CORE_CALL add(const ucstring &name, const vector<univalue> &values);
		MFW_CORE_API void MFW_CORE_CALL add(const ucstring &name, const univalue &value);
		MFW_CORE_API void MFW_CORE_CALL add(const ucstring &name);

		void remove(const ucstring &name) { arguments.erase(name); }
		MFW_CORE_API void MFW_CORE_CALL remove(const ucstring &name, const vector<univalue> &values);
		MFW_CORE_API void MFW_CORE_CALL remove(const ucstring &name, const univalue &value);

		MFW_CORE_API bool MFW_CORE_CALL validate(const ucstring_view &str) const;
		MFW_CORE_API void MFW_CORE_CALL print_help() const;
		
		size_t get_int(const ucstring &name, size_t def=0) const {
			if(!has(name)) {
				return def;
			}

			const univalue *val{value(name)};
			if(!val || val->empty()) {
				return def;
			}
			
			return val->get_int();
		}
		
		bool get_bool(const ucstring &name) const {
			if(!has(name)) {
				return false;
			}

			const univalue *val{value(name)};
			if(!val || val->empty()) {
				return true;
			}
			
			return val->get_bool();
		}

		using arg_map_t = unordered_map<ucstring, vector<univalue>>;
		using arg_vals_t = arg_map_t::value_type::second_type;
		using iterator = arg_map_t::iterator;
		using const_iterator = arg_map_t::const_iterator;

		//const univalue &operator[](const ucstring &str) const { return value(str); }
		//const ucstring &operator[](size_t i) const { return name(i); }

		bool empty() const { return count() == 0; }

		const_iterator cbegin() const { return arguments.cbegin(); }
		const_iterator cend() const { return arguments.cend(); }

		iterator begin() { return arguments.begin(); }
		iterator end() { return arguments.end(); }

		const_iterator begin() const { return cbegin(); }
		const_iterator end() const { return cend(); }

	private:
		arg_map_t arguments{};
		serializable help{};
	};
}

#endif