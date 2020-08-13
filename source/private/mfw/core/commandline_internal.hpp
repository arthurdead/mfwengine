#ifndef MFW_CORE_COMMANDLINE_INTERNAL_HPP
#define MFW_CORE_COMMANDLINE_INTERNAL_HPP

#pragma once

#include <public/mfw/core/application.hpp>
#include <public/mfw/core/commandline.hpp>
#include <public/mfw/core/globals.hpp>

namespace mfw::core
{
	class commandline_internal final : public commandline, interfaces::global_initializer
	{
	public:
		static commandline_internal &instance();

	private:
		exit_status initialize() override;
		exit_status update() override { return {}; }
		exit_status shutdown() override { return {}; }
	};
}

#endif