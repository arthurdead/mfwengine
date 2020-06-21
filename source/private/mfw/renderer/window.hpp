#ifndef __MFW_PRIVATE_RENDERER_WINDOW_H
#define __MFW_PRIVATE_RENDERER_WINDOW_H

#pragma once

#include <public/mfw/stl/stdint.hpp>
#include <private/mfw/renderer/monitor.hpp>
#include <private/mfw/renderer/opaque_data.hpp>

namespace mfw::renderer
{
	class window
	{
	public:
		static bool initialize();
		static void update();
		static void shutdown();

		window(const monitor &mon, size_t w, size_t h, size_t x, size_t y);
		~window();

		void show(bool show_);

	public:
		const monitor *mon_{nullptr};

		__MFW_RENDERER_OPAQUE_DATA(display)
		__MFW_RENDERER_OPAQUE_DATA(render)
	};
}

#endif