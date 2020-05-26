#include <private/mfw/core/commandline_parser.hpp>
#include <public/mfw/core/commandline.hpp>
#include <public/mfw/core/globals.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/pch_literals.hpp>

namespace mfw::core
{
	MFW_DECLARE_LOG_CONTEXT(log_commandline_parser, u8"core/commandline_parser"_p)

	MFW_DECLARE_GLOBAL_ALLOCATOR(commandlineparser, commandline_parser)

	commandline_parser &commandline_parser::instance() {
		return __commandlineparser_global_allocator.instance();
	}

	void commandline_parser::clear()
	{
		super::clear();

		cmdline = nullptr;
	}

	bool commandline_parser::parse(const ucstring_view &str, commandline &cmdline_)
	{
		clear();

		cmdline = &cmdline_;

		ignore_flags flags{ignore_flags::default_};
		//flags |= ignore_flags::divide;
		flags |= ignore_flags::slash;
		//flags |= ignore_flags::colon;
		flags |= ignore_flags::dot;

		return super::parse(str, flags);
	}

	void commandline_parser::parse_main()
	{
		while(true)
		{
			if(finished()) {
				break;
			}

			if(expect({token::extended_type::minus_minus}) ||
				expect({token::type::subtract}) ||
				expect({token::type::divide}) ||
				expect({token::type::add})) {

			}

			univalue name{};
			if(!expect({token::type::identifier}, name)) {
				error(u8"expected identifier but found: {}"_sv, token_().as_string());
				return;
			}

			vector<univalue> values{};
			if(expect({token::type::assign}) || expect({token::type::colon})) {
				while(true)
				{
					univalue &value{values.emplace_back()};
					if(!expect({token::type::identifier}, value)) {
						error(u8"expected identifier but found: {}"_sv, token_().as_string());
						return;
					}

					if(!expect({token::type::comma})) {
						break;
					}
				}
			}

			const ucstring &arg_name{name.get_string()};
			cmdline->add(arg_name, values);
		}
	}

	void commandline_parser::error(const ucstring_view &str)
	{
		super::error(str);

		log_commandline_parser().error(str);
	}
}