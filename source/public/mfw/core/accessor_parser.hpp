#ifndef MFW_PUBLIC_CORE_ACCESSOR_PARSER_INTERFACE_HPP
#define MFW_PUBLIC_CORE_ACCESSOR_PARSER_INTERFACE_HPP

#pragma once

#include <public/mfw/core/core.hpp>
#include <public/mfw/core/rttr_interface.hpp>
#include <public/mfw/stl/string_view.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/core/univalue.hpp>

namespace mfw::core
{
	class MFW_ABSTRACT_CLASS AccessorParserCallbacks
	{
	protected:
		virtual ~AccessorParserCallbacks() noexcept = default;
		
	public:
		virtual bool getVariable(stl::osstring_view, TypeHolder &) const noexcept
		{ return false; }
		virtual bool getFunction(stl::osstring_view, const stl::vector<UniValue> &, TypeHolder &) const noexcept
		{ return false; }
		virtual const FuncInfo *get_function(stl::osstring_view) const noexcept
		{ return nullptr; }

		virtual bool get_member_variable(const TypeHolder &, stl::osstring_view, TypeHolder &) const noexcept
		{ return false; }
		virtual bool get_member_function(const TypeHolder &, stl::osstring_view, const stl::vector<UniValue> &, TypeHolder &) const noexcept
		{ return false; }
	};
}

#endif