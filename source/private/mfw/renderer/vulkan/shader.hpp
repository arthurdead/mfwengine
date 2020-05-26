#ifndef __MFW_PRIVATE_RENDERER_VULKAN_SHADER_H
#define __MFW_PRIVATE_RENDERER_VULKAN_SHADER_H

#pragma once

#include <private/mfw/renderer/vulkan/helpers.hpp>
#include <public/mfw/core/searchpath.hpp>

namespace mfw::renderer::vulkan
{
	class shader
	{
	public:
		shader(ucstring_view name) : name_{name} {}
		~shader() { reset(); }

		enum /*class*/ type : uint8_t
		{
			vertex,
			fragment,
			count,
		};

		static void initialize();
		static void shutdown();

		using stages_t = array<vk::PipelineShaderStageCreateInfo, type::count>;
		stages_t stages() const;

		void reset();

		bool load_files(const vk::Device &device);

	private:
		bool load_file(const vk::Device &device, type type_, const core::searchpath &search);

		u16string name_{};

		struct data_t
		{
			vk::UniqueShaderModule module_{};
			vk::PipelineShaderStageCreateInfo stageinfo{};
		};

		array<data_t, type::count> data{};
	};
};

#endif