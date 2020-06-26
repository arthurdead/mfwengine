#ifndef __MFW_PRIVATE_RENDERER_RENDERER_H
#define __MFW_PRIVATE_RENDERER_RENDERER_H

#pragma once

#include <public/mfw/renderer/renderer_interface.hpp>
#include <public/mfw/core/globals.hpp>
//#include <private/mfw/renderer/gpu.hpp>
//#include <private/mfw/renderer/renderwindow.hpp>

namespace mfw::renderer
{
	class renderer : public interfaces::renderer, core::interfaces::global_initializer
	{
	public:
		renderer()
			: core::interfaces::global_initializer{u8"renderer"_sv} {}

		static renderer &instance();

		core::exit_status initialize() override;
		core::exit_status update() override;
		core::exit_status shutdown() override;

		core::exit_status initialize_render_api() override;

		//virtual gpu *create_gpu() const;
		//virtual renderwindow *create_window() const;

		bool wants_integrated() const { return true; }

	private:
		void do_stuff() override;
	};
}

#endif