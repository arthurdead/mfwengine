#include <private/mfw/core/accessor_parser.hpp>
#include <public/mfw/core/rttr_interface.hpp>
#include <public/mfw/core/globals.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <private/mfw/core/expression_parser.hpp>
#include <public/mfw/pch_literals.hpp>
#include <public/mfw/core/univalue.hpp>
#include <public/mfw/stl/unordered_map.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/version.hpp>

namespace mfw::core
{
	MFW_DECLARE_LOG_CONTEXT(log_accessor_parser, u8"core/accessor_parser"_p)

	MFW_DECLARE_GLOBAL_ALLOCATOR(accessorparser, accessor_parser)

	accessor_parser &accessor_parser::instance() {
		return __accessorparser_global_allocator.instance();
	}

	void accessor_parser::clear()
	{
		super::clear();

		callbacks_ = nullptr;
		currentvar.clear();
	}

	namespace __accessor_parser_internal
	{
		static void class_as_pointer(const class_info &cls_info, type_holder &holder)
		{
			type_info info{};
			info = cls_info;
			info.make_pointer();
			holder.deduce(info);
		}

		using predefined_macros_t = unordered_map<ucstring_view, size_t>;
		static const predefined_macros_t &predefined_macros() {
			static const predefined_macros_t __predefined_macros{
				{u8"MFW_OS"_sv, MFW_OS},
				{u8"MFW_OS_LINUX"_sv, MFW_OS_LINUX},
				{u8"MFW_OS_WINDOWS"_sv, MFW_OS_WINDOWS},
				
				{u8"MFW_PLATFORM"_sv, MFW_PLATFORM},
				{u8"MFW_PLATFORM_MOBILE"_sv, MFW_PLATFORM_MOBILE},
				{u8"MFW_PLATFORM_DESKTOP"_sv, MFW_PLATFORM_DESKTOP},
				
				{u8"MFW_CONFIGURATION"_sv, MFW_CONFIGURATION},
				{u8"MFW_CONFIGURATION_DEBUG"_sv, MFW_CONFIGURATION_DEBUG},
				{u8"MFW_CONFIGURATION_RELEASE"_sv, MFW_CONFIGURATION_RELEASE},
				
				{u8"MFW_PROCESSOR"_sv, MFW_PROCESSOR},
				{u8"MFW_PROCESSOR_X86_64"_sv, MFW_PROCESSOR_X86_64},
				{u8"MFW_PROCESSOR_X86"_sv, MFW_PROCESSOR_X86},
				{u8"MFW_PROCESSOR_ARM"_sv, MFW_PROCESSOR_ARM},
				{u8"MFW_PROCESSOR_AARCH64"_sv, MFW_PROCESSOR_AARCH64},
				{u8"MFW_PROCESSOR_64BITS_FLAG"_sv, MFW_PROCESSOR_64BITS_FLAG},
				{u8"MFW_PROCESSOR_32BITS_FLAG"_sv, MFW_PROCESSOR_32BITS_FLAG},
				{u8"MFW_PROCESSOR_X86_FLAG"_sv, MFW_PROCESSOR_X86_FLAG},
				{u8"MFW_PROCESSOR_ARM_FLAG"_sv, MFW_PROCESSOR_ARM_FLAG},
			};
			return __predefined_macros;
		}
		MFW_MESSAGE("remove separated later")
	}

	bool accessor_parser::get_internal_function(const ucstring_view &name, const vector<univalue> &args, type_holder &var) const
	{
		if(name == u8"defined"_sv) {
			const univalue &define_val{args[0]};
			const ucstring &define_name{define_val.get_string()};
			if(get_internal_variable(define_name, var)) {
				var.deduce(true);
			} else if(callbacks_->get_variable(define_name, var)) {
				var.deduce(true);
			} else {
				var.deduce(false);
			}
			return true;
		} else if(name == u8"__MFW_BIT"_sv || name == u8"MFW_BIT"_sv) {
			const univalue &bit{args[0]};
			var.deduce(__MFW_BIT(bit.get_int()));
			return true;
		} else if(name == u8"__has_feature"_sv ||
					name == u8"__has_extension"_sv ||
					name == u8"__has_builtin"_sv) {
			var.deduce(false);
			return true;
		}

		return false;
	}

	bool accessor_parser::get_internal_variable(const ucstring_view &name, type_holder &var) const
	{
		if(name == u8"cmdline"_sv) {
			var.deduce(&commandline::instance());
			return true;
		} else {
			const __accessor_parser_internal::predefined_macros_t &map{__accessor_parser_internal::predefined_macros()};
			__accessor_parser_internal::predefined_macros_t::const_iterator it{map.find(name)};
			if(it != map.end()) {
				var.deduce(it->second);
				return true;
			}
		}

		return false;
	}
	
	bool accessor_parser::get_internal_member_variable(const type_holder &obj, const ucstring_view &name, type_holder &var) const
	{
		const ucstring &obj_name{obj.info().name()};
		
		if(obj_name == u8"mfw::core::commandline"_sv) {
			commandline *cmdline{obj.get_var<commandline *>()};
		}
		
		return false;
	}
	
	bool accessor_parser::get_internal_member_function(const type_holder &obj, const ucstring_view &name, const vector<univalue> &args, type_holder &var) const
	{
		size_t num{args.size()};

		const ucstring &obj_name{obj.info().name()};

		if(obj_name == u8"mfw::core::commandline"_sv) {
			commandline *cmdline{obj.get_var<commandline *>()};
			if(name == u8"has"_sv) {
				if(num != 1 && num != 2) {
					error(u8"function takes 1 or 2 args but {} were provided"_sv, num);
					return false;
				}

				if(num == 1) {
					if(args[0].is_int()) {
						var.deduce(cmdline->has(args[0].get_int()));
					} else {
						var.deduce(cmdline->has(args[0].get_string()));
					}
				} else if(num == 2) {
					var.deduce(cmdline->has(args[0].get_string(), args[1]));
				}
				return true;
			} else if(name == u8"empty"_sv) {
				if(num != 0) {
					error(u8"function takes 0 args but {} were provided"_sv, num);
					return false;
				}

				var.deduce(cmdline->empty());
				return true;
			} else if(name == u8"value"_sv) {
				if(num != 1) {
					error(u8"function takes 1 arg but {} were provided"_sv, num);
					return false;
				}

				const ucstring &arg_name{args[0].get_string()};
				const univalue *value{cmdline->value(arg_name)};
				if(!value) {
					if(cmdline->has(arg_name)) {
						var.deduce(true);
					} else {
						var.deduce(false);
					}
				} else {
					if(value->empty()) {
						var.deduce(true);
					} else if(value->is_bool()) {
						var.deduce(value->get_bool());
					} else if(value->is_float()) {
						var.deduce(value->get_float());
					} else if(value->is_int()) {
						var.deduce(value->get_int());
					} else {
						var.deduce(*value);
					}
				}
				return true;
			}
		}

		return false;
	}

	void accessor_parser::parse_main()
	{
		while(true)
		{
			if(finished()) {
				break;
			}

			univalue tmp{};
			if(!expect({token::type::identifier}, tmp)) {
				error(u8"expected identifier but found: {}"_sv, token_().as_string());
				return;
			}

			bool isfunc{false};

			vector<univalue> args{};
			if(tmp == u8"defined"_sv) {
				if(read_function(args, callbacks_)) {
					isfunc = true;
				} else {
					if(!expect({token::type::identifier}, args.emplace_back())) {
						error(u8"expected identifier but found: {}"_sv, token_().as_string());
						return;
					}
					isfunc = true;
				}
			} else {
				isfunc = read_function(args, callbacks_);
			}

			if(isfunc) {
				if(currentvar.valid()) {
					if(!currentvar.info().is_class()) {
						error(u8"primitive cant have function: {}"_sv, tmp);
						return;
					}

					type_holder result{};

					const class_info *clsinfo{currentvar.info().find_class_info()};
					if(clsinfo) {
						const ucstring &func_name{tmp.get_string()};
						const func_info *func{clsinfo->get_function(func_name)};
						if(func) {
							vector<type_holder> funcargs{};

							type_holder &arg0{funcargs.emplace_back()};
							__accessor_parser_internal::class_as_pointer(*clsinfo, arg0);
							arg0.set_var(currentvar.memory());

							MFW_MESSAGE("TODO convert args")

							if(!interfaces::rttr::instance().call_function(*func, funcargs, result)) {
								MFW_DEBUGBREAK();
							}
						}
					}

					if(!result.valid()) {
						const ucstring &func_name{tmp.get_string()};
						if(!get_internal_member_function(currentvar, func_name, args, result)) {
							if(callbacks_) {
								callbacks_->get_member_function(currentvar, func_name, args, result);
							}
						}
					}

					if(!result.valid()) {
						error(u8"unknown member function: {}"_sv, tmp);
						return;
					}

					currentvar = move(result);
				} else {
					type_holder result{};

					const ucstring &func_name{tmp.get_string()};
					if(!get_internal_function(func_name, args, result)) {
						const func_info *func{callbacks_ ? callbacks_->get_function(func_name) : nullptr};
						if(func) {
							vector<type_holder> funcargs{};
							MFW_MESSAGE("TODO convert args")
							if(!interfaces::rttr::instance().call_function(*func, funcargs, result)) {
								MFW_DEBUGBREAK();
							}
						} else {
							if(callbacks_) {
								callbacks_->get_function(func_name, args, result);
							}
						}

						if(!result.valid()) {
							error(u8"unknown global function: {}"_sv, tmp);
							return;
						}
					}

					currentvar = move(result);
				}
			} else {
				if(!currentvar.valid()) {
					type_holder var{};

					const ucstring &var_name{tmp.get_string()};
					if(!get_internal_variable(var_name, var)) {
						if(callbacks_) {
							callbacks_->get_variable(var_name, var);
						}
					}

					if(!var.valid()) {
						MFW_MESSAGE("hmmmmmm")
						//error(u"unknown global variable: {}"_sv, tmp);
						error_ = true;
						return;
					}

					currentvar = move(var);
				} else {
					if(!currentvar.info().is_class()) {
						error(u8"primitive cant have members: {}"_sv, tmp);
						return;
					} else {
						type_holder member{};

						const ucstring &var_name{tmp.get_string()};
						const class_info *clsinfo{currentvar.info().find_class_info()};
						if(clsinfo) {
							const class_info::member_variable_info *varinfo{clsinfo->get_variable(var_name)};
							if(varinfo) {
								varinfo->get(currentvar, member);
							}
						}

						if(!member.valid()) {
							if(!get_internal_member_variable(currentvar, var_name, member)) {
								if(callbacks_) {
									callbacks_->get_member_variable(currentvar, var_name, member);
								}
							}
						}

						if(!member.valid()) {
							error(u8"unknown member variable: {}"_sv, tmp);
							return;
						}

						currentvar = move(member);
					}
				}
			}

			if(finished()) {
				return;
			}

			if(expect({token::type::dot})) {
				if(!currentvar.info().is_class()) {
					error(u8"primitive cant have members"_sv);
					return;
				}
			} else if(read_array(tmp, callbacks_)) {
				if(!currentvar.info().is_class()) {
					error(u8"primitive cant be array"_sv);
					return;
				} else {
					error(u8"arrays not implemented yet"_sv);
					return;
				}
			} else if(read_function(args, callbacks_)) {
				error(u8"return functions not implemented yet"_sv);
				return;
			} else {
				error(u8"expected dot or bracket but found: {}"_sv, token_().as_string());
				return;
			}
		}
	}

	void accessor_parser::error(const ucstring_view &str)
	{
		super::error(str);

		log_accessor_parser().error(str);
	}

	bool accessor_parser::parse(const token_vec_t &tokens, type_holder &result, const interfaces::accessor_parser_callbacks *callback)
	{
		if(tokens.empty()) {
			return false;
		}

		clear();

		callbacks_ = callback;

		bool ret{super::parse(tokens) && currentvar.valid()};
		if(ret) {
			result = currentvar;
		} else {
			result.clear();
		}

		return ret;
	}

	bool accessor_parser::parse(const ucstring_view &str, type_holder &result, const interfaces::accessor_parser_callbacks *callback)
	{
		if(str.empty()) {
			return false;
		}

		clear();

		callbacks_ = callback;

		bool ret{super::parse(str) && currentvar.valid()};
		if(ret) {
			result = currentvar;
		} else {
			result.clear();
		}

		return ret;
	}
}