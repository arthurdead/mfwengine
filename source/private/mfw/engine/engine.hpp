#ifndef __MFW_PRIVATE_ENGINE_ENGINE_HPP
#define __MFW_PRIVATE_ENGINE_ENGINE_HPP

#pragma once

#include <public/mfw/engine/engine_interface.hpp>
#include <public/mfw/core/globals.hpp>
#include <public/mfw/core/serializable.hpp>
#include <public/mfw/core/library.hpp>

namespace mfw::engine
{
	class engine final : public interfaces::engine, core::interfaces::global_initializer
	{
	public:
		static engine &instance();
		
		engine();

	private:
		core::exit_status initialize() override;
		core::exit_status update() override;
		core::exit_status shutdown() override;

		core::serializable gameinfo{};
		core::library gamelibrary{};
	};
}

#endif