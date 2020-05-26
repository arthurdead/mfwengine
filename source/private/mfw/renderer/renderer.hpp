#ifndef __MFW_PRIVATE_RENDERER_RENDERER_H
#define __MFW_PRIVATE_RENDERER_RENDERER_H

#pragma once

#include <public/mfw/renderer/renderer_interface.hpp>
#include <public/mfw/core/globals.hpp>
#include <private/mfw/renderer/gpu.hpp>
#include <private/mfw/renderer/renderwindow.hpp>

namespace mfw::renderer::agnostic
{
	class renderer : public interfaces::renderer, core::interfaces::global_initializer
	{
	public:
		static renderer &instance();

		core::exit_status initialize() override;
		core::exit_status update() override;
		core::exit_status shutdown() override;

		virtual gpu *create_gpu() const;
		virtual renderwindow *create_window() const;

	private:
		void do_stuff() override;
	};
};

#endif