#ifndef __MFW_PRIVATE_RENDERER_OPENGL_CONTEXT_API_FUNCS_H
#define __MFW_PRIVATE_RENDERER_OPENGL_CONTEXT_API_FUNCS_H

#pragma once

#include <private/mfw/renderer/window.hpp>
#include <private/mfw/renderer/graphics_card.hpp>

namespace mfw::renderer::interfaces
{
	class context_api_funcs
	{
	public:
		virtual ~context_api_funcs() = default;

	public:
		static context_api_funcs &instance();

		virtual bool initialize() = 0;
		virtual void create_window(window &win, const graphics_card &gpu, size_t w, size_t h) = 0;
		virtual void destroy_window(window &win) = 0;
		virtual void make_current(window &win) = 0;
		virtual void swap_buffers(window &win) = 0;
	};
}

#endif