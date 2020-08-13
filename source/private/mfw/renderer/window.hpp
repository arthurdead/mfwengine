#ifndef MFW_PRIVATE_RENDERER_WINDOW_HPP
#define MFW_PRIVATE_RENDERER_WINDOW_HPP

#pragma once

#include <public/mfw/stl/stdint.hpp>
#include <private/mfw/renderer/monitor.hpp>
#include <private/mfw/renderer/graphics_card.hpp>
#include <private/mfw/renderer/opaque_data.hpp>

namespace mfw::renderer
{
	#define __MFW_WINDOW_EVENT(name, args) \
		public: \
			using on_##name##_t = function<void args>; \
			void on_##name(on_##name##_t func) { on_##name##_func = func; } \
		private: \
			void on_##name() { if(on_##name##_func) { on_##name##_func(); } } \
			on_##name##_t on_##name##_func{};

	class window
	{
	public:
		static bool initialize();
		static void update();
		static void shutdown();

		window(const monitor &mon, const graphics_card &gpu, size_t w, size_t h, size_t x, size_t y);
		~window();

		void show(bool show_);

		__MFW_WINDOW_EVENT(closed, ())
		__MFW_WINDOW_EVENT(render, ())

	public:
		enum class internal_event_type : uchar_t
		{
			destroyed,
			closed,
			render,
		};

		void on_internal_event(internal_event_type type, opaque_data *data = nullptr);

		const monitor *mon_{nullptr};
		const graphics_card *gpu_{nullptr};

		enum class destroy_from : uchar_t
		{
			dtor,
			event,
		};

		destroy_from destroyed_from{destroy_from::dtor};

	public:
		__MFW_RENDERER_OPAQUE_DATA(display)
		__MFW_RENDERER_OPAQUE_DATA(render)
	};
}

#endif