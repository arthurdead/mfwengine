#ifndef __MFW_PRIVATE_CORE_COMMANDLINE_PARSER_H
#define __MFW_PRIVATE_CORE_COMMANDLINE_PARSER_H

#pragma once

#include <private/mfw/core/base_parser.hpp>
#include <public/mfw/core/commandline.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/defines.hpp>

namespace mfw::core
{
	class commandline_parser : public base_parser
	{
	public:
		using super = base_parser;

		static commandline_parser &instance();

		bool parse(const ucstring_view &str, commandline &cmdline_);

		void clear() override;
		void parse_main() override;

		void error(const ucstring_view &str) override;
		using super::error;

	private:
		commandline *cmdline{nullptr};
	};
}

#endif