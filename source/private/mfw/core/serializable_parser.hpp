#ifndef MFW_PRIVATE_CORE_SERIALIZABLE_PARSER_HPP
#define MFW_PRIVATE_CORE_SERIALIZABLE_PARSER_HPP

#pragma once

#include <private/mfw/core/base_parser.hpp>
#include <public/mfw/core/serializable.hpp>
#include <public/mfw/core/expression_parser_interface.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/stl/stack.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/defines.hpp>

namespace mfw::core
{
	class serializable_parser : public base_parser, public interfaces::expression_parser_callbacks
	{
	public:
		using super = base_parser;

		static serializable_parser &instance();

		bool parse(const ucstring_view &str, serializable &root, const interfaces::serializable_parser_callbacks *callbacks_ = nullptr);
		bool parse(const token_vec_t &tokens, serializable &root, const interfaces::serializable_parser_callbacks *callbacks_ = nullptr);

		bool contains_special_chars(const ucstring &str)
		{ return scanner::instance().contains_special_chars(str); }
		bool contains_special_chars(const univalue &val)
		{ return scanner::instance().contains_special_chars(val); }

	private:
		void parse_setup(serializable &root, const interfaces::serializable_parser_callbacks *callbacks_ = nullptr);
	
		void clear() override;
		void parse_main() override;

		void error(const ucstring_view &str) override;
		void error(const ucstring_view &str, const token &tok);
		using super::error;

		void parse_preprocessor();
		void parse_dynamicprocessor();
		void parse_keys_header();
		void parse_keys() { parse_keys_header(); }

		enum class value_type : uchar_t
		{
			none,
			assign,
			add,
			remove,
		};

		bool parse_key_value(serializable &child, value_type &type, univalue &tmp) {
			vector<serializable *> childs{};
			childs.emplace_back(&child);
			return parse_key_value(childs, type, tmp);
		}
		bool parse_key_value(const vector<serializable *> &childs, value_type &type, univalue &tmp);

		const serializable *get_inherit(const univalue &name) const;
		const serializable *get_inherit(const ucstring_view &name) const;

		bool get_variable(const ucstring_view &name, type_holder &var) const override;
		bool get_member_variable(const type_holder &obj, const ucstring_view &name, type_holder &var) const override;
		bool get_function(const ucstring_view &name, const vector<univalue> &args, type_holder &var) const override;

		pstring filename() const;

		void replace_defines(ucstring &str) const;
		void replace_defines(univalue &str) const;
		void replace_defines(token_vec_t &str) const;

		struct parent_context
		{
			vector<serializable *> parents{};

			enum class force_type : uchar_t
			{
				none,
				create,
				remove,
			};
			force_type force{force_type::none};

			token::extended_type open_type{token::extended_type::unknown};
			token::extended_type close_type() const;

			size_t openoffset{0};

			struct __tmpdata
			{
				vector<serializable *> group{};
				bool grouping{false};
				vector<serializable *> parents{};
				bool namespace_{false};
			};
			__tmpdata tmpdata{};
		};

		bool handle_include(bool optional, bool root, univalue &tmp);

		bool parse_key_open(parent_context &context, serializable &child, value_type type) {
			vector<serializable *> tmp{};
			tmp.emplace_back(&child);
			return parse_key_open(context, tmp, type);
		}
		bool parse_key_open(parent_context &context, const vector<serializable *> &childs, value_type type);
		bool parse_key_close(parent_context &context, bool footer);
		void parse_keys_footer(parent_context &context, serializable &child) {
			vector<serializable *> childs{};
			childs.emplace_back(&child);
			parse_keys_footer(context, childs);
		}
		void parse_keys_footer(parent_context &context, const vector<serializable *> &childs);
		void parse_key_end(parent_context &context, serializable &child, univalue &tmp, bool footer) {
			vector<serializable *> childs{};
			childs.emplace_back(&child);
			parse_key_end(context, &childs, tmp, footer);
		}
		void parse_key_end(parent_context &context, const vector<serializable *> *childs, univalue &tmp, bool footer);

		bool parse_key(parent_context &context, serializable &child, univalue &tmp) {
			vector<serializable *> childs{};
			childs.emplace_back(&child);
			return parse_key(context, childs, tmp);
		}
		bool parse_key(parent_context &context, const vector<serializable *> &childs, univalue &tmp);

		parent_context &top();
		const parent_context &top() const { return const_cast<serializable_parser *>(this)->top(); }
		parent_context &root();
		const parent_context &root() const { return const_cast<serializable_parser *>(this)->root(); }
		parent_context &parent();
		const parent_context &parent() const { return const_cast<serializable_parser *>(this)->parent(); }

		parent_context &emplace_parent();
		void pop_parent();

		struct base_if_context
		{
			bool last{false};
		};

		struct static_if_context : base_if_context
		{
			bool skipping{false};
			bool success{false};

			token_vec_t check{};

			void invert();
			void append(const token_vec_t &check_);
		};

		struct dynamic_if_context : base_if_context
		{
			ucstring check{};

			void invert();
			void append(const ucstring &check_);
		};

		size_t if_depth{0};

		bool is_skipping() const;

		stack<static_if_context> static_if_contexts{};
		stack<dynamic_if_context> dynamic_if_contexts{};

		vector<unique_ptr<parent_context>> parents{};
		unordered_map<ucstring, univalue> defines{};
		
		pstring filename_{};

		const interfaces::serializable_parser_callbacks *callbacks{nullptr};
	};
}

#endif