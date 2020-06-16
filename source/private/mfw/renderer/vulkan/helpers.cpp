#include <private/mfw/renderer/vulkan/helpers.hpp>

#if MFW_COMPILER == MFW_COMPILER_MSVC
	#define __MFW_VK_GET_FUNCPTR(name) \
		using func_t = decltype(name) *; \
		__pragma(warning(suppress: 4191)) \
		static func_t func{reinterpret_cast<func_t>(vkGetInstanceProcAddr(instance, #name))};
#else
	#define __MFW_VK_GET_FUNCPTR(name) \
		using func_t = decltype(name) *; \
		static func_t func{reinterpret_cast<func_t>(vkGetInstanceProcAddr(instance, #name))};
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

namespace mfw::renderer::vulkan
{
	static __MFW_ALLOC_PRE __MFW_ALIGN_ALLOC_POST(2, 3) void * MFW_CALL_SHARED __vk_alloc(void *pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) noexcept
	{
		return allocate(size, alignment, 0, false,
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		__MFW_MEM_BLOCK,
		#endif
		__FILE__, __LINE__, nullptr
	#endif
		);
	}

	static __MFW_ALLOC_PRE __MFW_ALIGN_REALLOC_POST(3, 4) void * MFW_CALL_SHARED __vk_realloc(void *pUserData, void *pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) noexcept
	{
		return reallocate(pOriginal, size, alignment, 0, false,
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		__MFW_MEM_BLOCK,
		#endif
		__FILE__, __LINE__, nullptr
	#endif
		);
	}

	static void MFW_CALL_SHARED __vk_free(void *pUserData, void *pMemory) noexcept
	{
		deallocate(pMemory, 0, __MFW_MEM_ALIGN, 0, false,
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		__MFW_MEM_BLOCK,
		#endif
		__FILE__, __LINE__, nullptr
	#endif
		);
	}

	const vk::AllocationCallbacks &__vk_alloc_callbacks()
	{
		static vk::AllocationCallbacks callbacks{};
		MFW_EXECUTE_N_TIMES(1, {
			callbacks.setPUserData(nullptr);
			callbacks.setPfnAllocation(reinterpret_cast<PFN_vkAllocationFunction>(__vk_alloc));
			callbacks.setPfnReallocation(reinterpret_cast<PFN_vkReallocationFunction>(__vk_realloc));
			callbacks.setPfnFree(reinterpret_cast<PFN_vkFreeFunction>(__vk_free));
			callbacks.setPfnInternalAllocation(nullptr);
			callbacks.setPfnInternalFree(nullptr);
		})
		return callbacks;
	}
}