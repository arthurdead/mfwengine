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
	MFW_DECLARE_LOG_CONTEXT(log_renderer, u8"renderer/vulkan"_p)

	static VkBool32 MFW_CALL_SHARED __vk_debug_msg(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
	{
		if(bool_cast(messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) ||
			bool_cast(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)) {
			return VK_TRUE;
		}

		ucstring msg{uc_str(pCallbackData->pMessage)};

		if(bool_cast(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)) {
			log_renderer().info(u8"{}"_sv, msg);
		} else if(bool_cast(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)) {
			log_renderer().warning(u8"{}"_sv, msg);
		} else if(bool_cast(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)) {
			log_renderer().error(u8"{}"_sv, msg);
		}

		return VK_TRUE;
	}

	core::exit_status renderer::initialize()
	{
		core::exit_status status{super::initialize()};
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

		vector<const ucchar_t *> extensions_required{};
		extensions(extensions_required);
		instinfo.setEnabledExtensionCount(static_cast<uint32_t>(extensions_required.size()));
		instinfo.setPpEnabledExtensionNames(reinterpret_cast<const char *const *const>(extensions_required.data()));

		vector<const ucchar_t *> layers_required{};
		layers(layers_required);
		instinfo.setEnabledLayerCount(static_cast<uint32_t>(layers_required.size()));
		instinfo.setPpEnabledLayerNames(reinterpret_cast<const char *const *const>(layers_required.data()));

		instance_.instance = MFW_VKRES(vk::createInstanceUnique(move(instinfo), &__vk_alloc_callbacks()));

		#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
		vk::DebugUtilsMessengerCreateInfoEXT dbgmsginfo{};
		dbgmsginfo.setPfnUserCallback(reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(__vk_debug_msg));
		dbgmsginfo.setPUserData(nullptr);
		dbgmsginfo.setMessageSeverity(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(vk::FlagTraits<vk::DebugUtilsMessageSeverityFlagBitsEXT>::allFlags));
		dbgmsginfo.setMessageType(static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(vk::FlagTraits<vk::DebugUtilsMessageTypeFlagBitsEXT>::allFlags));
		instance_.messenger = MFW_VKRES(instance_.instance->createDebugUtilsMessengerEXTUnique(move(dbgmsginfo), &__vk_alloc_callbacks()));
		#endif

		vector<vk::PhysicalDevice> devices{MFW_VKRES(instance_.instance->enumeratePhysicalDevices())};

	#if MFW_OS_IS(WINDOWS)
		vk::Win32SurfaceCreateInfoKHR win32info{};
		win32info.setHinstance(window::instance());
		win32info.setHwnd(window::desktop_window());

		vk::UniqueSurfaceKHR surface{MFW_VKRES(instance_.instance->createWin32SurfaceKHRUnique(move(win32info), &__vk_alloc_callbacks()))};
	#else
		//#error
	#endif

	#if 0
		for(const vk::PhysicalDevice &device : devices) {
			if(gpu::is_device_valid(device, *surface)) {
				vk::PhysicalDeviceProperties dprop{};
				device.getProperties(&dprop);

				const gpu *gpu_{static_cast<const gpu *>(gpu::find(dprop.deviceID))};

				ucstring name{uc_str(dprop.deviceName)};
				log_renderer.info(u"found {}"_sv, name);

				gpu::maingpu_ = gpu_;
				const_cast<gpu *>(gpu_)->physical_device_ = device;
			}
		}
	#endif

		shader::initialize();

		return core::exit_status::success;
	}

	core::exit_status renderer::shutdown()
	{
		shader::shutdown();

		return super::shutdown();
	}

	void renderer::layers(vector<const ucchar_t *> &required, bool *anyunsupported)
	{
		vector<vk::LayerProperties> supported{MFW_VKRES(vk::enumerateInstanceLayerProperties())};

		#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
		required.push_back(u8"VK_LAYER_KHRONOS_validation");
		#endif
		required.push_back(u8"VK_LAYER_NV_optimus");

		__vk_remove_unsupported(required, supported,
			[anyunsupported](const ucstring_view &name) -> void {
				log_renderer().warning(u8"{} not supported"_sv, name);
				if(anyunsupported) {
					*anyunsupported = true;
				}
		});
	}

	void renderer::extensions(vector<const ucchar_t *> &required, bool *anyunsupported)
	{
		vector<vk::ExtensionProperties> supported{MFW_VKRES(vk::enumerateInstanceExtensionProperties())};

		#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
		required.push_back(reinterpret_cast<const ucchar_t *>(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));
		#endif
		required.push_back(reinterpret_cast<const ucchar_t *>(VK_KHR_SURFACE_EXTENSION_NAME));
	#if MFW_OS_IS(WINDOWS)
		required.push_back(reinterpret_cast<const ucchar_t *>(VK_KHR_WIN32_SURFACE_EXTENSION_NAME));
	#else
		//#error
	#endif

		__vk_remove_unsupported(required, supported,
			[anyunsupported](const ucstring_view &name) -> void {
				log_renderer().warning(u8"{} not supported"_sv, name);
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
}