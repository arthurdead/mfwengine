#ifndef __MFW_PRIVATE_CORE_COMMANDLINE_VALIDATOR_H
#define __MFW_PRIVATE_CORE_COMMANDLINE_VALIDATOR_H

#pragma once

#include <public/mfw/core/commandline.hpp>
#include <public/mfw/core/serializable.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/core/univalue.hpp>
#include <public/mfw/core/expression_parser_interface.hpp>

namespace mfw::core
{
	class commandline_validator : interfaces::expression_parser_callbacks
	{
	public:
		bool parse(const commandline &cmdline_, const ucstring_view &str, serializable &temp);
		static void print(const serializable &ser);

	private:
		static void print_internal(const serializable &ser);
		bool parse_main(const serializable &ser) const;

		bool get_variable(const ucstring_view &name, type_holder &value) const override;

		commandline *cmdline{nullptr};
	};
}

#endif