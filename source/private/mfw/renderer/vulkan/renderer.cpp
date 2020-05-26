#include <private/mfw/renderer/vulkan/renderer.hpp>
#include <private/mfw/renderer/vulkan/gpu.hpp>
#include <private/mfw/renderer/vulkan/helpers.hpp>
#include <private/mfw/renderer/vulkan/shader.hpp>
#include <private/mfw/renderer/window.hpp>
#include <private/mfw/renderer/vulkan/renderwindow.hpp>
#include <public/mfw/core/logging_interface.hpp>
#if MFW_OS == MFW_OS_WINDOWS
	#include <Windows.h>
#endif

namespace mfw::renderer::vulkan
{
	MFW_DECLARE_LOG_CONTEXT(log_renderer, u"renderer/vulkan"_p);

	static VkBool32 MFW_SHAREDCALL __vk_debug_msg(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
	{
		if(bcast(messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) ||
		   bcast(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)) {
			return VK_TRUE;
		}

		ucstring msg{};
		convert(rcast<const char8_t *>(pCallbackData->pMessage), msg);

		if(bcast(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)) {
			log_renderer().info(u"{}"_sv, msg);
		} else if(bcast(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)) {
			log_renderer().warning(u"{}"_sv, msg);
		} else if(bcast(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)) {
			log_renderer().error(u"{}"_sv, msg);
		}

		return VK_TRUE;
	}

	core::exit_status renderer::initialize()
	{
		core::exit_status status{__super::initialize()};
		if(!status.succeded()) {
			return status;
		}

		//SetEnvironmentVariableW(L"DISABLE_LAYER_NV_OPTIMUS_1", L"");

		uint32_t apiver{0};
		vk::enumerateInstanceVersion(&apiver);

		vk::ApplicationInfo appinfo{};
		appinfo.setApiVersion(apiver);
		appinfo.setApplicationVersion(VK_MAKE_VERSION(0, 0, 0));
		appinfo.setEngineVersion(VK_MAKE_VERSION(0, 0, 0));
		appinfo.setPApplicationName("GAME_NAME");
		appinfo.setPEngineName("MFW_ENGINE");

		vk::InstanceCreateInfo instinfo{};
		instinfo.setPApplicationInfo(&appinfo);

		vector<const char8_t *> extensions_required{};
		extensions(extensions_required);
		instinfo.setEnabledExtensionCount(scast<uint32_t>(extensions_required.size()));
		instinfo.setPpEnabledExtensionNames(rcast<const char *const *>(extensions_required.data()));

		vector<const char8_t *> layers_required{};
		layers(layers_required);
		instinfo.setEnabledLayerCount(scast<uint32_t>(layers_required.size()));
		instinfo.setPpEnabledLayerNames(rcast<const char *const *>(layers_required.data()));

		instance_.instance = MFW_VKRES(vk::createInstanceUnique(move(instinfo), &__vk_alloc_callbacks()));

		#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
		vk::DebugUtilsMessengerCreateInfoEXT dbgmsginfo{};
		dbgmsginfo.setPfnUserCallback(rcast<PFN_vkDebugUtilsMessengerCallbackEXT>(__vk_debug_msg));
		dbgmsginfo.setPUserData(nullptr);
		dbgmsginfo.setMessageSeverity(scast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(vk::FlagTraits<vk::DebugUtilsMessageSeverityFlagBitsEXT>::allFlags));
		dbgmsginfo.setMessageType(scast<vk::DebugUtilsMessageTypeFlagBitsEXT>(vk::FlagTraits<vk::DebugUtilsMessageTypeFlagBitsEXT>::allFlags));
		instance_.messenger = MFW_VKRES(instance_.instance->createDebugUtilsMessengerEXTUnique(move(dbgmsginfo), &__vk_alloc_callbacks()));
		#endif

		vector<vk::PhysicalDevice> devices{MFW_VKRES(instance_.instance->enumeratePhysicalDevices())};

		vk::Win32SurfaceCreateInfoKHR win32info{};
		win32info.setHinstance(window::instance());
		win32info.setHwnd(window::desktop_window());

		vk::UniqueSurfaceKHR surface{MFW_VKRES(instance_.instance->createWin32SurfaceKHRUnique(move(win32info), &__vk_alloc_callbacks()))};

		for(const vk::PhysicalDevice &device : devices) {
			if(gpu::is_device_valid(device, *surface)) {
				vk::PhysicalDeviceProperties dprop{};
				device.getProperties(&dprop);

				const gpu *gpu_{scast<const gpu *>(gpu::find(dprop.deviceID))};

				log_renderer.info(u"found {}"_sv, convert(rcast<const char8_t *>(dprop.deviceName)));

				gpu::maingpu_ = gpu_;
				ccast<gpu *>(gpu_)->physical_device_ = device;
			}
		}

		shader::initialize();

		renderwindow *wtf = scast<renderwindow *>(create_window());

		return core::exit_code::success;
	}

	core::exit_code renderer::shutdown()
	{
		shader::shutdown();

		return __super::shutdown();
	}

	void renderer::layers(vector<const char8_t *> &required, bool *anyunsupported)
	{
		vector<vk::LayerProperties> supported{MFW_VKRES(vk::enumerateInstanceLayerProperties())};

		#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
		required.push_back(u8"VK_LAYER_KHRONOS_validation");
		#endif
		required.push_back(u8"VK_LAYER_NV_optimus");

		__vk_remove_unsupported(required, supported,
			[anyunsupported](const u8string_view &name) -> void {
				log_renderer.warning(u"{} not supported"_sv, convert(name));
				if(anyunsupported) {
					*anyunsupported = true;
				}
		});
	}

	void renderer::extensions(vector<const char8_t *> &required, bool *anyunsupported)
	{
		vector<vk::ExtensionProperties> supported{MFW_VKRES(vk::enumerateInstanceExtensionProperties())};

		#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
		required.push_back(rcast<const char8_t *>(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));
		#endif
		required.push_back(rcast<const char8_t *>(VK_KHR_SURFACE_EXTENSION_NAME));
		required.push_back(rcast<const char8_t *>(VK_KHR_WIN32_SURFACE_EXTENSION_NAME));

		__vk_remove_unsupported(required, supported,
			[anyunsupported](const u8string_view &name) -> void {
				log_renderer.warning(u"{} not supported"_sv, convert(name));
				if(anyunsupported) {
					*anyunsupported = true;
				}
		});
	}

	agnostic::gpu *renderer::create_gpu() const
	{
		return new gpu{};
	}

	agnostic::renderwindow *renderer::create_window() const
	{
		const gpu &gpu_{gpu::maingpu()};
		return new renderwindow{&gpu_, &gpu_.main_monitor()};
	}
};