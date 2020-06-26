#ifndef __MFW_PRIVATE_RENDERER_RENDER_API_FUNCS_H
#define __MFW_PRIVATE_RENDERER_RENDER_API_FUNCS_H

#pragma once

#include <private/mfw/renderer/window.hpp>

namespace mfw::renderer::interfaces
{
	class render_api_funcs
	{
	public:
		virtual ~render_api_funcs() = default;

	public:
		static render_api_funcs &instance();

		virtual bool pre_initialize() { return true; }
		virtual bool initialize() = 0;

		virtual void create_window(window &win, const graphics_card &gpu, size_t w, size_t h) = 0;
		virtual void destroy_window(window &win) = 0;

		virtual void render_window(window &win) = 0;
	};
}

#endif