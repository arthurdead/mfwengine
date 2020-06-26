#ifndef __MFW_PRIVATE_RENDERER_VULKAN_VULKAN_RENDER_API_FUNCS_H
#define __MFW_PRIVATE_RENDERER_VULKAN_VULKAN_RENDER_API_FUNCS_H

#pragma once

#include <private/mfw/renderer/render_api_funcs.hpp>
#include <private/mfw/renderer/vulkan/vulkan.hpp>
#include <private/mfw/renderer/vulkan/shared.hpp>
#include <private/mfw/renderer/vulkan/shader.hpp>
#include <public/mfw/stl/array.hpp>

namespace mfw::renderer
{
	class render_api_funcs_vulkan : public interfaces::render_api_funcs
	{
	public:
		bool initialize() override;
		void create_window(window &win, const graphics_card &gpu, size_t w, size_t h) override;
		void destroy_window(window &win) override {}
		void render_window(window &win) override {}

	private:
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

		struct gpu_render_data_t
		{
			vk::PhysicalDevice device{};
		};

		struct window_render_data_t
		{
			vk::PresentModeKHR present_mode(const vk::PhysicalDevice &device) const;
			vk::SurfaceFormatKHR surface_format(const vk::PhysicalDevice &device) const;

			void setup(vk::UniqueSurfaceKHR &&surface, const vk::PhysicalDevice &device);

			void create_device(const vk::PhysicalDevice &device);
			void create_swapchain(const vk::PhysicalDevice &device);
			void create_pipeline();
			void create_frame_buffers();
			void create_sync();
			void create_memory_buffers();
			void create_vertex_buffer();
			void create_index_buffer();
			void create_buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memoryflags, vk::UniqueBuffer &buffer, vk::UniqueDeviceMemory &memory);
			void copy_buffer(vk::UniqueBuffer &src, vk::UniqueBuffer &dst, size_t size);

			void begin_single_tile_commands(vk::UniqueCommandBuffer &cmdbuffer);
			void end_single_tile_commands(vk::UniqueCommandBuffer &cmdbuffer);

			void recreate_swapchain();

			~window_render_data_t();

			struct device_t
			{
				~device_t() { reset(); }

				vk::UniqueSurfaceKHR surface{};
				vk::UniqueDevice device{};
				vk::PhysicalDevice physical_device{};
				queue_array queues{};

				void reset()
				{
					device.reset(nullptr);
					surface.reset(nullptr);
				}
			};
			device_t device_{};

			struct swapchain_t
			{
				~swapchain_t() { reset(); }

				vk::UniqueSwapchainKHR swapchain{};
				vk::SurfaceFormatKHR format{};
				vk::Extent2D extent{};
				vector<vk::UniqueFramebuffer> framebuffers{};

				struct image_t
				{
					vk::Image image{};
					vk::UniqueImageView view{};
				};

				vector<image_t> images{};

				vector<vk::UniqueCommandBuffer> cmdbuffers{};
				bool resized{false};

				void reset()
				{
					cmdbuffers.clear();
					framebuffers.clear();
					images.clear();
					swapchain.reset(nullptr);
				}
			};
			swapchain_t swapchain_{};

			struct pipeline_t
			{
				~pipeline_t() { reset(); }

				vk::UniquePipelineLayout layout{};
				vk::UniquePipeline pipeline{};
				vk::UniqueRenderPass renderpass{};

				void reset()
				{
					layout.reset(nullptr);
					renderpass.reset(nullptr);
					pipeline.reset(nullptr);
				}
			};
			pipeline_t pipeline_{};

			struct sync_t
			{
				~sync_t() { reset(); }

				static inline constexpr stl::size_t MAX_FRAMES{2};

				array<vk::UniqueSemaphore, MAX_FRAMES> images{};
				array<vk::UniqueSemaphore, MAX_FRAMES> renders{};
				array<vk::UniqueFence, MAX_FRAMES> fences{};
				stl::size_t frame{0};

				void reset()
				{
					for(vk::UniqueSemaphore &semaphore : renders) {
						semaphore.reset(nullptr);
					}
					for(vk::UniqueSemaphore &semaphore : images) {
						semaphore.reset(nullptr);
					}
					for(vk::UniqueFence &fence : fences) {
						fence.reset(nullptr);
					}
				}
			};
			sync_t sync_{};

			shader_vulkan base_shader{u8"shader_vertexbuffer"_sv};

			struct memory_t
			{
				~memory_t() { reset(); }

				vk::UniqueCommandPool cmdpool{};

				struct vertex_t
				{
					~vertex_t() { reset(); }

					vk::UniqueBuffer buffer{};
					vk::UniqueDeviceMemory memory{};
					//vector<mesh> vertices{};

					void reset()
					{
						//vertices.clear();
						buffer.reset(nullptr);
						memory.reset(nullptr);
					}
				};
				vertex_t vertex{};

				struct index_t
				{
					~index_t() { reset(); }

					vk::UniqueBuffer buffer{};
					vk::UniqueDeviceMemory memory{};
					vector<uint16_t> indices{};

					void reset()
					{
						indices.clear();
						buffer.reset(nullptr);
						memory.reset(nullptr);
					}
				};
				index_t index{};

				void reset()
				{
					index.reset();
					vertex.reset();
					cmdpool.reset(nullptr);
				}
			};
			memory_t memory_{};
		};

		vk::UniqueInstance instance{};
	#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
		vk::UniqueDebugUtilsMessengerEXT messenger{};
	#endif

		static void queues(queue_array &required, const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface);
		static void instance_layers(vector<const ucchar_t *> &required, bool *anyunsupported = nullptr);
		static void instance_extensions(vector<const ucchar_t *> &required, bool *anyunsupported = nullptr);
		static void device_layers(vector<const ucchar_t *> &required, const vk::PhysicalDevice &device, bool *anyunsupported = nullptr);
		static void device_extensions(vector<const ucchar_t *> &required, const vk::PhysicalDevice &device, bool *anyunsupported = nullptr);
	};
}

#endif