#ifndef __MFW_PRIVATE_RENDERER_GRAPHICS_CARD_H
#define __MFW_PRIVATE_RENDERER_GRAPHICS_CARD_H

#pragma once

#include <public/mfw/stl/stdint.hpp>
#include <private/mfw/renderer/opaque_data.hpp>

namespace mfw::renderer
{
	class graphics_card
	{
	public:
		static bool initialize();
		static void shutdown();

	public:
		ucstring name{};
		size_t vendor{0};
		size_t device{0};

	public:
		__MFW_RENDERER_OPAQUE_DATA(display)
		__MFW_RENDERER_OPAQUE_DATA(render)
	};
}

#endif