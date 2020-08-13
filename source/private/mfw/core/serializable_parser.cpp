#include <private/mfw/core/serializable_parser.hpp>
#include <public/mfw/core/globals.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <public/mfw/core/filesystem_interface.hpp>
#include <public/mfw/core/commandline.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/format.hpp>
#include <public/mfw/stl/memory.hpp>
#include <public/mfw/pch_literals.hpp>
#include <private/mfw/core/expression_parser.hpp>

namespace mfw::core
{
	MFW_DECLARE_LOG_CONTEXT(log_serializable_parser, u8"core/serializable_parser"_p)

	MFW_DECLARE_GLOBAL_ALLOCATOR(serializableparser, serializable_parser)

	serializable_parser &serializable_parser::instance() {
		return __serializableparser_global_allocator.instance();
	}

	void serializable_parser::clear()
	{
		super::clear();

		callbacks = nullptr;
		defines.clear();
		parents.clear();
		if_depth = 0;
		filename_.clear();
	}
	
	void serializable_parser::parse_setup(serializable &root, const interfaces::serializable_parser_callbacks *callbacks_)
	{
		pstring old_filename{filename_};
		
		clear();
		
		parent_context &context{emplace_parent()};
		context.parents.emplace_back(&root);
		context.open_type = token::extended_type::brace_left;

		callbacks = callbacks_;
		
		if(!old_filename.empty()) {
			filename_ = old_filename;
		} else {
			const ucstring &name{root.get_name()};
			if(!name.empty() && name != u8"inline"_sv) {
				filename_ = as_string<pstring>(name);
			}
		}
	}

	bool serializable_parser::parse(const ucstring_view &str, serializable &root, const interfaces::serializable_parser_callbacks *callbacks_)
	{
		if(str.empty()) {
			return false;
		}

		parse_setup(root, callbacks_);

		ignore_flags flags{ignore_flags::default_};
		flags &= ~ignore_flags::newline;
		flags |= ignore_flags::divide;
		flags |= ignore_flags::slash;
		//flags |= ignore_flags::dot;
		//flags |= ignore_flags::colon;

		return super::parse(str, flags);
	}

	bool serializable_parser::parse(const token_vec_t &tokens, serializable &root, const interfaces::serializable_parser_callbacks *callbacks_)
	{
		if(tokens.empty()) {
			return false;
		}

		parse_setup(root, callbacks_);

		return super::parse(tokens);
	}

	void serializable_parser::parse_main()
	{
		while(true)
		{
			if(finished()) {
				if(!error_) {
					if(parents.size() != 1) {
						error(u8"childs never closed"_sv);
						return;
					} else if(!static_if_contexts.empty() || !dynamic_if_contexts.empty()) {
						error(u8"ifs never closed"_sv);
						return;
					}
				}
				filename_.clear();
				break;
			}

			parse_preprocessor();

			if(!is_skipping()) {
				parse_dynamicprocessor();
				parse_keys();
			} else {
				advance();
			}
		}
	}
	
	void serializable_parser::error(const ucstring_view &err, const token &tok)
	{
		pstring file{filename()};

		ucstring tmp{};
		format(tmp, err, tok);

		ucstring str{};
		format(str, u8"{}:{}:{}: {}"_sv, file, tok.line, tok.offset, tmp);

		super::error(str);

		log_serializable_parser().error(str);
	}

	void serializable_parser::error(const ucstring_view &err)
	{
		pstring file{filename()};

		ucstring str{};
		format(str, u8"{}: {}"_sv, file, err);

		super::error(str);

		log_serializable_parser().error(str);
	}

	serializable_parser::parent_context &serializable_parser::top()
	{
		return *(parents.back());
	}

	serializable_parser::parent_context &serializable_parser::parent()
	{
		return *(*(parents.end()-2));
	}

	serializable_parser::parent_context &serializable_parser::root()
	{
		return *(parents.front());
	}

	serializable_parser::parent_context &serializable_parser::emplace_parent()
	{
		unique_ptr<parent_context> &ptr{parents.emplace_back()};
		ptr.reset(new parent_context{});
		return *ptr;
	}

	void serializable_parser::pop_parent()
	{
		parents.erase(parents.cend()-1);
	}

	pstring serializable_parser::filename() const
	{
		return filename_;
	}

	void serializable_parser::dynamic_if_context::invert()
	{
		invert_expression(check, check);
	}

	void serializable_parser::dynamic_if_context::append(const ucstring &check_)
	{
		append_expression(check, check_, check);
	}

	void serializable_parser::static_if_context::invert()
	{
		expression_parser::invert(check);
	}

	void serializable_parser::static_if_context::append(const token_vec_t &check_)
	{
		expression_parser::append(check, check_);
	}

	void serializable_parser::parse_dynamicprocessor()
	{
		if(finished()) {
			return;
		}

		skip_newlines();

		if(expect({token::type::dollar})) {
			if(expect({token::extended_type::parenthesis_left})) {
				advance(-2);
				return;
			} else if(expect({token::type::identifier, u8"if"_uv})) {
				ucstring line{};
				if(!read_line(line)) {
					error(u8"failed to read line"_sv);
					return;
				}

				ucstring top_check{};
				if(!dynamic_if_contexts.empty()) {
					const dynamic_if_context &top_context{dynamic_if_contexts.top()};
					top_check = top_context.check;
				}

				dynamic_if_context &context{dynamic_if_contexts.emplace()};
				context.check = top_check;
				context.append(line);
			} else if(expect({token::type::identifier, u8"elif"_uv})) {
				if(dynamic_if_contexts.empty()) {
					error(u8"elif without if"_sv);
					return;
				}

				ucstring line{};
				if(!read_line(line)) {
					error(u8"failed to read line"_sv);
					return;
				}

				dynamic_if_context &context{dynamic_if_contexts.top()};
				if(context.last) {
					error(u8"elif after else"_sv);
					return;
				}

				context.invert();
				context.append(line);
			} else if(expect({token::type::identifier, u8"endif"_uv})) {
				if(dynamic_if_contexts.empty()) {
					error(u8"endif without if"_sv);
					return;
				}

				dynamic_if_contexts.pop();
			} else if(expect({token::type::identifier, u8"else"_uv})) {
				if(dynamic_if_contexts.empty()) {
					error(u8"else without if"_sv);
					return;
				}

				dynamic_if_context &context{dynamic_if_contexts.top()};
				if(context.last) {
					error(u8"else after else"_sv);
					return;
				}

				context.last = true;
				context.invert();
			} else {
				error(u8"unknown dynamicprocessor directive: {}"_sv, token_());
				return;
			}
		}

		skip_newlines();

		const token &tok{token_()};
		if(tok.type_ == token::type::hashtag) {
			parse_preprocessor();
		} else if(tok.type_ == token::type::dollar) {
			parse_dynamicprocessor();
		}
	}

	bool serializable_parser::is_skipping() const
	{
		if(static_if_contexts.empty()) {
			return false;
		}

		const static_if_context &context{static_if_contexts.top()};
		return (context.skipping || !context.success);
	}

	bool serializable_parser::handle_include(bool optional, bool root, univalue &tmp)
	{
		if(!expect({token::type::identifier}, tmp)) {
			error(u8"expected identifier but found: {}"_sv, token_());
			return false;
		}

		pstring file{filename()};
		if(file.empty()) {
			error(u8"#include needs to be inside file"_sv);
			return false;
		}
		
		pstring value_file{as_string<pstring>(tmp)};
		if(file != u8"inline"_sv) {
			file.remove_filename();
			file /= move(value_file);
		} else {
			file = move(value_file);
		}

		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		file = filesys.resolve({file});
		if(file.empty()) {
			if(!optional) {
				error(u8"include file not found: {}"_sv, tmp);
				return false;
			} else {
				return true;
			}
		}
		if(file == filename()) {
			error(u8"include file included itself: {}"_sv, file);
			return false;
		}

		ucstring str{};
		filesys.open_text_file({file}, str);

		parent_context &context{top()};
		
		serializable_parser subparser{};
		subparser.filename_ = file;

		if(!root) {
			for(serializable *child : context.parents) {
				if(!subparser.parse(str, *child, callbacks)) {
					error(u8"failed to parse include: {}"_sv, file);
					return false;
				}
			}
		} else {
			serializable *child{*context.parents.begin()};
			if(!subparser.parse(str, *child, callbacks)) {
				error(u8"failed to parse include: {}"_sv, file);
				return false;
			}
		}

		return true;
	}

	void serializable_parser::parse_preprocessor()
	{
		if(finished()) {
			return;
		}

		skip_newlines();

		if(expect({token::type::hashtag})) {
			univalue tmp{};

			bool valid1{true};
			if(!is_skipping()) {
				if(expect({token::type::identifier, u8"pragma"_uv})) {
					univalue name{};
					if(!expect({token::type::identifier}, name)) {
						error(u8"expected identifier but found: {}"_sv, token_());
						return;
					}

					vector<univalue> args{};
					if(!read_function(args, this)) {
						if(expect({token::type::identifier}, tmp)) {
							args.emplace_back(move(tmp));
						} else {
							error(u8"expected function or identifier but found: {}"_sv, token_());
							return;
						}
					}

					if(name == u8"message"_sv) {
						pstring file{filename()};

						for(const univalue &it : args) {
							log_serializable_parser().info(u8"{}: {}"_sv, file, it);
						}
					} else if(name == u8"warning"_sv) {
						pstring file{filename()};

						for(const univalue &it : args) {
							log_serializable_parser().warning(u8"{}: {}"_sv, file, it);
						}
					} else if(name == u8"insert"_sv) {
						for(const univalue &it : args) {
							const serializable *insert{get_inherit(it)};
							if(!insert) {
								error(u8"unknown insert: {}"_sv, it);
								return;
							}

							parent_context &context{top()};

							for(serializable *parent : context.parents) {
								parent->merge(*insert);
							}
						}
					} else {
						error(u8"unknown pragma: {}"_sv, name);
						return;
					}
				} else if(expect({token::type::identifier, u8"error"_uv})) {
					read_line(tmp);

					const ucstring &err_str{tmp.get_string()};
					error(err_str);
					return;
				} else if(expect({token::type::identifier, u8"endinput"_uv})) {
					iterators_[iterators::current] = iterators_[iterators::end];
					while(!static_if_contexts.empty()) {
						static_if_contexts.pop();
					}
					while(!dynamic_if_contexts.empty()) {
						dynamic_if_contexts.pop();
					}
					return;
				} else if(expect({token::type::identifier, u8"define"_uv})) {
					if(!expect({token::type::identifier}, tmp)) {
						error(u8"expected identifier but found: {}"_sv, token_());
						return;
					}

					const ucstring &def_name{tmp.get_string()};
					if(defines.find(def_name) != defines.end()) {
						error(u8"define already defined: {}"_sv, tmp);
						return;
					}

					univalue value{};
					expect({token::type::identifier}, value);

					replace_defines(value);

					defines.insert_or_assign(def_name, value);
				} else if(expect({token::type::identifier, u8"undef"_uv})) {
					if(!expect({token::type::identifier}, tmp)) {
						error(u8"expected identifier but found: {}"_sv, token_());
						return;
					}

					const ucstring &def_name{tmp.get_string()};
					if(defines.find(def_name) == defines.end()) {
						error(u8"define not defined: {}"_sv, tmp);
						return;
					}

					defines.erase(def_name);
				} else if(expect({token::type::identifier, u8"include"_uv})) {
					if(!handle_include(false, false, tmp)) {
						return;
					}
				} else if(expect({token::type::identifier, u8"try_include"_uv})) {
					if(!handle_include(true, false, tmp)) {
						return;
					}
				} else if(expect({token::type::identifier, u8"root_include"_uv})) {
					if(!handle_include(false, true, tmp)) {
						return;
					}
				} else if(expect({token::type::identifier, u8"try_root_include"_uv})) {
					if(!handle_include(true, true, tmp)) {
						return;
					}
				} else {
					valid1 = false;
				}
			}

			bool valid2{true};
			if(expect({token::type::identifier, u8"ifdef"_uv})) {
				if(is_skipping()) {
					if_depth++;
					return;
				}

				if(!expect({token::type::identifier}, tmp)) {
					error(u8"expected identifier but found: {}"_sv, token_());
					return;
				}

				static_if_context &context{static_if_contexts.emplace()};
				//context.check = tmp;

				const ucstring &def_name{tmp.get_string()};
				if(defines.find(def_name) != defines.end()) {
					context.success = true;
				} else {
					context.success = false;
				}
			} else if(expect({token::type::identifier, u8"ifndef"_uv})) {
				if(is_skipping()) {
					if_depth++;
					return;
				}

				if(!expect({token::type::identifier}, tmp)) {
					error(u8"expected identifier but found: {}"_sv, token_());
					return;
				}

				static_if_context &context{static_if_contexts.emplace()};
				//context.check = tmp;

				const ucstring &def_name{tmp.get_string()};
				if(defines.find(def_name) == defines.end()) {
					context.success = true;
				} else {
					context.success = false;
				}
			} else if(expect({token::type::identifier, u8"if"_uv})) {
				if(is_skipping()) {
					if_depth++;
					return;
				}

				token_vec_t subtokens{};
				if(!read_line(subtokens)) {
					error(u8"failed to read line"_sv);
					return;
				}

				replace_defines(subtokens);

				static_if_context &context{static_if_contexts.emplace()};
				context.check = move(subtokens);

				univalue result{};
				if(!expression_parser::instance().parse(context.check, result, this)) {
					error(u8"failed to parse if: {}"_sv, as_string<ucstring>(context.check));
					return;
				}

				if(result) {
					context.success = true;
				} else {
					context.success = false;
				}
			} else if(expect({token::type::identifier, u8"elif"_uv})) {
				if(is_skipping() && if_depth > 0) {
					return;
				}

				if(static_if_contexts.empty()) {
					error(u8"elif without if"_sv);
					return;
				}

				static_if_context &context{static_if_contexts.top()};
				if(context.last) {
					error(u8"elif after else"_sv);
					return;
				}

				if(context.success) {
					context.skipping = true;
					return;
				}

				token_vec_t subtokens{};
				if(!read_line(subtokens)) {
					error(u8"failed to read line"_sv);
					return;
				}

				replace_defines(subtokens);

				context.invert();
				context.append(subtokens);

				univalue result{};
				if(!expression_parser::instance().parse(context.check, result, this)) {
					error(u8"failed to parse elif: {}"_sv, as_string<ucstring>(context.check));
					return;
				}

				if(result) {
					context.skipping = false;
					context.success = true;
				} else {
					context.success = false;
				}
			} else if(expect({token::type::identifier, u8"endif"_uv})) {
				if(is_skipping() && if_depth > 0) {
					if_depth--;
					return;
				}

				if(static_if_contexts.empty()) {
					error(u8"endif without if"_sv);
					return;
				}

				static_if_contexts.pop();
			} else if(expect({token::type::identifier, u8"else"_uv})) {
				if(is_skipping() && if_depth > 0) {
					return;
				}

				if(static_if_contexts.empty()) {
					error(u8"else without if"_sv);
					return;
				}

				static_if_context &context{static_if_contexts.top()};
				if(context.last) {
					error(u8"else after else"_sv);
					return;
				}

				if(context.success) {
					context.skipping = true;
					return;
				}

				context.last = true;
				context.invert();

				univalue result{};
				if(!expression_parser::instance().parse(context.check, result, this)) {
					error(u8"failed to parse else: {}"_sv, as_string<ucstring>(context.check));
					return;
				}

				if(result) {
					context.skipping = false;
					context.success = true;
				} else {
					context.success = false;
				}
			} else {
				valid2 = false;
			}

			if(!valid1 && !valid2) {
				error(u8"unknown preprocessor directive: {}"_sv, token_());
				return;
			}
		}

		skip_newlines();

		const token &tok{token_()};
		if(tok.type_ == token::type::hashtag) {
			parse_preprocessor();
		} else if(tok.type_ == token::type::dollar) {
			parse_dynamicprocessor();
		}
	}

	bool serializable_parser::get_variable(const ucstring_view &name, type_holder &var) const
	{
		if(name == u8"root"_sv) {
			const parent_context &ctx{top()};
			const serializable *inherit{ctx.parents.back()};
			var.deduce(inherit->root());
			return true;
		} else if(name == u8"file_root"_sv) {
			const parent_context &ctx{*parents[0]};
			const serializable *inherit{ctx.parents[0]};
			var.deduce(inherit);
			return true;
		} else if(name == u8"parent"_sv) {
			MFW_DEBUGBREAK();
		} else if(name == u8"value"_sv) {
			MFW_DEBUGBREAK();
		} else if(name == u8"this"_sv) {
			MFW_DEBUGBREAK();
		} else {
			const serializable *inherit{get_inherit(name)};
			if(inherit) {
				var.deduce(inherit);
				return true;
			}
		}

		return false;
	}

	bool serializable_parser::get_member_variable(const type_holder &obj, const ucstring_view &name, type_holder &var) const
	{
		const ucstring &obj_name{obj.info().name()};

		if(obj_name == u8"mfw::core::serializable"_sv) {
			const serializable *inherit{obj.get_var<const serializable *>()};
			if(name == u8"root"_sv) {
				MFW_DEBUGBREAK();
			} else if(name == u8"parent"_sv) {
				var.deduce(inherit->parent());
				return true;
			} else if(name == u8"value"_sv) {
				var.deduce(&inherit->get_value());
				return true;
			} else if(name == u8"this"_sv) {
				MFW_DEBUGBREAK();
			} else {
				inherit = inherit->get_child(name);
				if(inherit) {
					var.deduce(inherit);
					return true;
				}
			}
		}

		return false;
	}

	bool serializable_parser::get_function(const ucstring_view &name, const vector<univalue> &args, type_holder &var) const
	{
		size_t num{args.size()};
		if(num != 1) {
			error(u8"function takes 1 arg but {} were provided"_sv, num);
			return false;
		}

		if(name == u8"exists"_sv || name == u8"key_exists"_sv) {
			const univalue &arg{args[0]};
			const serializable *inherit{get_inherit(arg)};
			var.deduce(inherit != nullptr);
			return true;
		} else if(name == u8"file_exists"_sv || name == u8"__has_include"_sv) {
			interfaces::filesystem &filesys{interfaces::filesystem::instance()};
			const univalue &arg{args[0]};
			pstring path{as_string<pstring>(arg.get_string())};
			var.deduce(filesys.exists(path));
			return true;
		}

		return false;
	}

	const serializable *serializable_parser::get_inherit(const univalue &name) const
	{
		const type_holder &var{name.get_var()};
		if(var.valid()) {
			return var.get_var<const serializable *>();
		}

		return get_inherit(name.get_string_view());
	}

	const serializable *serializable_parser::get_inherit(const ucstring_view &name) const
	{
		const parent_context &context{top()};

		const serializable *inherit{nullptr};
		if(callbacks) {
			inherit = callbacks->get_inherit(name);
		}

		for(const serializable *child : context.parents) {
			if(!inherit) {
				const serializable *parent{child->parent()};
				if(parent) {
					inherit = parent->get_child(name);
				}
			}
			if(!inherit) {
				inherit = child->get_child(name);
			}
			if(inherit) {
				break;
			}
		}

		return inherit;
	}

	bool serializable_parser::parse_key(parent_context &context, const vector<serializable *> &childs, univalue &tmp)
	{
		if(context.tmpdata.grouping && !context.tmpdata.namespace_) {
			for(serializable *child : childs) {
				context.tmpdata.group.emplace_back(child);
			}
		}

		if(!dynamic_if_contexts.empty()) {
			dynamic_if_context &if_context{dynamic_if_contexts.top()};
			for(serializable *child : childs) {
				child->set_condition(if_context.check);
			}
		}
		
		bool was_newline{false};

		if(expect({token::type::colon})) {
			while(true) {
				if(finished()) {
					break;
				}

				if(!read_identifier(tmp, this)) {
					error(u8"expected identifier but found: {}"_sv, token_());
					return false;
				}

				const serializable *inherit{nullptr};
				const type_holder &var{tmp.get_var()};
				if(var.valid()) {
					inherit = var.get_var<const serializable *>();
				} else {
					replace_defines(tmp);
					inherit = get_inherit(tmp);
				}
				if(!inherit) {
					error(u8"unknown inheritence: {}"_sv, tmp);
					return false;
				}

				for(serializable *child : childs) {
					child->merge(*inherit);
				}

				if(expect({token::type::comma})) {

				} else if(expect({token::type::semicolon})) {
					break;
				} else if(expect({token::type::newline})) {
					was_newline = true;
					break;
				} else {
					const token &tok{token_()};
					token::extended_type extype{tok.get_extended_type()};
					if(extype == token::extended_type::brace_left ||
						extype == token::extended_type::bracket_left) {
						break;
					}
					error(u8"expected comma or newline or semicolon or key open but found: {}"_sv, token_());
					return false;
				}
			}
		}
		
		if(was_newline) {
			advance(-1);
		}

		parse_key_end(context, &childs, tmp, true);

		return true;
	}

	void serializable_parser::replace_defines(ucstring &str) const
	{
		size_t count{0};

		for(const pair<ucstring, univalue> &it : defines) {
			if(!it.second.empty()) {
				count += replace_all(str, it.first, it.second.get_string());
			}
		}

		if(count > 0) {
			replace_defines(str);
		}
	}

	void serializable_parser::replace_defines(univalue &tmp) const
	{
		ucstring str{tmp.get_string()};
		replace_defines(str);
		tmp = str;
	}

	void serializable_parser::replace_defines(token_vec_t &tmp) const
	{
		for(token &tok : tmp) {
			replace_defines(tok.value);
		}
	}

	void serializable_parser::parse_keys_header()
	{
		if(finished()) {
			return;
		}

		univalue tmp{};
		parent_context &context{top()};

		if(context.force == parent_context::force_type::remove) {
			error(u8"group removing is not implemented"_sv);
			return;
		}

		if(parse_key_close(context, true)) {
			return;
		} else if(expect({token::extended_type::brace_left}) ||
				expect({token::extended_type::bracket_left}) ||
				expect({token::extended_type::lesser}) ||
				expect({token::extended_type::parenthesis_left})) {
			error(u8"unexpected opening: {}"_sv, peek(-1));
			return;
		}

		skip_newlines();

		if(finished()) {
			return;
		}

		serializable flags{};
		bool hasflags{false};
		if(expect({token::type::dollar})) {
			vector<token> tokens{};
			if(read({token::extended_type::parenthesis_left}, {token::extended_type::parenthesis_right}, tokens)) {
				skip_newlines();
				serializable_parser subparser{};
				if(!subparser.parse(tokens, flags)) {
					error(u8"failed to parse flags"_sv);
					return;
				} else {
					hasflags = !flags.empty();
				}
			} else {
				error(u8"expected flags"_sv);
				return;
			}
		}

		if(expect({token::type::bit_not})) {
			error(u8"inherit expand are not implemented"_sv);
			return;
		}

		bool force_create{false};
		if(expect({token::type::bit_and})) {
			if(context.force == parent_context::force_type::remove) {
				error(u8"cant force create while removing"_sv);
				return;
			}
			force_create = true;
		}

		if(!expect({token::type::identifier}, tmp)) {
		//if(!read_identifier(tmp)) {
			error(u8"expected identifier but found: {}"_sv, token_());
			return;
		}

		replace_defines(tmp);

		vector<serializable *> *parents_{nullptr};
		if(context.tmpdata.namespace_) {
			parents_ = &context.tmpdata.parents;
		} else {
			parents_ = &context.parents;
		}
		if(!parents_ || parents_->empty()) {
			error(u8"no parents"_sv);
			return;
		}

		bool base_create{
			context.force == parent_context::force_type::create ||
			force_create ||
			hasflags
		};

		vector<serializable *> childs{};

		const ucstring &child_name{tmp.get_string()};
		for(serializable *parent : *parents_) {
			bool create{base_create};
			
			if(!dynamic_if_contexts.empty()) {
				const dynamic_if_context &top_context{dynamic_if_contexts.top()};
				const serializable *child_tmp{parent->get_child(child_name)};
				if(child_tmp) {
					if(child_tmp->get_condition() != top_context.check) {
						create = true;
					}
				}
			}
			
			serializable *child{nullptr};
			if(create) {
				child = &parent->create_child(child_name);
			} else {
				child = &parent->child(child_name);
			}
			childs.emplace_back(child);
			if(hasflags) {
				child->create_flags().merge(flags);
			}
		}

		if(!parse_key(context, childs, tmp)) {
			return;
		}
	}

	void serializable_parser::parse_key_end(parent_context &context, const vector<serializable *> *childs, univalue &tmp, bool footer)
	{
		bool last{false};
		if(context.tmpdata.grouping) {
			skip_newlines();

			const token &tok{token_()};
			token::extended_type extype{tok.get_extended_type()};
			if(tok.type_ != token::type::bit_or &&
				extype != token::extended_type::double_colon) {
				last = true;
				context.tmpdata.grouping = false;
				childs = &context.tmpdata.group;
			}
		}
		
		if(childs->empty()) {
			error(u8"empty childs"_sv);
			return;
		}

		value_type valtype{value_type::none};
		bool foundval{parse_key_value(*childs, valtype, tmp)};

		bool opened{parse_key_open(context, *childs, valtype)};
		if(!opened) {
			if(!foundval && valtype != value_type::none) {
				error(u8"expected identifier or opening but found: {}"_sv, token_());
				return;
			}

			if(footer) {
				parse_keys_footer(context, *childs);
			}
		}

		if(last) {
			context.tmpdata.group.clear();
		}
	}

	bool serializable_parser::parse_key_value(const vector<serializable *> &childs, value_type &valtype, univalue &tmp)
	{
		if(expect({token::type::assign})) {
			valtype = value_type::assign;
		} else if(expect({token::extended_type::plus_equal})) {
			valtype = value_type::add;
		} else if(expect({token::extended_type::minus_equal})) {
			valtype = value_type::remove;
		}

		bool foundval{false};
		if(valtype != value_type::none) {
			if(expect({token::type::identifier}, tmp)) {
				replace_defines(tmp);

				const ucstring &child_name{tmp.get_string()};
				for(serializable *child : childs) {
					if(valtype == value_type::assign) {
						child->set_value(tmp);
					} else if(valtype == value_type::add) {
						child->create_child(child_name);
					} else if(valtype == value_type::remove) {
						child->erase(child_name);
					}
				}
				foundval = true;
			}
		} else {
			if(expect({token::type::newline})) {
				skip_newlines();
			} else if(expect({token::type::identifier}, tmp)) {
				replace_defines(tmp);

				for(serializable *child : childs) {
					child->set_value(tmp);
				}
				foundval = true;
			}
		}

		return foundval;
	}

	bool serializable_parser::parse_key_open(parent_context &, const vector<serializable *> &childs, value_type type)
	{
		if(childs.empty()) {
			error(u8"empty childs"_sv);
			return false;
		}

		skip_newlines();

		bool separator{false};
		if(expect({token::type::comma}) ||
		   expect({token::type::semicolon})) {
			separator = true;
		}

		skip_newlines();

		if(expect({token::extended_type::brace_left}) ||
			expect({token::extended_type::bracket_left}) ||
			expect({token::extended_type::lesser}) ||
			expect({token::extended_type::parenthesis_left})) {
			if(separator) {
				error(u8"unexpected opening: {}"_sv, peek(-1));
				return false;
			}

			const token &tok{peek(-1)};

			parent_context &child_context{emplace_parent()};
			child_context.openoffset = absolute_offset();
			child_context.open_type = tok.get_extended_type();
			bool bracket{child_context.open_type == token::extended_type::bracket_left};
			if(bracket || type == value_type::add) {
				child_context.force = parent_context::force_type::create;
			} else if(type == value_type::remove) {
				child_context.force = parent_context::force_type::remove;
				if(bracket) {
					error(u8"cant force remove using brackets"_sv);
					return false;
				}
			}
			if(type == value_type::assign) {
				for(serializable *child : childs) {
					child->remove_all();
				}
			}
			child_context.parents = childs;

			if(child_context.parents.empty()) {
				error(u8"empty childs"_sv);
				return false;
			}

			return true;
		}

		skip_newlines();

		return false;
	}

	bool serializable_parser::parse_key_close(parent_context &context, bool footer)
	{
		skip_newlines();

		token::extended_type close{context.close_type()};
		if(close == token::extended_type::unknown) {
			MFW_DEBUGBREAK();
			error(u8"what the fuck"_sv);
			return false;
		}

		if(expect({close})) {
			if(parents.size() == 1) {
				error(u8"unexpected closing: {}"_sv, peek(-1));
				return false;
			}

			parent_context &ctx{parent()};

			if(footer) {
				parse_keys_footer(ctx, context.parents);
			}

			pop_parent();
			return true;
		}

		skip_newlines();

		return false;
	}

	void serializable_parser::parse_keys_footer(parent_context &context, const vector<serializable *> &childs)
	{
		if(finished()) {
			return;
		}

		skip_newlines();

		if(expect({token::extended_type::double_colon})) {
			context.tmpdata.namespace_ = true;
			if(!context.tmpdata.group.empty()) {
				context.tmpdata.group.pop_back();
			}
			context.tmpdata.parents = childs;
		} else if(expect({token::type::bit_or})) {
			if(context.tmpdata.namespace_) {
				context.tmpdata.namespace_ = false;
				context.tmpdata.parents.clear();
			}
			if(!context.tmpdata.grouping) {
				context.tmpdata.grouping = true;
				context.tmpdata.group.clear();
				context.tmpdata.group = childs;
			}
		} else {
			if(context.tmpdata.namespace_) {
				context.tmpdata.namespace_ = false;
				context.tmpdata.parents.clear();
			}
			if(context.tmpdata.grouping) {
				context.tmpdata.grouping = false;
				context.tmpdata.group.clear();
			}
		}

		MFW_MESSAGE("is this the right place ?")
		if(expect({token::type::comma}) ||
			expect({token::type::semicolon})) {
		}
	}

	serializable_parser::token::extended_type serializable_parser::parent_context::close_type() const
	{
		switch(open_type) {
			case token::extended_type::bracket_left: { return token::extended_type::bracket_right; }
			case token::extended_type::lesser: { return token::extended_type::greater; }
			case token::extended_type::brace_left: { return token::extended_type::brace_right; }
			case token::extended_type::parenthesis_left: { return token::extended_type::parenthesis_right; }
			default: { return token::extended_type::unknown; }
		}
	}
}