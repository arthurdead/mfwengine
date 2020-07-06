#ifndef __MFW_PRIVATE_RENDERER_MONITOR_HPP
#define __MFW_PRIVATE_RENDERER_MONITOR_HPP

#pragma once

#include <public/mfw/stl/stdint.hpp>
#include <private/mfw/renderer/opaque_data.hpp>

namespace mfw::renderer
{
	class monitor
	{
	public:
		static bool initialize();

		static monitor &main_monitor() { return *mainmonitor; }

	public:
		ucstring name{};
		size_t width{0};
		size_t height{0};
		ssize_t x{0};
		ssize_t y{0};

		static inline monitor *mainmonitor{nullptr};

	public:
		__MFW_RENDERER_OPAQUE_DATA(display)
	};
}

#endif