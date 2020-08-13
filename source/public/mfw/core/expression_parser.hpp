#ifndef MFW_PUBLIC_CORE_EXPRESSION_PARSER_INTERFACE_HPP
#define MFW_PUBLIC_CORE_EXPRESSION_PARSER_INTERFACE_HPP

#pragma once

#include <public/mfw/core/core.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/core/univalue.hpp>
#include <public/mfw/core/accessor_parser_interface.hpp>
#include <public/mfw/core/rttr_interface.hpp>

namespace mfw::core
{
	class MFW_ABSTRACT_CLASS ExpressionParserCallbacks : public AccessorParserCallbacks
	{
	protected:
		~ExpressionParserCallbacks() noexcept override = default;
		
	public:
		
	};

	MFW_CORE_API void MFW_CORE_CALL append_expression(const stl::osstring &expr1, const stl::osstring &expr2, stl::osstring &result) noexcept;
	MFW_CORE_API void MFW_CORE_CALL invert_expression(const stl::osstring &expr1, stl::osstring &result) noexcept;

	MFW_CORE_API bool MFW_CORE_CALL parse_expression(stl::osstring_view str, UniValue &result, const ExpressionParserCallbacks *callbacks = nullptr) noexcept;
}

#endif