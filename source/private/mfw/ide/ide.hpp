#ifndef __MFW_PRIVATE_BUILDER_BUILDER_H
#define __MFW_PRIVATE_BUILDER_BUILDER_H

#pragma once

#include <public/mfw/core/application.hpp>
#include <public/mfw/core/globals.hpp>
#include <public/mfw/renderer/renderer_interface.hpp>

namespace mfw::ide
{
	class ide final : core::interfaces::global_initializer
	{
	public:
		ide();
		~ide();

		static ide &instance();

		core::exit_status initialize() override;
		core::exit_status update() override { return {}; }
		core::exit_status shutdown() override { return {}; }
	};
}

#endif