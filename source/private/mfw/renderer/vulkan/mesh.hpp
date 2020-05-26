#ifndef __MFW_PRIVATE_RENDERER_VULKAN_MESH_H
#define __MFW_PRIVATE_RENDERER_VULKAN_MESH_H

#pragma once

#include <private/mfw/renderer/mesh.hpp>
#include <private/mfw/renderer/vulkan/helpers.hpp>

namespace mfw::renderer::vulkan
{
	class mesh : agnostic::mesh
	{
	public:
		static const vk::VertexInputBindingDescription &binding_description();
		static const array<vk::VertexInputAttributeDescription, 2> &attribute_descriptions();
	};
};

#endif