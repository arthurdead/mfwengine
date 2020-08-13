#ifndef MFW_SCRIPTING_SCRIPTING_INTERFACE_HPP
#define MFW_SCRIPTING_SCRIPTING_INTERFACE_HPP

#pragma once

#include <public/mfw/scripting/scripting.hpp>

namespace mfw::scripting::interfaces
{
	class scripting
	{
	protected:
		virtual ~scripting() = default;

	public:
		MFW_SCRIPTING_API static scripting & MFW_SCRIPTING_CALL instance();

		virtual univalue execute_string(const u16string_view &str) const = 0;
	};
};

#endif