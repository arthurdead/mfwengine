#ifndef __MFW_PRIVATE_RENDERER_VULKAN_SHARED_HPP
#define __MFW_PRIVATE_RENDERER_VULKAN_SHARED_HPP

#pragma once

#include <private/mfw/renderer/vulkan/vulkan.hpp>

namespace mfw::renderer
{
	class window;

	namespace vulkan
	{
		struct base_input_t
		{
			vk::UniqueInstance *instance{nullptr};
		};

		struct input_create_window_t : base_input_t
		{
			window *win{nullptr};
		};

		vk::AllocationCallbacks &alloc_callbacks();
	}
}

#endif