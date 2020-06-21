#include <private/mfw/core/commandline_validator.hpp>
#include <public/mfw/core/serializable.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/core/univalue.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/pch_literals.hpp>

namespace mfw::core
{
	MFW_DECLARE_LOG_CONTEXT(log_commandline_validator, u8"core/commandline_validator"_p)

	bool commandline_validator::parse(const commandline &cmdline_, const ucstring_view &str, serializable &temp)
	{
		if(str.empty()) {
			return true;
		}

		cmdline = &const_cast<commandline &>(cmdline_);
		if(!temp.from_string(str)) {
			return false;
		}
		
		bool parsed{parse_main(temp)};
		if(!parsed) {
			const vector<univalue> *arg_vals{cmdline->values(u8"help"_s)};
			if(arg_vals) {
				if(!arg_vals->empty()) {
					log_commandline_validator().error(u8"arg help needs no values"_sv);
				} else {
					print(temp);
				}
				return false;
			}
		}
		return parsed;
	}

	bool commandline_validator::get_member_variable(const type_holder &obj, const ucstring_view &name, type_holder &var) const
	{
		const type_info &info{obj.info()};
		if(info.is_exact<univalue>()) {
			const univalue &value{obj.get_var<const univalue>()};
			if(value == name) {
				var.deduce(true);
			} else {
				var.deduce(false);
			}
			return true;
		} else {
			const vector<univalue> *values{obj.get_var<const vector<univalue> *>()};
			if(contains(*values, name)) {
				var.deduce(true);
			} else {
				var.deduce(false);
			}
			return true;
		}
	}

	bool commandline_validator::get_variable(const ucstring_view &name, type_holder &result) const
	{
		const vector<univalue> *values{cmdline->values(ucstring{name})};
		if(!values) {
			return false;
		}

		if(values->size() == 1) {
			result.deduce((*values)[0]);
		} else {
			result.deduce(values);
		}
		return true;
	}

	namespace __commandline_validator_internal
	{
		using build_of_string_t = void(const serializable &, const serializable &, ucstring &, const vector<univalue> *, bool &);

		#define __MFW_BUILD_OF_STRING_PRE \
			for(const serializable &val : of) { \
				const ucstring &val_name{val.get_name()}; \
				if(def.get_child(val_name)) { \
					vals += u8'*'; \
				} \
				vals += val_name; \
				vals += u8", "_sv;

		#define __MFW_BUILD_OF_STRING_POST \
			} \
			vals.erase(vals.cend() - 2, vals.cend());

		static void build_of_string_none(const serializable &of, const serializable &def, ucstring &vals, const vector<univalue> *arg_vals, bool &err)
		{
			__MFW_BUILD_OF_STRING_PRE

				if(arg_vals && (!err && contains(*arg_vals, val_name))) {
					err = true;
				}

			__MFW_BUILD_OF_STRING_POST
		}

		static void build_of_string_all(const serializable &of, const serializable &def, ucstring &vals, const vector<univalue> *arg_vals, bool &err)
		{
			if(!arg_vals) {
				err = true;
			}

			__MFW_BUILD_OF_STRING_PRE

				if(arg_vals && (!err && !contains(*arg_vals, val_name))) {
					err = true;
				}

			__MFW_BUILD_OF_STRING_POST
		}

		static void build_of_string_any(const serializable &of, const serializable &def, ucstring &vals, const vector<univalue> *arg_vals, bool &err)
		{
			bool any{false};

			__MFW_BUILD_OF_STRING_PRE

				if(arg_vals && (!any && contains(*arg_vals, val_name))) {
					any = true;
				}

			__MFW_BUILD_OF_STRING_POST

			if(arg_vals && (!any)) {
				err = true;
			}
		}

		static bool do_of(const serializable &of, const serializable &def, const vector<univalue> *arg_vals, const ucstring &name, const ucstring_view &fmt, build_of_string_t func)
		{
			bool err{false};

			ucstring vals{};
			func(of, def, vals, arg_vals, err);

			if(err) {
				log_commandline_validator().error(fmt, name, vals);
				return false;
			}

			return true;
		}

		static void print_of(const serializable &flags, const serializable &def, const ucstring_view &name, ucstring &fmt)
		{
			const serializable *of{flags.get_child(name)};
			if(of) {
				fmt += name;
				fmt += u8'(';
				for(const serializable &val : *of) {
					const ucstring &val_name{val.get_name()};
					if(def.get_child(val_name)) {
						fmt += u8'*';
					}
					fmt += val_name;
					fmt += u8", "_sv;
				}
				fmt.erase(fmt.cend() - 2, fmt.cend());
				fmt += u8"), "_sv;
			}
		}
	}

	void commandline_validator::print_internal(const serializable &ser)
	{
		ucstring fmt{};

		const serializable *flags{ser.get_flags()};
		if(flags) {
			fmt += u8'[';

			if(flags->get_child(u8"required"_sv)) {
				fmt += u8"required, "_sv;
			} else if(flags->get_child(u8"optional"_sv)) {
				fmt += u8"optional, "_sv;
			}

			__commandline_validator_internal::print_of(*flags, ser, u8"any_of"_sv, fmt);
			__commandline_validator_internal::print_of(*flags, ser, u8"all_of"_sv, fmt);
			__commandline_validator_internal::print_of(*flags, ser, u8"none_of"_sv, fmt);

			const serializable *count{flags->get_child(u8"count"_sv)};
			const serializable *min{flags->get_child(u8"min"_sv)};
			const serializable *max{flags->get_child(u8"max"_sv)};
			if(count || min || max) {
				fmt += u8"count "_sv;
				if(count) {
					const univalue &count_val{count->get_value()};
					const ucstring &count_str{count_val.get_string()};
					fmt += u8"= "_sv;
					fmt += count_str;
				} else {
					if(min) {
						const univalue &min_val{min->get_value()};
						const ucstring &min_str{min_val.get_string()};
						fmt += u8">= "_sv;
						fmt += min_str;
						if(max) {
							fmt += u8"&& "_sv;
						}
					}
					if(max) {
						const univalue &max_val{max->get_value()};
						const ucstring &max_str{max_val.get_string()};
						fmt += u8"<= "_sv;
						fmt += max_str;
					}
				}
				//fmt += u8", "_sv;
			} else {
				fmt.erase(fmt.cend() - 2, fmt.cend());
			}

			fmt += u8"]\n";
		}
		
		log_commandline_validator().info(fmt);

		fmt = ser.get_name();

		if(!ser.empty()) {
			fmt += u8" = "_sv;
			for(const serializable &val : ser) {
				fmt += val.get_name();
				fmt += u8", "_sv;
			}
			fmt.erase(fmt.cend() - 2, fmt.cend());
		}

		fmt += u8'\n';

		if(flags) {
			const serializable *description{flags->get_child(u8"description"_sv)};
			if(description) {
				const univalue &desc_val{description->get_value()};
				const ucstring &desc_str{desc_val.get_string()};
				fmt += u8'"';
				fmt += desc_str;
				fmt += u8"\"\n";
			}
		}

		fmt += u8'\n';

		log_commandline_validator().add_ident();
		log_commandline_validator().info(fmt);
		log_commandline_validator().remove_ident();
	}

	void commandline_validator::print(const serializable &ser)
	{
		unordered_map<ucstring, vector<const serializable *>> later{};
		for(const serializable &child : ser) {
			const ucstring &condition{child.get_condition()};
			if(!condition.empty()) {
				later[condition].emplace_back(&child);
				continue;
			}

			print_internal(child);
		}

		for(const pair<ucstring, vector<const serializable *>> &it : later) {
			log_commandline_validator().info(u8"if {}:"_sv, it.first);
			log_commandline_validator().add_ident();
			for(const serializable *child : it.second) {
				print_internal(*child);
			}
			log_commandline_validator().remove_ident();
		}
	}

	bool commandline_validator::parse_main(const serializable &ser) const
	{
		/*if(ser.has_child(u8"help"_sv)) {
			const vector<univalue> *arg_vals{cmdline->values(u8"help"_s)};
			if(arg_vals) {
				if(!arg_vals->empty()) {
					log_commandline_validator().error(u8"arg help needs no values"_sv);
				} else {
					print(ser);
				}
				return false;
			}
		}*/
		
		for(const serializable &child : ser) {
			const ucstring &name{child.get_name()};

			if(!child.passes_condition(this)) {
				log_commandline_validator().error(u8"arg {} does not pass condition [{}]"_sv, name, child.get_condition());
				return false;
			}

			bool optional{false};
			const serializable *flags{child.get_flags()};
			if(flags && flags->get_child(u8"optional"_sv)) {
				optional = true;
			}
			if(!optional) {
				if(!child.empty()) {
					vector<univalue> vals{};
					for(const serializable &def_value : child) {
						vals.emplace_back(def_value.get_name());
					}
					cmdline->add(name, move(vals));
				} else {
					const univalue &value{child.get_value()};
					if(!value.empty()) {
						cmdline->add(name, value);
					} else {
						cmdline->add(name);
					}
				}
			}

			if(flags) {
				bool has_arg{cmdline->has(name)};
				if(!has_arg && optional) {
					continue;
				}
				if(flags->get_child(u8"required"_sv)) {
					if(!has_arg) {
						log_commandline_validator().error(u8"missing arg {}"_sv, name);
						return false;
					}
				}
				const vector<univalue> *arg_vals{cmdline->values(name)};
				const serializable *all_of{flags->get_child(u8"all_of"_sv)};
				if(all_of) {
					if(!arg_vals) {
						log_commandline_validator().error(u8"arg {} missing values"_sv, name);
						return false;
					}
				}
				size_t nargs{0};
				if(arg_vals) {
					nargs = arg_vals->size();
				}
				#define __MFW_COUNT_CODE_START(var, op, extra) \
					if(var) { \
						size_t var##_val{static_cast<size_t>(var->get_value().get_int())};
				#define __MFW_COUNT_CODE_MAX(var, op, extra) \
					__MFW_COUNT_CODE_START(var, op, extra) \
						if(var##_val == 0) { \
							if(arg_vals && !arg_vals->empty()) { \
								log_commandline_validator().error(u8"arg {} needs no values"_sv, name); \
								return false; \
							} \
						} else if(var##_val == 1) { \
							if(nargs op 1) { \
								log_commandline_validator().error(u8"arg {} needs 1 value {}"_sv, name, extra); \
								return false; \
							} \
						} else { \
							if(var##_val op nargs) { \
								log_commandline_validator().error(u8"arg {} needs {} values {}"_sv, name, var##_val, extra); \
								return false; \
							} \
						} \
					}
				#define __MFW_COUNT_CODE_MIN(var, op, extra) \
					__MFW_COUNT_CODE_START(var, op, extra) \
						if(var##_val == 1) { \
							if(nargs op 1) { \
								log_commandline_validator().error(u8"arg {} needs 1 value {}"_sv, name, extra); \
								return false; \
							} \
						} else { \
							if(var##_val op nargs) { \
								log_commandline_validator().error(u8"arg {} needs {} values"_sv, name, var##_val, extra); \
								return false; \
							} \
						} \
					}
				const serializable *count{flags->get_child(u8"count"_sv)};
				__MFW_COUNT_CODE_MAX(count, !=, u8""_sv)
				const serializable *min{flags->get_child(u8"min"_sv)};
				__MFW_COUNT_CODE_MIN(min, <, u8"or more"_sv)
				const serializable *max{flags->get_child(u8"max"_sv)};
				__MFW_COUNT_CODE_MAX(max, >, u8"or less"_sv)
				const serializable *any_of{flags->get_child(u8"any_of"_sv)};
				if(any_of) {
					if(!__commandline_validator_internal::do_of(*any_of, child, arg_vals, name, u8"arg {} values needs to contain any of [{}]"_sv, __commandline_validator_internal::build_of_string_any)) {
						return false;
					}
				}
				const serializable *none_of{flags->get_child(u8"none_of"_sv)};
				if(none_of) {
					if(!__commandline_validator_internal::do_of(*none_of, child, arg_vals, name, u8"arg {} values needs to contain none of [{}]"_sv, __commandline_validator_internal::build_of_string_none)) {
						return false;
					}
				}
				if(all_of) {
					if(!__commandline_validator_internal::do_of(*all_of, child, arg_vals, name, u8"arg {} values needs to contain all of [{}]"_sv, __commandline_validator_internal::build_of_string_all)) {
						return false;
					}
				}
			}
		}
		return true;
	}
}