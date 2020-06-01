#ifndef __MFW_PRIVATE_RENDERER_VULKAN_GPU_H
#define __MFW_PRIVATE_RENDERER_VULKAN_GPU_H

#pragma once

#include <private/mfw/renderer/gpu.hpp>
#include <private/mfw/renderer/vulkan/helpers.hpp>
#include <private/mfw/renderer/vulkan/renderer.hpp>
#include <public/mfw/stl/array.hpp>

namespace mfw::renderer::vulkan
{
	class gpu : public agnostic::gpu
	{
		friend class vulkan::renderer;

	public:
		const vk::PhysicalDevice &physical_device() const { return physical_device_; }

		enum /*class*/ queue_type : uchar_t
		{
			graphics,
			present,

			count,
		};

		struct queue_family_t
		{
			uint32_t index{VK_QUEUE_FAMILY_IGNORED};
			vk::Queue queue{};
		};

		class queue_array : public array<queue_family_t, queue_type::count>
		{
			public:
				bool valid() const {
					for(size_t i{0}; i < size(); i++) {
						if(operator[](i).index == VK_QUEUE_FAMILY_IGNORED) {
							return false;
						}
					}
					return true;
				}

				using queue_indexes_t = array<uint32_t, queue_type::count>;

				queue_indexes_t indexes() const {
					queue_indexes_t indexes{};
					for(size_t i{0}; i < size(); i++) {
						const queue_family_t &family{operator[](i)};
						if(family.index == VK_QUEUE_FAMILY_IGNORED) {
							continue;
						}
						indexes[i] = family.index;
					}
					return indexes;
				}

				queue_family_t &graphics() { return operator[](queue_type::graphics); }

				queue_family_t &present()
				{
					queue_family_t &family{operator[](queue_type::present)};
					if(family.index == VK_QUEUE_FAMILY_IGNORED) {
						return operator[](queue_type::graphics);
					}
					return family;
				}
		};

		static void queues(queue_array &required, const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface);
		static void layers(vector<const char8_t *> &required, const vk::PhysicalDevice &device, bool *anyunsupported = nullptr);
		static void extensions(vector<const char8_t *> &required, const vk::PhysicalDevice &device, bool *anyunsupported = nullptr);

		static bool is_device_valid(const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface);

		static const gpu &maingpu() { return *static_cast<const gpu *>(maingpu_); }

	private:
		vk::PhysicalDevice physical_device_{};
	};
}

#endif