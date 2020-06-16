#ifndef __MFW_PRIVATE_CORE_ACCESSOR_PARSER_H
#define __MFW_PRIVATE_CORE_ACCESSOR_PARSER_H

#pragma once

#include <private/mfw/core/base_parser.hpp>
#include <public/mfw/core/rttr_interface.hpp>
#include <public/mfw/core/accessor_parser_interface.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/defines.hpp>

namespace mfw::core
{
	class accessor_parser : public base_parser
	{
	public:
		using super = base_parser;

		static accessor_parser &instance();

		bool parse(const ucstring_view &str, type_holder &result, const interfaces::accessor_parser_callbacks *callback = nullptr);
		bool parse(const token_vec_t &tokens, type_holder &result, const interfaces::accessor_parser_callbacks *callback = nullptr);

	private:
		void clear() override;
		void parse_main() override;

		bool get_internal_variable(const ucstring_view &name, type_holder &var) const;
		bool get_internal_function(const ucstring_view &name, const vector<univalue> &args, type_holder &var) const;
		bool get_internal_member_variable(const type_holder &obj, const ucstring_view &name, type_holder &var) const;
		bool get_internal_member_function(const type_holder &obj, const ucstring_view &name, const vector<univalue> &args, type_holder &var) const;

		void error(const ucstring_view &str) override;
		using super::error;

		const interfaces::accessor_parser_callbacks *callbacks_{nullptr};
		type_holder currentvar{};
	};
}

#endif