#include <private/mfw/renderer/vulkan/gpu.hpp>
#include <public/mfw/core/logging_interface.hpp>

namespace mfw::renderer::vulkan
{
	MFW_DECLARE_LOG_CONTEXT(log_gpu, u"renderer/vulkan/gpu"_p);

	void gpu::queues(queue_array &required, const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface)
	{
		vector<vk::QueueFamilyProperties> familyinfos{MFW_VKRES(device.getQueueFamilyProperties())};

		for(uint32 fi{0}; fi < familyinfos.size(); fi++) {
			const vk::QueueFamilyProperties &props{familyinfos[fi]};

			queue_family_t family{};
			family.index = fi;

			vk::Bool32 supported{false};
			device.getSurfaceSupportKHR(fi, surface, &supported);

			if(props.queueCount > 0 && (props.queueFlags & vk::QueueFlagBits::eGraphics)) {
				required[queue_type::graphics] = family;
				if(supported) {
					//queues[QueueType::Present] = family;
				}
				continue;
			}

			if(props.queueCount > 0 && supported) {
				required[queue_type::present] = move(family);
				continue;
			}

			if(required.valid()) {
				break;
			}
		}
	}

	void gpu::layers(vector<const char8_t *> &required, const vk::PhysicalDevice &device, bool *anyunsupported)
	{
		vector<vk::LayerProperties> supported{MFW_VKRES(device.enumerateDeviceLayerProperties())};

		__vk_remove_unsupported(required, supported,
			[anyunsupported](const u8string_view &name) -> void {
				log_gpu.warning(u"{} not supported"_sv, convert(name));
				if(anyunsupported) {
					*anyunsupported = true;
				}
		});
	}

	void gpu::extensions(vector<const char8_t *> &required, const vk::PhysicalDevice &device, bool *anyunsupported)
	{
		vector<vk::ExtensionProperties> supported{MFW_VKRES(device.enumerateDeviceExtensionProperties())};

		required.push_back(rcast<const char8_t *>(VK_KHR_SWAPCHAIN_EXTENSION_NAME));

		__vk_remove_unsupported(required, supported,
			[anyunsupported](const u8string_view &name) -> void {
				log_gpu.warning(u"{} not supported"_sv, convert(name));
				if(anyunsupported) {
					*anyunsupported = true;
				}
		});
	}

	bool gpu::is_device_valid(const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface)
	{
		return true;
	}
};