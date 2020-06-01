#ifndef __MFW_PRIVATE_RENDERER_VULKAN_HELPERS_H
#define __MFW_PRIVATE_RENDERER_VULKAN_HELPERS_H

#pragma once

#if 0
	#define module module_
	#include <glad/vulkan.h>
	#undef module
#else
	#define module module_
	#pragma push_macro("free")
	#undef free
	#if MFW_COMPILER == MFW_COMPILER_MSVC
		MFW_WARNING_PUSH()
		MFW_WARNING_DISABLE(4061 4582 4191)
		MFW_WARNING_DISABLE(4458)
	#endif
namespace std
{
	template <typename T>
	using __vulkan_allocator_replace_hack = ::mfw::allocator<T>;

	template <typename T, typename A = ::mfw::allocator<T>>
	using __vulkan_vector_replace_hack = ::mfw::vector<T, A>;

	#define allocator __vulkan_allocator_replace_hack
	#define vector __vulkan_vector_replace_hack
}
	#include <vulkan/vulkan.hpp>
	#undef allocator
	#undef vector
	#if MFW_COMPILER == MFW_COMPILER_MSVC
		MFW_WARNING_POP()
	#endif
	#pragma pop_macro("free")
	#undef module
#endif

namespace mfw::renderer::vulkan
{
#ifdef VULKAN_HPP_NO_EXCEPTIONS
	#define MFW_VKRES(result) ((result).value)
#else
	#define MFW_VKRES(result) (result)
#endif

	const vk::AllocationCallbacks &__vk_alloc_callbacks();

	template <typename T>
	static ucstring_view __vk_prop_name(const T &prop) = delete;

	template <>
	ucstring_view __vk_prop_name(const vk::LayerProperties &prop)
		{ return uc_str(prop.layerName); }

	template <>
	ucstring_view __vk_prop_name(const vk::ExtensionProperties &prop)
		{ return uc_str(prop.extensionName); }

	template <typename T>
	static const T *__vk_find_prop(const ucstring_view &name, const vector<T> &supported)
	{
		for(const T &it : supported) {
			if(__vk_prop_name<T>(it) == name) {
				return &it;
			}
		}
		return nullptr;
	}

	template <typename T>
	void __vk_remove_unsupported(vector<const ucchar_t *> &required, const vector<T> &supported, const function<void(const ucstring_view &)> &onremove = nullptr)
	{
		required.erase(remove_if(required.begin(), required.end(), 
			[supported, onremove](const ucstring_view &name) -> bool {
				if(__vk_find_prop(name, supported) == nullptr) {
					onremove(name);
					return true;
				}
				return false;
		}), required.end());
	}
}

#endif