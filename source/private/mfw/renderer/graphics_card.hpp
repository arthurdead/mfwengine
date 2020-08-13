#ifndef MFW_PRIVATE_RENDERER_GRAPHICS_CARD_HPP
#define MFW_PRIVATE_RENDERER_GRAPHICS_CARD_HPP

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

		using list_t = ptr_vector<graphics_card>;
		static list_t &list();

		size_t vendor_id() const { return vendor; }
		size_t device_id() const { return device; }

		static graphics_card &main_gpu() { return *maingpu; }

	public:
		ucstring name{};
		size_t vendor{0};
		size_t device{0};

		static inline graphics_card *maingpu{nullptr};

	public:
		__MFW_RENDERER_OPAQUE_DATA(render)
	};
}

#endif