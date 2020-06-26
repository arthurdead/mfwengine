#ifndef __MFW_PRIVATE_RENDERER_VULKAN_VULKAN_H
#define __MFW_PRIVATE_RENDERER_VULKAN_VULKAN_H

#pragma once

#ifdef VULKAN_HPP
	#error
#endif

#ifdef __MFW_PRIVATE_RENDERER_VULKAN_SHARED_H
	#error
#endif

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/defines.hpp>

#include <public/mfw/stl/algorithm.hpp>
#include <public/mfw/stl/array.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/functional.hpp>
#include <public/mfw/stl/initializer_list.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/system_error.hpp>
#include <public/mfw/stl/tuple.hpp>
#include <public/mfw/stl/type_traits.hpp>
#include <public/mfw/stl/memory.hpp>
#include <public/mfw/stl/vector.hpp>
#include <cassert>
#include <compare>

#define VK_NO_PROTOTYPES

#ifdef VK_NO_PROTOTYPES
	#if MFW_OS_IS(LINUX)
		#define VK_USE_PLATFORM_XCB_KHR
		#define VK_USE_PLATFORM_XLIB_KHR
		#define VK_USE_PLATFORM_WAYLAND_KHR
	#endif
	#if MFW_OS_IS(WINDOWS)
		#define VK_USE_PLATFORM_WIN32_KHR
	#endif
#endif

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
	#include <wayland-client.h>
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
	#include <windows.h>
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
	#include <xcb/xcb.h>
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
	#include <X11/Xlib.h>
#endif
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
	#include <X11/Xlib.h>
	#include <X11/extensions/Xrandr.h>
#endif

#if MFW_CONFIGURATION_IS(RELEASE)
	#define VULKAN_HPP_NO_EXCEPTIONS
	#define VULKAN_HPP_ASSERT (void)
#else
	#define VULKAN_HPP_ASSERT assert
#endif

#define module module_
#pragma push_macro("free")
#undef free
#if MFW_COMPILER_IS(MSVC)
	MFW_WARNING_PUSH()
	MFW_WARNING_DISABLE(4061 4582 4191)
	MFW_WARNING_DISABLE(4458)
#endif
namespace __vulkan_std_namespace
{
	using namespace ::mfw::stl;

	template <typename T>
	struct is_error_code_enum : public ::mfw::stl::is_error_code_enum<T>
	{
	};

	template <typename T>
	struct hash : public ::mfw::stl::hash<T>
	{
	};
}
#define std __vulkan_std_namespace
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>
#ifdef VK_USE_PLATFORM_ANDROID_KHR
	#include <vulkan/vulkan_android.h>
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
	#include <vulkan/vulkan_wayland.h>
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
	#include <vulkan/vulkan_win32.h>
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
	#include <vulkan/vulkan_xcb.h>
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
	#include <vulkan/vulkan_xlib.h>
#endif
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
	#include <vulkan/vulkan_xlib_xrandr.h>
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
	#include <vulkan/vulkan_beta.h>
#endif
#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>
#undef std
namespace MFW_STD_NAMESPACE
{
	#define __MFW_VULKAN_STD_REMAP_CLASS(name) \
		template <> \
		struct name : public __vulkan_std_namespace::name {};

	__MFW_VULKAN_STD_REMAP_CLASS(is_error_code_enum<VULKAN_HPP_NAMESPACE::Result>)

	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::AccelerationStructureKHR>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::Buffer>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::BufferView>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::CommandBuffer>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::CommandPool>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::DebugReportCallbackEXT>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::DebugUtilsMessengerEXT>)
#ifdef VK_ENABLE_BETA_EXTENSIONS
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::DeferredOperationKHR>)
#endif
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::DescriptorPool>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::DescriptorSet>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::DescriptorSetLayout>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::DescriptorUpdateTemplate>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::Device>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::DeviceMemory>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::DisplayKHR>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::DisplayModeKHR>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::Event>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::Fence>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::Framebuffer>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::Image>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::ImageView>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::IndirectCommandsLayoutNV>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::Instance>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::PerformanceConfigurationINTEL>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::PhysicalDevice>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::Pipeline>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::PipelineCache>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::PipelineLayout>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::PrivateDataSlotEXT>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::QueryPool>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::Queue>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::RenderPass>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::Sampler>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::SamplerYcbcrConversion>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::Semaphore>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::ShaderModule>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::SurfaceKHR>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::SwapchainKHR>)
	__MFW_VULKAN_STD_REMAP_CLASS(hash<vk::ValidationCacheEXT>)
}
#if MFW_COMPILER_IS(MSVC)
	MFW_WARNING_POP()
#endif
#pragma pop_macro("free")
#undef module

#ifdef VULKAN_HPP_NO_EXCEPTIONS
	#define MFW_VKRES(result) ((result).value)
#else
	#define MFW_VKRES(result) (result)
#endif

#endif