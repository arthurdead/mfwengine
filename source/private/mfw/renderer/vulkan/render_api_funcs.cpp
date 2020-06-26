#include <private/mfw/renderer/vulkan/render_api_funcs.hpp>
#include <public/mfw/core/environment.hpp>
#include <private/mfw/renderer/display_api_funcs.hpp>
#include <public/mfw/stl/algorithm.hpp>
#include <private/mfw/renderer/vulkan/shader.hpp>

namespace mfw::renderer
{
	MFW_DECLARE_LOG_CONTEXT(log_render_api_vulkan, u8"renderer/render_api/vulkan"_p)

	namespace __render_api_funcs_internal
	{
		static VkBool32 MFW_CALL_SHARED debug_msg(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
		{
			if(bool_cast(messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) ||
				bool_cast(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)) {
				//return VK_TRUE;
			}

			ucstring msg{uc_str(pCallbackData->pMessage)};

			if(msg.find(u8"Device Extension: "_sv) != ucstring::npos) {
				return VK_TRUE;
			}

			if(bool_cast(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) ||
				bool_cast(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)) {
				log_render_api_vulkan().info(u8"{}"_sv, msg);
			} else if(bool_cast(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)) {
				log_render_api_vulkan().warning(u8"{}"_sv, msg);
			} else if(bool_cast(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)) {
				log_render_api_vulkan().error(u8"{}"_sv, msg);
			}

			return VK_TRUE;
		}

		static __MFW_ALLOC_PRE __MFW_ALIGN_ALLOC_POST(2, 3) void * MFW_CALL_SHARED alloc(void *pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) noexcept
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

		static __MFW_ALLOC_PRE __MFW_ALIGN_REALLOC_POST(3, 4) void * MFW_CALL_SHARED realloc(void *pUserData, void *pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) noexcept
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

		static void MFW_CALL_SHARED free(void *pUserData, void *pMemory) noexcept
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

		template <typename T>
		static ucstring_view prop_name(const T &prop) = delete;

		template <>
		ucstring_view prop_name(const vk::LayerProperties &prop) {
			return ucstring_view{uc_str(prop.layerName)};
		}

		template <>
		ucstring_view prop_name(const vk::ExtensionProperties &prop) {
			return ucstring_view{uc_str(prop.extensionName)};
		}

		template <typename T>
		static const T *find_prop(const ucstring_view &name, const vector<T> &supported)
		{
			for(const T &it : supported) {
				ucstring_view it_name{prop_name<T>(it)};
				if(it_name == name) {
					return &it;
				}
			}
			return nullptr;
		}

		template <typename T>
		void remove_unsupported(vector<const ucchar_t *> &required, const vector<T> &supported, const function<void(const ucstring_view &)> &onremove = nullptr)
		{
			required.erase(remove_if(required.begin(), required.end(), 
				[supported, onremove](const ucstring_view &name) -> bool {
					if(find_prop(name, supported) == nullptr) {
						onremove(name);
						return true;
					}
					return false;
				}
			), required.end());
		}

	#ifdef VK_NO_PROTOTYPES
		static vk::DynamicLoader &loader()
		{
			static vk::DynamicLoader __loader{};
			return __loader;
		}
	#endif

		static vk::AllocationCallbacks alloc_callbacks{};
	}

	namespace vulkan
	{
		vk::AllocationCallbacks &alloc_callbacks()
		{
			return __render_api_funcs_internal::alloc_callbacks;
		}
	}

	void render_api_funcs_vulkan::instance_layers(vector<const ucchar_t *> &required, bool *anyunsupported)
	{
		required.emplace_back(u8"VK_LAYER_NV_optimus");
	#if MFW_CONFIGURATION_IS(DEBUG)
		required.emplace_back(u8"VK_LAYER_KHRONOS_validation");
		required.emplace_back(u8"VK_LAYER_LUNARG_standard_validation");
		required.emplace_back(u8"VK_LAYER_GOOGLE_threading");
		required.emplace_back(u8"VK_LAYER_LUNARG_parameter_validation");
		required.emplace_back(u8"VK_LAYER_LUNARG_object_tracker");
		required.emplace_back(u8"VK_LAYER_LUNARG_core_validation");
		required.emplace_back(u8"VK_LAYER_GOOGLE_unique_objects");
	#endif

		vector<vk::LayerProperties> supported{MFW_VKRES(vk::enumerateInstanceLayerProperties())};

		__render_api_funcs_internal::remove_unsupported(required, supported,
			[anyunsupported](const ucstring_view &name) -> void {
				log_render_api_vulkan().warning(u8"{} not supported"_sv, name);
				if(anyunsupported) {
					*anyunsupported = true;
				}
			}
		);
	}

	void render_api_funcs_vulkan::instance_extensions(vector<const ucchar_t *> &required, bool *anyunsupported)
	{
		required.emplace_back(uc_str(VK_KHR_SURFACE_EXTENSION_NAME));
		required.emplace_back(uc_str(VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME));
		required.emplace_back(uc_str(VK_KHR_DISPLAY_EXTENSION_NAME));
	#if MFW_CONFIGURATION_IS(DEBUG)
		required.emplace_back(uc_str(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));
		required.emplace_back(uc_str(VK_EXT_DEBUG_REPORT_EXTENSION_NAME));
		required.emplace_back(uc_str(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME));
	#endif

		opaque_data data{};
		interfaces::display_api_funcs::instance().query_data(u8"vulkan_surface_ext_name"_sv, data);
		required.emplace_back(data.get<const ucchar_t *>());

		vector<vk::ExtensionProperties> supported{MFW_VKRES(vk::enumerateInstanceExtensionProperties())};

		__render_api_funcs_internal::remove_unsupported(required, supported,
			[anyunsupported](const ucstring_view &name) -> void {
				log_render_api_vulkan().warning(u8"{} not supported"_sv, name);
				if(anyunsupported) {
					*anyunsupported = true;
				}
			}
		);
	}

	bool render_api_funcs_vulkan::initialize()
	{
		/*
		core::environment_var envvar{u8"DISABLE_LAYER_NV_OPTIMUS_1"_sv};
		envvar.set();
		envvar.commit();
		*/

		shader_vulkan::compile_shaders();

	#ifdef VK_NO_PROTOTYPES
		PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr{__render_api_funcs_internal::loader().getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr")};
		VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
	#endif

		__render_api_funcs_internal::alloc_callbacks.setPUserData(nullptr);
		__render_api_funcs_internal::alloc_callbacks.setPfnAllocation(reinterpret_cast<PFN_vkAllocationFunction>(__render_api_funcs_internal::alloc));
		__render_api_funcs_internal::alloc_callbacks.setPfnReallocation(reinterpret_cast<PFN_vkReallocationFunction>(__render_api_funcs_internal::realloc));
		__render_api_funcs_internal::alloc_callbacks.setPfnFree(reinterpret_cast<PFN_vkFreeFunction>(__render_api_funcs_internal::free));
		__render_api_funcs_internal::alloc_callbacks.setPfnInternalAllocation(nullptr);
		__render_api_funcs_internal::alloc_callbacks.setPfnInternalFree(nullptr);

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
		instance_extensions(extensions_required);
		instinfo.setEnabledExtensionCount(static_cast<uint32_t>(extensions_required.size()));
		instinfo.setPpEnabledExtensionNames(reinterpret_cast<const char *const *const>(extensions_required.data()));

		vector<const ucchar_t *> layers_required{};
		instance_layers(layers_required);
		instinfo.setEnabledLayerCount(static_cast<uint32_t>(layers_required.size()));
		instinfo.setPpEnabledLayerNames(reinterpret_cast<const char *const *const>(layers_required.data()));

		instance = MFW_VKRES(vk::createInstanceUnique(move(instinfo), &__render_api_funcs_internal::alloc_callbacks));

	#ifdef VK_NO_PROTOTYPES
		VULKAN_HPP_DEFAULT_DISPATCHER.init(instance.get());
	#endif

	#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
		vk::DebugUtilsMessengerCreateInfoEXT dbgmsginfo{};
		dbgmsginfo.setPfnUserCallback(reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(__render_api_funcs_internal::debug_msg));
		dbgmsginfo.setPUserData(nullptr);
		dbgmsginfo.setMessageSeverity(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(vk::FlagTraits<vk::DebugUtilsMessageSeverityFlagBitsEXT>::allFlags));
		dbgmsginfo.setMessageType(static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(vk::FlagTraits<vk::DebugUtilsMessageTypeFlagBitsEXT>::allFlags));
		messenger = MFW_VKRES(instance->createDebugUtilsMessengerEXTUnique(move(dbgmsginfo), &__render_api_funcs_internal::alloc_callbacks));
	#endif

		vector<vk::PhysicalDevice> devices{MFW_VKRES(instance->enumeratePhysicalDevices())};

		for(const vk::PhysicalDevice &device : devices) {
			vk::PhysicalDeviceProperties dprop{};
			device.getProperties(&dprop);

			graphics_card::list_t &list{graphics_card::list()};
			graphics_card::list_t::iterator it{list.begin()};
			while(it != list.end()) {
				if(it->device_id() == dprop.deviceID &&
					it->vendor_id() == dprop.vendorID) {
					break;
				}
				it++;
			}

			graphics_card &gpu{*it};
			gpu_render_data_t &data{gpu.allocate_render_data<gpu_render_data_t>()};
			data.device = device;
		}

		return true;
	}

	void render_api_funcs_vulkan::create_window(window &win, const graphics_card &gpu, size_t w, size_t h)
	{
		opaque_data output{};
		opaque_data input{};
		vulkan::input_create_window_t &data{input.allocate<vulkan::input_create_window_t>()};
		data.instance = &instance;
		data.win = &win;
		interfaces::display_api_funcs::instance().query_data(u8"vulkan_create_surface"_sv, output, &input);
		vk::UniqueSurfaceKHR &surface{output.get<vk::UniqueSurfaceKHR>()};
		window_render_data_t &win_data{win.allocate_render_data<window_render_data_t>()};
		const gpu_render_data_t &gpu_data{gpu.get_render_data<gpu_render_data_t>()};
		win_data.setup(move(surface), gpu_data.device);
	}

	void render_api_funcs_vulkan::device_layers(vector<const ucchar_t *> &required, const vk::PhysicalDevice &device, bool *anyunsupported)
	{
	#if 0
		vector<vk::LayerProperties> supported{MFW_VKRES(device.enumerateDeviceLayerProperties())};

		__render_api_funcs_internal::remove_unsupported(required, supported,
			[anyunsupported](const ucstring_view &name) -> void {
				log_render_api_vulkan().warning(u8"{} not supported"_sv, name);
				if(anyunsupported) {
					*anyunsupported = true;
				}
			}
		);
	#endif
	}

	void render_api_funcs_vulkan::device_extensions(vector<const ucchar_t *> &required, const vk::PhysicalDevice &device, bool *anyunsupported)
	{
		required.emplace_back(uc_str(VK_KHR_SWAPCHAIN_EXTENSION_NAME));

		vector<vk::ExtensionProperties> supported{MFW_VKRES(device.enumerateDeviceExtensionProperties())};

		__render_api_funcs_internal::remove_unsupported(required, supported,
			[anyunsupported](const ucstring_view &name) -> void {
				log_render_api_vulkan().warning(u8"{} not supported"_sv, name);
				if(anyunsupported) {
					*anyunsupported = true;
				}
			}
		);
	}

	void render_api_funcs_vulkan::queues(queue_array &required, const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface)
	{
		vector<vk::QueueFamilyProperties> familyinfos{device.getQueueFamilyProperties()};

		for(uint32_t fi{0}; fi < familyinfos.size(); fi++) {
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

	render_api_funcs_vulkan::window_render_data_t::~window_render_data_t()
	{
		device_.device->waitIdle();

		swapchain_.reset();
		sync_.reset();
		pipeline_.reset();
		base_shader.reset();
		memory_.reset();
		device_.reset();
	}

	void render_api_funcs_vulkan::window_render_data_t::setup(vk::UniqueSurfaceKHR &&surface, const vk::PhysicalDevice &device)
	{
		device_.surface = move(surface);

		device_.physical_device = device;

		create_device(device_.physical_device);
		create_swapchain(device_.physical_device);
		base_shader.load_files(*device_.device);
		create_pipeline();
		create_memory_buffers();
		create_frame_buffers();
		create_sync();
	}

	vk::PresentModeKHR render_api_funcs_vulkan::window_render_data_t::present_mode(const vk::PhysicalDevice &device) const
	{
		vector<vk::PresentModeKHR> presentmodes{MFW_VKRES(device.getSurfacePresentModesKHR(*device_.surface))};

		vk::PresentModeKHR preferredmode{vk::PresentModeKHR::eFifo};

		for(vk::PresentModeKHR mode : presentmodes) {
			if(mode == vk::PresentModeKHR::eMailbox) {
				return mode;
			} else if(mode == vk::PresentModeKHR::eImmediate) {
				preferredmode = mode;
			}
		}

		return preferredmode;
	}

	vk::SurfaceFormatKHR render_api_funcs_vulkan::window_render_data_t::surface_format(const vk::PhysicalDevice &device) const
	{
		vector<vk::SurfaceFormatKHR> formats{MFW_VKRES(device.getSurfaceFormatsKHR(*device_.surface))};

		if(formats.size() == 1 && formats.cbegin()->format == vk::Format::eUndefined) {
			vk::SurfaceFormatKHR format{};
			format.format = vk::Format::eB8G8R8A8Unorm;
			format.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
			return format;
		}

		for(const vk::SurfaceFormatKHR &format : formats) {
			if(format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				return format;
			}
		}

		return *formats.begin();
	}

	void render_api_funcs_vulkan::window_render_data_t::create_swapchain(const vk::PhysicalDevice &device)
	{
		vk::SurfaceCapabilitiesKHR capabilities{};
		device.getSurfaceCapabilitiesKHR(*device_.surface, &capabilities);

		vk::Extent2D currentExtent{capabilities.currentExtent};
		//currentExtent.setWidth(max(currentExtent.width, static_cast<uint32_t>(MFW_WINDOW_MINIMAL_WIDTH)));
		//currentExtent.setHeight(max(currentExtent.height, static_cast<uint32_t>(MFW_WINDOW_MINIMAL_HEIGHT)));

		vk::Extent2D minExtent{capabilities.minImageExtent};
		//minExtent.setWidth(max(minExtent.width, static_cast<uint32_t>(MFW_WINDOW_MINIMAL_WIDTH)));
		//minExtent.setHeight(max(minExtent.height, static_cast<uint32_t>(MFW_WINDOW_MINIMAL_HEIGHT)));

		vk::Extent2D maxExtent{capabilities.maxImageExtent};

		//int32_t mw{0}; int32_t mh{0};
		//get_monitor_bounds(nullptr, nullptr, &mw, &mh);

		//maxExtent.setWidth(max(maxExtent.width, static_cast<uint32_t>(mw)));
		//maxExtent.setHeight(max(maxExtent.height, static_cast<uint32_t>(mh)));

		vk::PresentModeKHR mode{present_mode(device)};
		vk::SurfaceFormatKHR format{surface_format(device)};

		swapchain_.format = format;

		vk::SwapchainCreateInfoKHR swapinfo{};
		swapinfo.setSurface(*device_.surface);
		swapinfo.setOldSwapchain(*swapchain_.swapchain);

		uint32_t imageCount{capabilities.minImageCount + 1};
		if((capabilities.maxImageCount > 0) && (imageCount > capabilities.maxImageCount)) {
			imageCount = capabilities.maxImageCount;
		}

		vk::Extent2D extent{};
		if(currentExtent.width != UINT32_MAX) {
			extent = currentExtent;
		} else {
			int32_t w{0}; int32_t h{0};
			//get_bounds(nullptr, nullptr, &w, &h);
			//w = max(w, MFW_WINDOW_MINIMAL_WIDTH);
			//h = max(h, MFW_WINDOW_MINIMAL_HEIGHT);

			extent.setWidth(static_cast<uint32_t>(w));
			extent.setHeight(static_cast<uint32_t>(h));

			extent.setWidth(max(minExtent.width, min(maxExtent.width, extent.width)));
			extent.setHeight(max(minExtent.height, min(maxExtent.height, extent.height)));
		}

		swapchain_.extent = extent;

		swapinfo.setMinImageCount(imageCount);
		swapinfo.setImageExtent(extent);
		swapinfo.setImageArrayLayers(1);
		swapinfo.setPreTransform(capabilities.currentTransform);
		swapinfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
		swapinfo.setClipped(true);

		queue_array::queue_indexes_t indexes{device_.queues.indexes()};
		if(indexes[queue_type::graphics] != indexes[queue_type::present]) {
			swapinfo.setImageSharingMode(vk::SharingMode::eConcurrent);
		} else {
			swapinfo.setImageSharingMode(vk::SharingMode::eExclusive);
		}
		swapinfo.setPQueueFamilyIndices(indexes.data());
		swapinfo.setQueueFamilyIndexCount(static_cast<uint32_t>(indexes.size()));

		swapinfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

		swapinfo.setImageColorSpace(format.colorSpace);
		swapinfo.setImageFormat(format.format);

		swapinfo.setPresentMode(mode);

		swapchain_.swapchain = MFW_VKRES(device_.device->createSwapchainKHRUnique(move(swapinfo), &__render_api_funcs_internal::alloc_callbacks));

		vector<vk::Image> images{MFW_VKRES(device_.device->getSwapchainImagesKHR(*swapchain_.swapchain))};

		for(const vk::Image &img : images) {

			swapchain_t::image_t vkimg{};
			vkimg.image = img;

			vk::ImageViewCreateInfo info{};
			info.setImage(vkimg.image);
			info.setViewType(vk::ImageViewType::e2D);

			vk::ComponentMapping components{};
			components.setR(vk::ComponentSwizzle::eIdentity);
			components.setG(vk::ComponentSwizzle::eIdentity);
			components.setB(vk::ComponentSwizzle::eIdentity);
			components.setA(vk::ComponentSwizzle::eIdentity);
			info.setComponents(move(components));

			vk::ImageSubresourceRange range{};
			range.setAspectMask(vk::ImageAspectFlagBits::eColor);
			range.setBaseArrayLayer(0);
			range.setBaseMipLevel(0);
			range.setLayerCount(1);
			range.setLevelCount(1);
			info.setSubresourceRange(move(range));

			info.setFormat(swapchain_.format.format);

			vkimg.view = MFW_VKRES(device_.device->createImageViewUnique(move(info), &__render_api_funcs_internal::alloc_callbacks));

			swapchain_.images.emplace_back(move(vkimg));
		}
	}

	void render_api_funcs_vulkan::window_render_data_t::create_device(const vk::PhysicalDevice &device)
	{
		vk::DeviceCreateInfo deviceinfo{};

		vk::PhysicalDeviceFeatures features{};
		device.getFeatures(&features);

		deviceinfo.setPEnabledFeatures(&features);

		vector<const ucchar_t *> extensions{};
		device_extensions(extensions, device);
		deviceinfo.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()));
		deviceinfo.setPpEnabledExtensionNames(reinterpret_cast<const char *const *const>(extensions.data()));

		vector<const ucchar_t *> layers{};
		device_layers(layers, device);
		deviceinfo.setEnabledLayerCount(static_cast<uint32_t>(layers.size()));
		deviceinfo.setPpEnabledLayerNames(reinterpret_cast<const char *const *const>(layers.data()));

		queues(device_.queues, device, *device_.surface);

		vector<vk::DeviceQueueCreateInfo> queueinfos{};

		for(const queue_family_t &fi : device_.queues) {
			if(fi.index == VK_QUEUE_FAMILY_IGNORED) {
				continue;
			}

			vk::DeviceQueueCreateInfo &queueinfo{queueinfos.emplace_back()};

			queueinfo.setQueueCount(1);
			queueinfo.setQueueFamilyIndex(fi.index);

			constexpr float32_t prio{1.0f};
			queueinfo.setPQueuePriorities(&prio);
		}

		deviceinfo.setPQueueCreateInfos(queueinfos.data());
		deviceinfo.setQueueCreateInfoCount(static_cast<uint32_t>(queueinfos.size()));

		device_.device = MFW_VKRES(device.createDeviceUnique(move(deviceinfo), &__render_api_funcs_internal::alloc_callbacks));

		queue_family_t &graphics{device_.queues.graphics()};
		device_.device->getQueue(graphics.index, 0, &graphics.queue);

		queue_family_t &present{device_.queues.present()};
		device_.device->getQueue(present.index, 0, &present.queue);
	}

	void render_api_funcs_vulkan::window_render_data_t::create_pipeline()
	{
		vk::GraphicsPipelineCreateInfo pipelineinfo{};

		vk::PipelineVertexInputStateCreateInfo vertexstate{};

		/*
		array<vk::VertexInputAttributeDescription, 2> attribdesc{mesh::attribute_descriptions()};
		vertexstate.setVertexAttributeDescriptionCount(static_cast<uint32_t>(attribdesc.size()));
		vertexstate.setPVertexAttributeDescriptions(attribdesc.data());

		vk::VertexInputBindingDescription binddesc{mesh::binding_description()};
		vertexstate.setVertexBindingDescriptionCount(1);
		vertexstate.setPVertexBindingDescriptions(&binddesc);
		*/

		pipelineinfo.setPVertexInputState(&vertexstate);

		vk::PipelineInputAssemblyStateCreateInfo assemblystate{};
		assemblystate.setPrimitiveRestartEnable(false);
		assemblystate.setTopology(vk::PrimitiveTopology::eTriangleList);
		pipelineinfo.setPInputAssemblyState(&assemblystate);

		vk::Viewport viewport{};
		viewport.setX(0.0f);
		viewport.setY(0.0f);
		viewport.setHeight(static_cast<float32_t>(swapchain_.extent.height));
		viewport.setWidth(static_cast<float32_t>(swapchain_.extent.width));
		viewport.setMinDepth(0.0f);
		viewport.setMaxDepth(1.0f);

		vk::Rect2D scissor{};
		vk::Offset2D offset{};
		offset.setX(0);
		offset.setY(0);
		scissor.setOffset(move(offset));
		scissor.setExtent(swapchain_.extent);

		vk::PipelineViewportStateCreateInfo viewportstate{};
		viewportstate.setViewportCount(1);
		viewportstate.setScissorCount(1);
		viewportstate.setPViewports(&viewport);
		viewportstate.setPScissors(&scissor);
		pipelineinfo.setPViewportState(&viewportstate);

		vk::PipelineRasterizationStateCreateInfo rasterizerstate{};
		rasterizerstate.setCullMode(vk::CullModeFlagBits::eBack);
		rasterizerstate.setDepthClampEnable(false);
		rasterizerstate.setDepthBiasEnable(false);
		rasterizerstate.setDepthBiasConstantFactor(0.0f);
		rasterizerstate.setDepthBiasClamp(0.0f);
		rasterizerstate.setDepthBiasSlopeFactor(0.0f);
		rasterizerstate.setRasterizerDiscardEnable(false);
		rasterizerstate.setPolygonMode(vk::PolygonMode::eFill);
		rasterizerstate.setLineWidth(1.0f);
		rasterizerstate.setFrontFace(vk::FrontFace::eClockwise);
		pipelineinfo.setPRasterizationState(&rasterizerstate);

		vk::PipelineMultisampleStateCreateInfo multisamplingstate{};
		multisamplingstate.setSampleShadingEnable(false);
		multisamplingstate.setRasterizationSamples(vk::SampleCountFlagBits::e1);
		multisamplingstate.setMinSampleShading(1.0f);
		multisamplingstate.setPSampleMask(nullptr);
		multisamplingstate.setAlphaToCoverageEnable(false);
		multisamplingstate.setAlphaToOneEnable(false);
		pipelineinfo.setPMultisampleState(&multisamplingstate);

		vk::PipelineColorBlendAttachmentState colorattachstate{};
		colorattachstate.setBlendEnable(false);
		colorattachstate.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
		colorattachstate.setSrcColorBlendFactor(vk::BlendFactor::eOne);
		colorattachstate.setDstColorBlendFactor(vk::BlendFactor::eZero);
		colorattachstate.setColorBlendOp(vk::BlendOp::eAdd);
		colorattachstate.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
		colorattachstate.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
		colorattachstate.setAlphaBlendOp(vk::BlendOp::eAdd);

		vk::PipelineColorBlendStateCreateInfo colorblendstate{};
		colorblendstate.setLogicOpEnable(false);
		colorblendstate.setLogicOp(vk::LogicOp::eCopy);
		colorblendstate.setAttachmentCount(1);
		colorblendstate.setPAttachments(&colorattachstate);
		colorblendstate.setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});
		pipelineinfo.setPColorBlendState(&colorblendstate);

		pipelineinfo.setBasePipelineHandle(nullptr);

		vk::PipelineLayoutCreateInfo layoutinfo{};
		layoutinfo.setSetLayoutCount(0);
		layoutinfo.setPushConstantRangeCount(0);

		pipeline_.layout = MFW_VKRES(device_.device->createPipelineLayoutUnique(move(layoutinfo), &__render_api_funcs_internal::alloc_callbacks));

		pipelineinfo.setLayout(*pipeline_.layout);

		vk::RenderPassCreateInfo passinfo{};
		passinfo.setAttachmentCount(1);

		vk::AttachmentDescription attachdesc{};
		attachdesc.setFormat(swapchain_.format.format);
		attachdesc.setSamples(vk::SampleCountFlagBits::e1);
		attachdesc.setLoadOp(vk::AttachmentLoadOp::eClear);
		attachdesc.setStoreOp(vk::AttachmentStoreOp::eStore);
		attachdesc.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
		attachdesc.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
		attachdesc.setInitialLayout(vk::ImageLayout::eUndefined);
		attachdesc.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
		passinfo.setPAttachments(&attachdesc);

		passinfo.setSubpassCount(1);

		vk::SubpassDescription subpasdesc{};
		subpasdesc.setColorAttachmentCount(1);

		vk::AttachmentReference colorattach{};
		colorattach.setLayout(vk::ImageLayout::eColorAttachmentOptimal);
		subpasdesc.setPColorAttachments(&colorattach);

		passinfo.setPSubpasses(&subpasdesc);

		pipeline_.renderpass = MFW_VKRES(device_.device->createRenderPassUnique(move(passinfo), &__render_api_funcs_internal::alloc_callbacks));

		pipelineinfo.setRenderPass(*pipeline_.renderpass);
		pipelineinfo.setSubpass(0);

		shader_vulkan::stages_t stages{base_shader.stages()};
		pipelineinfo.setStageCount(static_cast<uint32_t>(stages.size()));
		pipelineinfo.setPStages(stages.data());

		pipeline_.pipeline = MFW_VKRES(device_.device->createGraphicsPipelineUnique(nullptr, move(pipelineinfo), &__render_api_funcs_internal::alloc_callbacks));
	}

	void render_api_funcs_vulkan::window_render_data_t::create_frame_buffers()
	{
		for(const swapchain_t::image_t &it : swapchain_.images) {
			vk::FramebufferCreateInfo framebufferinfo{};
			framebufferinfo.setRenderPass(*pipeline_.renderpass);
			framebufferinfo.setAttachmentCount(1);
			framebufferinfo.setPAttachments(&(*it.view));
			framebufferinfo.setWidth(swapchain_.extent.width);
			framebufferinfo.setHeight(swapchain_.extent.height);
			framebufferinfo.setLayers(1);
			swapchain_.framebuffers.emplace_back(MFW_VKRES(device_.device->createFramebufferUnique(move(framebufferinfo), &__render_api_funcs_internal::alloc_callbacks)));
		}

		vk::CommandBufferAllocateInfo bufalloc{};
		bufalloc.setCommandPool(*memory_.cmdpool);
		bufalloc.setCommandBufferCount(static_cast<uint32_t>(swapchain_.framebuffers.size()));
		bufalloc.setLevel(vk::CommandBufferLevel::ePrimary);
		swapchain_.cmdbuffers = MFW_VKRES(device_.device->allocateCommandBuffersUnique(move(bufalloc)));

		for(size_t i{0}; i < swapchain_.framebuffers.size(); i++) {
			const vk::CommandBuffer &buffer{*swapchain_.cmdbuffers[i]};

			vk::CommandBufferBeginInfo bufbegin{};
			bufbegin.setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
			buffer.begin(&bufbegin);

			vk::RenderPassBeginInfo renderpass{};
			renderpass.setRenderPass(*pipeline_.renderpass);
			renderpass.setFramebuffer(*swapchain_.framebuffers[i]);

			vk::Rect2D area{};
			vk::Offset2D offset{};
			offset.setX(0);
			offset.setY(0);
			area.setOffset(move(offset));
			area.setExtent(swapchain_.extent);
			renderpass.setRenderArea(move(area));

			vk::ClearValue clear{};
			vk::ClearColorValue color{};
			color.setFloat32({0.0, 0.0, 0.0, 1.0f});
			clear.setColor(move(color));

			renderpass.setClearValueCount(1);
			renderpass.setPClearValues(&clear);

			buffer.beginRenderPass(&renderpass, vk::SubpassContents::eInline);

			buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline_.pipeline);

			vk::DeviceSize dvsize{0};
			buffer.bindVertexBuffers(0, 1, &(*memory_.vertex.buffer), &dvsize);
			buffer.bindIndexBuffer((*memory_.index.buffer), 0, vk::IndexType::eUint16);

			buffer.drawIndexed(static_cast<uint32_t>(memory_.index.indices.size()), 1, 0, 0, 0);

			buffer.endRenderPass();

			buffer.end();
		}
	}

	void render_api_funcs_vulkan::window_render_data_t::create_sync()
	{
		for(size_t i{0}; i < sync_t::MAX_FRAMES; i++) {
			vk::SemaphoreCreateInfo semaphoreinfo{};

			sync_.images[i] = MFW_VKRES(device_.device->createSemaphoreUnique(move(semaphoreinfo), &__render_api_funcs_internal::alloc_callbacks));
			sync_.renders[i] = MFW_VKRES(device_.device->createSemaphoreUnique(move(semaphoreinfo), &__render_api_funcs_internal::alloc_callbacks));

			vk::FenceCreateInfo fenceinfo{};
			fenceinfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

			sync_.fences[i] = MFW_VKRES(device_.device->createFenceUnique(move(fenceinfo), &__render_api_funcs_internal::alloc_callbacks));
		}
	}

	void render_api_funcs_vulkan::window_render_data_t::create_buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memoryflags, vk::UniqueBuffer &buffer, vk::UniqueDeviceMemory &memory)
	{
		vk::BufferCreateInfo buffercreate{};
		buffercreate.setSize(static_cast<uint32_t>(size));
		buffercreate.setUsage(usage);
		buffercreate.setSharingMode(vk::SharingMode::eExclusive);
		buffer = MFW_VKRES(device_.device->createBufferUnique(move(buffercreate), &__render_api_funcs_internal::alloc_callbacks));

		vk::MemoryRequirements required{};
		device_.device->getBufferMemoryRequirements(*buffer, &required);

		vk::MemoryAllocateInfo allocinfo{};
		allocinfo.setAllocationSize(required.size);

		vk::PhysicalDeviceMemoryProperties props{};
		device_.physical_device.getMemoryProperties(&props);

		for(uint32_t i{0}; i < props.memoryTypeCount; i++) {
			if(bool_cast(required.memoryTypeBits & (1 << i)) && bool_cast(props.memoryTypes[i].propertyFlags & memoryflags)) {
				allocinfo.setMemoryTypeIndex(i);
				break;
			}
		}

		memory = MFW_VKRES(device_.device->allocateMemoryUnique(move(allocinfo), &__render_api_funcs_internal::alloc_callbacks));

		device_.device->bindBufferMemory(*buffer, *memory, 0);
	}

	void render_api_funcs_vulkan::window_render_data_t::begin_single_tile_commands(vk::UniqueCommandBuffer &cmdbuffer)
	{
		vk::CommandBufferAllocateInfo cmdinfo{};
		cmdinfo.setLevel(vk::CommandBufferLevel::ePrimary);
		cmdinfo.setCommandBufferCount(1);
		cmdinfo.setCommandPool(*memory_.cmdpool);

		cmdbuffer = move(MFW_VKRES(device_.device->allocateCommandBuffersUnique(move(cmdinfo)))[0]);

		vk::CommandBufferBeginInfo begininfo{};
		begininfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		cmdbuffer->begin(&begininfo);
	}

	void render_api_funcs_vulkan::window_render_data_t::end_single_tile_commands(vk::UniqueCommandBuffer &cmdbuffer)
	{
		cmdbuffer->end();

		vk::SubmitInfo submitinfo{};
		submitinfo.setCommandBufferCount(1);
		submitinfo.setPCommandBuffers(&(*cmdbuffer));

		device_.queues.graphics().queue.submit(1, &submitinfo, nullptr);

		device_.queues.graphics().queue.waitIdle();
	}

	void render_api_funcs_vulkan::window_render_data_t::copy_buffer(vk::UniqueBuffer &src, vk::UniqueBuffer &dst, size_t size)
	{
		vk::UniqueCommandBuffer cmdbuffer{};
		begin_single_tile_commands(cmdbuffer);

		vk::BufferCopy buffcopy{};
		buffcopy.setSize(static_cast<vk::DeviceSize>(size));
		cmdbuffer->copyBuffer(*src, *dst, 1, &buffcopy);

		end_single_tile_commands(cmdbuffer);
	}

	void render_api_funcs_vulkan::window_render_data_t::create_vertex_buffer()
	{
		vk::UniqueBuffer stagingbuffer{};
		vk::UniqueDeviceMemory stagingmemory{};

		/*
		size_t vertisize{sizeof(mesh) * memory_.vertex.vertices.size()};
		create_buffer(vertisize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingbuffer, stagingmemory);
		*/

		void *data{nullptr};
		//device_.device->mapMemory(stagingmemory.get(), 0, static_cast<vk::DeviceSize>(vertisize), vk::MemoryMapFlags{0}, &data);
		//memcpy(data, memory_.vertex.vertices.data(), vertisize);
		device_.device->unmapMemory(*stagingmemory);

		//create_buffer(vertisize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, memory_.vertex.buffer, memory_.vertex.memory);

		//copy_buffer(stagingbuffer, memory_.vertex.buffer, vertisize);
	}

	void render_api_funcs_vulkan::window_render_data_t::create_index_buffer()
	{
		vk::UniqueBuffer stagingbuffer{};
		vk::UniqueDeviceMemory stagingmemory{};

		size_t indisize{sizeof(uint16_t) * memory_.index.indices.size()};
		create_buffer(indisize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingbuffer, stagingmemory);

		void *data{nullptr};
		device_.device->mapMemory(stagingmemory.get(), 0, static_cast<vk::DeviceSize>(indisize), vk::MemoryMapFlags{0}, &data);
		memcpy(data, memory_.index.indices.data(), indisize);
		device_.device->unmapMemory(*stagingmemory);

		create_buffer(indisize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, memory_.index.buffer, memory_.index.memory);

		copy_buffer(stagingbuffer, memory_.index.buffer, indisize);
	}

	void render_api_funcs_vulkan::window_render_data_t::create_memory_buffers()
	{
		/*_memory._vertex._vertices = {
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
		};
		_memory._index._indices = {
			0, 1, 2, 2, 3, 0
		};*/

		vk::CommandPoolCreateInfo cmdpool{};
		cmdpool.setQueueFamilyIndex(device_.queues.graphics().index);
		memory_.cmdpool = MFW_VKRES(device_.device->createCommandPoolUnique(move(cmdpool), &__render_api_funcs_internal::alloc_callbacks));

		create_vertex_buffer();
		create_index_buffer();
	}

	void render_api_funcs_vulkan::window_render_data_t::recreate_swapchain()
	{
		device_.device->waitIdle();

		pipeline_.reset();
		swapchain_.reset();

		create_swapchain(device_.physical_device);
		create_pipeline();
		create_frame_buffers();
	}
}