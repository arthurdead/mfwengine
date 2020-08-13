#ifndef MFW_PRIVATE_CORE_EXPRESSION_PARSER_HPP
#define MFW_PRIVATE_CORE_EXPRESSION_PARSER_HPP

#pragma once

#include <private/mfw/core/base_parser.hpp>
#include <private/mfw/core/accessor_parser.hpp>
#include <public/mfw/core/expression_parser_interface.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/core/univalue.hpp>
#include <public/mfw/stl/defines.hpp>

namespace mfw::core
{
	class expression_parser : public base_parser
	{
		friend class base_parser;

	public:
		using super = base_parser;

		static expression_parser &instance();

		bool parse(const ucstring_view &str, univalue &result, const interfaces::expression_parser_callbacks *callback = nullptr);
		bool parse(const token_vec_t &tokens, univalue &result, const interfaces::expression_parser_callbacks *callback = nullptr);

		static void invert(token_vec_t &subtokens);
		static void append(token_vec_t &tokens1, const token_vec_t &tokens2);

	private:
		bool parse(const token_vec_t &tokens, token &result, const interfaces::expression_parser_callbacks *callback = nullptr);

		void clear() override;
		void parse_main() override;

		void error(const ucstring_view &str) override;
		using super::error;

		bool read_var(token &value);
		bool read_op(token &op, token &value);

		bool read_accessor(const token_vec_t &tokens, type_holder &result);

		static bool needs_assign(const token &tok);
		static bool stops_subparser(const token &start, const token &current);
		static bool needs_subparser(const token &tok);
		static bool stops_accessor(const token &tok, size_t &funcnum);
		bool is_unary(const token &tok);

		bool do_operator(const token &val1, const token &val2, const token &tok, token &result);

		token currvalue{};
		const interfaces::expression_parser_callbacks *callbacks_{nullptr};
	};
}

#endif