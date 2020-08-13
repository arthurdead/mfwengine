#ifndef MFW_PRIVATE_RENDERER_DISPLAY_API_FUNCS_HPP
#define MFW_PRIVATE_RENDERER_DISPLAY_API_FUNCS_HPP

#pragma once

#include <private/mfw/renderer/graphics_card.hpp>
#include <private/mfw/renderer/monitor.hpp>
#include <private/mfw/renderer/window.hpp>
#include <public/mfw/stl/vector.hpp>
#include <private/mfw/renderer/opaque_data.hpp>

namespace mfw::renderer::interfaces
{
	class display_api_funcs
	{
	public:
		virtual ~display_api_funcs() = default;

	public:
		static display_api_funcs &instance();

		virtual bool collect_gpus(ptr_vector<graphics_card> &gpus) = 0;
		virtual bool collect_monitors(ptr_vector<monitor> &monitors) = 0;
		virtual bool initialize() = 0;
		virtual void create_window(window &win, const monitor &monitor, size_t w, size_t h, ssize_t x, ssize_t y) = 0;
		virtual void show_window(window &win, bool show) = 0;
		virtual void destroy_window(window &win) = 0;
		virtual void update(monitor &mon) = 0;
		virtual void query_data(ucstring_view name, opaque_data &outputs, const opaque_data *inputs = nullptr) = 0;
	};
}

#endif