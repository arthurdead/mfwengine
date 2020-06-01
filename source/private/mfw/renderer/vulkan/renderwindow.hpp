#ifndef __MFW_PRIVATE_RENDERER_VULKAN_RENDERWINDOW_H
#define __MFW_PRIVATE_RENDERER_VULKAN_RENDERWINDOW_H

#pragma once

#include <private/mfw/renderer/renderwindow.hpp>
#include <private/mfw/renderer/vulkan/helpers.hpp>
#include <private/mfw/renderer/vulkan/gpu.hpp>
#include <private/mfw/renderer/vulkan/shader.hpp>
#include <private/mfw/renderer/vulkan/mesh.hpp>

namespace mfw::renderer::vulkan
{
	class renderwindow : public agnostic::renderwindow
	{
	public:
		renderwindow(const gpu *gpu_, const monitor *mon);
		~renderwindow() override;

	private:
	#if MFW_OS_IS(WINDOWS)
		int64_t window_proc(uint32_t msg, uint64_t param1, int64_t param2) override;
	#endif

		vk::PresentModeKHR present_mode(const vk::PhysicalDevice &device) const;
		vk::SurfaceFormatKHR surface_format(const vk::PhysicalDevice &device) const;

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

		struct device_t
		{
			~device_t() { reset(); }

			vk::UniqueSurfaceKHR surface{};
			vk::UniqueDevice device{};
			vk::PhysicalDevice physical_device{};
			gpu::queue_array queues{};

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

		shader base_shader{u8"shader_vertexbuffer"_sv};

		struct memory_t
		{
			~memory_t() { reset(); }

			vk::UniqueCommandPool cmdpool{};

			struct vertex_t
			{
				~vertex_t() { reset(); }

				vk::UniqueBuffer buffer{};
				vk::UniqueDeviceMemory memory{};
				vector<mesh> vertices{};

				void reset()
				{
					vertices.clear();
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
}

#endif