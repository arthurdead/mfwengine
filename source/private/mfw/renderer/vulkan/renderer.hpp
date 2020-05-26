#ifndef __MFW_PRIVATE_RENDERER_VULKAN_RENDERER_H
#define __MFW_PRIVATE_RENDERER_VULKAN_RENDERER_H

#pragma once

#include <private/mfw/renderer/renderer.hpp>
#include <private/mfw/renderer/gpu.hpp>
#include <private/mfw/renderer/vulkan/helpers.hpp>

namespace mfw::renderer::vulkan
{
	class renderer : public agnostic::renderer
	{
	public:
		static renderer &instance() { return static_cast<renderer &>(__super::instance()); }

		core::exit_status initialize() override;
		core::exit_status shutdown() override;

		agnostic::gpu *create_gpu() const override;
		agnostic::renderwindow *create_window() const override;

		vk::Instance &vulkan_instance() { return instance_.instance.get(); }

	private:
		static void layers(vector<const ucchar_t *> &required, bool *anyunsupported = nullptr);
		static void extensions(vector<const ucchar_t *> &required, bool *anyunsupported = nullptr);

		struct instance_t
		{
			~instance_t() { reset(); }

			vk::UniqueInstance instance{};
		#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
			vk::UniqueDebugUtilsMessengerEXT messenger{};
		#endif

			void reset()
			{
			#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
				messenger.reset(nullptr);
			#endif
				instance.reset(nullptr);
			}
		};
		instance_t instance_{};
	};
};

#endif