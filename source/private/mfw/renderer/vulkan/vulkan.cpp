#include <private/mfw/renderer/vulkan/vulkan.hpp>
#include <public/mfw/stl/version.hpp>

#define __MFW_VK_GET_FUNCPTR_BEGIN(name) \
	using func_t = decltype(name) *;

#define __MFW_VK_GET_FUNCPTR_END(name) \
	static func_t func{reinterpret_cast<func_t>(vkGetInstanceProcAddr(instance, #name))};

#if MFW_COMPILER_FLAGGED(MSVC)
	#define __MFW_VK_GET_FUNCPTR(name) \
		__MFW_VK_GET_FUNCPTR_BEGIN(name) \
		MFW_WARNING_SUPPRESS(4191) \
		__MFW_VK_GET_FUNCPTR_END(name)
#else
	#define __MFW_VK_GET_FUNCPTR(name) \
		__MFW_VK_GET_FUNCPTR_BEGIN(name) \
		__MFW_VK_GET_FUNCPTR_END(name)
#endif

#ifdef VK_NO_PROTOTYPES
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#endif

extern "C"
{
#ifdef VK_NO_PROTOTYPES
PFN_vkVoidFunction vkGetInstanceProcAddr(VkInstance instance, const char *pName)
{
	return VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr(instance, pName);
}
#endif

#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pMessenger)
{
	__MFW_VK_GET_FUNCPTR(vkCreateDebugUtilsMessengerEXT)
	if(!func) {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
	return func(instance, pCreateInfo, pAllocator, pMessenger);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks *pAllocator)
{
	__MFW_VK_GET_FUNCPTR(vkDestroyDebugUtilsMessengerEXT)
	if(!func) {
		return;
	}
	func(instance, messenger, pAllocator);
}
#endif
}