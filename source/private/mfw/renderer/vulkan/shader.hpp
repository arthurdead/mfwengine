#ifndef __MFW_PRIVATE_RENDERER_VULKAN_SHADER_HPP
#define __MFW_PRIVATE_RENDERER_VULKAN_SHADER_HPP

#pragma once

#include <private/mfw/renderer/vulkan/vulkan.hpp>
#include <public/mfw/core/searchpath.hpp>
#include <public/mfw/stl/array.hpp>
#include <private/mfw/renderer/vulkan/spirv_shader.hpp>

namespace mfw::renderer
{
	class shader_vulkan
	{
	public:
		shader_vulkan(ucstring_view name) : name_{name} {}
		~shader_vulkan() { reset(); }

		using type = spirv_shader_type;

		static void compile_shaders();
		static void shutdown();

		using stages_t = array<vk::PipelineShaderStageCreateInfo, type::count>;
		stages_t stages() const;

		void reset();

		bool load_files(const vk::Device &device);

	private:
		bool load_file(const vk::Device &device, type type_, const core::searchpath &search);

		ucstring name_{};

		struct data_t
		{
			vk::UniqueShaderModule module_{};
			vk::PipelineShaderStageCreateInfo stageinfo{};
		};

		array<data_t, type::count> data{};
	};
}

#endif