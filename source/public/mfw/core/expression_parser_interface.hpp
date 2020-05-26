#ifndef __MFW_PUBLIC_CORE_EXPRESSION_PARSER_INTERFACE_H
#define __MFW_PUBLIC_CORE_EXPRESSION_PARSER_INTERFACE_H

#pragma once

#include <public/mfw/core/core.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/core/univalue.hpp>
#include <public/mfw/core/accessor_parser_interface.hpp>
#include <public/mfw/core/rttr_interface.hpp>

namespace mfw::core
{
	namespace interfaces
	{
		class expression_parser_callbacks : public accessor_parser_callbacks
		{
		public:
			
		};
	}

	extern "C"
	{
		MFW_CORE_API void MFW_CORE_CALL append_expression(const ucstring &expr1, const ucstring &expr2, ucstring &result);
		MFW_CORE_API void MFW_CORE_CALL invert_expression(const ucstring &expr1, ucstring &result);

		MFW_CORE_API bool MFW_CORE_CALL parse_expression(const ucstring_view &str, univalue &result, const interfaces::expression_parser_callbacks *callbacks = nullptr);
	}
}

#endif