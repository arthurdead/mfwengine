#include <private/mfw/renderer/vulkan/mesh.hpp>

namespace mfw::renderer::vulkan
{
	const vk::VertexInputBindingDescription &mesh::binding_description()
	{
		static vk::VertexInputBindingDescription binding{};

		MFW_EXECUTE_N_TIMES(1, {
			binding.setBinding(0);
			binding.setStride(sizeof(float32_t) * 2 + sizeof(float32_t) * 3);
			binding.setInputRate(vk::VertexInputRate::eVertex);
		})

		return binding;
	}

	const array<vk::VertexInputAttributeDescription, 2> &mesh::attribute_descriptions()
	{
		static array<vk::VertexInputAttributeDescription, 2> attributedesc{};

		MFW_EXECUTE_N_TIMES(1, {
			attributedesc[0].setBinding(0);
			attributedesc[0].setLocation(0);
			attributedesc[0].setFormat(vk::Format::eR32G32Sfloat);
			attributedesc[0].setOffset(0);

			attributedesc[1].setBinding(0);
			attributedesc[1].setLocation(1);
			attributedesc[1].setFormat(vk::Format::eR32G32B32Sfloat);
			attributedesc[1].setOffset(8);
		})

		return attributedesc;
	}
}