#ifndef __MFW_PUBLIC_CORE_ACCESSOR_PARSER_INTERFACE_H
#define __MFW_PUBLIC_CORE_ACCESSOR_PARSER_INTERFACE_H

#pragma once

#include <public/mfw/core/core.hpp>
#include <public/mfw/core/rttr_interface.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/core/univalue.hpp>

namespace mfw::core
{
	namespace interfaces
	{
		class accessor_parser_callbacks
		{
		public:
			virtual bool get_variable(const ucstring_view &, type_holder &) const { return false; }
			virtual bool get_function(const ucstring_view &, const vector<univalue> &, type_holder &) const { return false; }
			virtual const func_info *get_function(const ucstring_view &) const { return nullptr; }

			virtual bool get_member_variable(const type_holder &, const ucstring_view &, type_holder &) const { return false; }
			virtual bool get_member_function(const type_holder &, const ucstring_view &, const vector<univalue> &, type_holder &) const { return false; }
		};
	}
}

#endif