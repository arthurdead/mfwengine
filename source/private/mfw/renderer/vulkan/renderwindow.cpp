#include <private/mfw/renderer/vulkan/renderwindow.hpp>
#include <private/mfw/renderer/vulkan/renderer.hpp>
#if MFW_OS == MFW_OS_WINDOWS
	#include <Windows.h>
#endif

namespace mfw::renderer::vulkan
{
	renderwindow::renderwindow(const gpu *gpu_, const monitor *mon)
		: agnostic::renderwindow{mon}
	{
		vk::Win32SurfaceCreateInfoKHR win32info{};
		win32info.setHinstance(instance());
		win32info.setHwnd(handle());

		device_.surface = MFW_VKRES(renderer::instance().vulkan_instance().createWin32SurfaceKHRUnique(move(win32info), &__vk_alloc_callbacks()));

		device_.physical_device = gpu_->physical_device();

		create_device(device_.physical_device);
		create_swapchain(device_.physical_device);
		base_shader.load_files(*device_.device);
		create_pipeline();
		create_memory_buffers();
		create_frame_buffers();
		create_sync();
	}

	renderwindow::~renderwindow()
	{
		device_.device->waitIdle();

		swapchain_.reset();
		sync_.reset();
		pipeline_.reset();
		base_shader.reset();
		memory_.reset();
		device_.reset();
	}

	vk::PresentModeKHR renderwindow::present_mode(const vk::PhysicalDevice &device) const
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

	vk::SurfaceFormatKHR renderwindow::surface_format(const vk::PhysicalDevice &device) const
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

	void renderwindow::create_swapchain(const vk::PhysicalDevice &device)
	{
		vk::SurfaceCapabilitiesKHR capabilities{};
		device.getSurfaceCapabilitiesKHR(*device_.surface, &capabilities);

		vk::Extent2D currentExtent{capabilities.currentExtent};
		//currentExtent.setWidth(max(currentExtent.width, scast<const uint32>(WINDOW_MINIMAL_WIDTH)));
		//currentExtent.setHeight(max(currentExtent.height, scast<const uint32>(WINDOW_MINIMAL_HEIGHT)));

		vk::Extent2D minExtent{capabilities.minImageExtent};
		//minExtent.setWidth(max(minExtent.width, scast<const uint32>(WINDOW_MINIMAL_WIDTH)));
		//minExtent.setHeight(max(minExtent.height, scast<const uint32>(WINDOW_MINIMAL_HEIGHT)));

		vk::Extent2D maxExtent{capabilities.maxImageExtent};

		//int32 mw{0}; int32 mh{0};
		//GetMonitorBounds(nullptr, nullptr, &mw, &mh);

		//maxExtent.setWidth(max(maxExtent.width, scast<const uint32>(mw)));
		//maxExtent.setHeight(max(maxExtent.height, scast<const uint32>(mh)));

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
			get_bounds(nullptr, nullptr, &w, &h);
			//w = max(w, WINDOW_MINIMAL_WIDTH);
			//h = max(h, WINDOW_MINIMAL_HEIGHT);

			extent.setWidth(scast<uint32_t>(w));
			extent.setHeight(scast<uint32_t>(h));

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

		gpu::queue_array::queue_indexes_t indexes{device_.queues.indexes()};
		if(indexes[gpu::queue_type::graphics] != indexes[gpu::queue_type::present]) {
			swapinfo.setImageSharingMode(vk::SharingMode::eConcurrent);
		} else {
			swapinfo.setImageSharingMode(vk::SharingMode::eExclusive);
		}
		swapinfo.setPQueueFamilyIndices(indexes.data());
		swapinfo.setQueueFamilyIndexCount(scast<uint32_t>(indexes.size()));

		swapinfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

		swapinfo.setImageColorSpace(format.colorSpace);
		swapinfo.setImageFormat(format.format);

		swapinfo.setPresentMode(mode);

		swapchain_.swapchain = MFW_VKRES(device_.device->createSwapchainKHRUnique(move(swapinfo), &__vk_alloc_callbacks()));

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

			vkimg.view = MFW_VKRES(device_.device->createImageViewUnique(move(info), &__vk_alloc_callbacks()));

			swapchain_.images.push_back(move(vkimg));
		}
	}

	void renderwindow::create_device(const vk::PhysicalDevice &device)
	{
		vk::DeviceCreateInfo deviceinfo{};

		vk::PhysicalDeviceFeatures features{};
		device.getFeatures(&features);

		deviceinfo.setPEnabledFeatures(&features);

		vector<const char8_t *> extensions{};
		gpu::extensions(extensions, device);
		deviceinfo.setEnabledExtensionCount(scast<uint32_t>(extensions.size()));
		deviceinfo.setPpEnabledExtensionNames(rcast<const char *const *>(extensions.data()));

		vector<const char8_t *> layers{};
		gpu::layers(layers, device);
		deviceinfo.setEnabledLayerCount(scast<uint32_t>(layers.size()));
		deviceinfo.setPpEnabledLayerNames(rcast<const char *const *>(layers.data()));

		gpu::queues(device_.queues, device, *device_.surface);

		vector<vk::DeviceQueueCreateInfo> queueinfos{};

		for(const gpu::queue_family_t &fi : device_.queues) {
			if(fi.index == VK_QUEUE_FAMILY_IGNORED) {
				continue;
			}

			vk::DeviceQueueCreateInfo &queueinfo{queueinfos.emplace_back()};

			queueinfo.setQueueCount(1);
			queueinfo.setQueueFamilyIndex(fi.index);

			const float32_t prio{1.0f};
			queueinfo.setPQueuePriorities(&prio);
		}

		deviceinfo.setPQueueCreateInfos(queueinfos.data());
		deviceinfo.setQueueCreateInfoCount(scast<uint32_t>(queueinfos.size()));

		device_.device = MFW_VKRES(device.createDeviceUnique(move(deviceinfo), &__vk_alloc_callbacks()));

		gpu::queue_family_t &graphics{device_.queues.graphics()};
		device_.device->getQueue(graphics.index, 0, &graphics.queue);

		gpu::queue_family_t &present{device_.queues.present()};
		device_.device->getQueue(present.index, 0, &present.queue);
	}

	void renderwindow::create_pipeline()
	{
		vk::GraphicsPipelineCreateInfo pipelineinfo{};

		vk::PipelineVertexInputStateCreateInfo vertexstate{};

		array<vk::VertexInputAttributeDescription, 2> attribdesc{mesh::attribute_descriptions()};
		vertexstate.setVertexAttributeDescriptionCount(scast<uint32_t>(attribdesc.size()));
		vertexstate.setPVertexAttributeDescriptions(attribdesc.data());

		vk::VertexInputBindingDescription binddesc{mesh::binding_description()};
		vertexstate.setVertexBindingDescriptionCount(1);
		vertexstate.setPVertexBindingDescriptions(&binddesc);

		pipelineinfo.setPVertexInputState(&vertexstate);

		vk::PipelineInputAssemblyStateCreateInfo assemblystate{};
		assemblystate.setPrimitiveRestartEnable(false);
		assemblystate.setTopology(vk::PrimitiveTopology::eTriangleList);
		pipelineinfo.setPInputAssemblyState(&assemblystate);

		vk::Viewport viewport{};
		viewport.setX(0.0f);
		viewport.setY(0.0f);
		viewport.setHeight(scast<float32_t>(swapchain_.extent.height));
		viewport.setWidth(scast<float32_t>(swapchain_.extent.width));
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

		pipeline_.layout = MFW_VKRES(device_.device->createPipelineLayoutUnique(move(layoutinfo), &__vk_alloc_callbacks()));

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

		pipeline_.renderpass = MFW_VKRES(device_.device->createRenderPassUnique(move(passinfo), &__vk_alloc_callbacks()));

		pipelineinfo.setRenderPass(*pipeline_.renderpass);
		pipelineinfo.setSubpass(0);

		shader::stages_t stages{base_shader.stages()};
		pipelineinfo.setStageCount(scast<uint32_t>(stages.size()));
		pipelineinfo.setPStages(stages.data());

		pipeline_.pipeline = MFW_VKRES(device_.device->createGraphicsPipelineUnique(nullptr, move(pipelineinfo), &__vk_alloc_callbacks()));
	}

	void renderwindow::create_frame_buffers()
	{
		for(const swapchain_t::image_t &it : swapchain_.images) {
			vk::FramebufferCreateInfo framebufferinfo{};
			framebufferinfo.setRenderPass(*pipeline_.renderpass);
			framebufferinfo.setAttachmentCount(1);
			framebufferinfo.setPAttachments(&(*it.view));
			framebufferinfo.setWidth(swapchain_.extent.width);
			framebufferinfo.setHeight(swapchain_.extent.height);
			framebufferinfo.setLayers(1);
			swapchain_.framebuffers.emplace_back(MFW_VKRES(device_.device->createFramebufferUnique(move(framebufferinfo), &__vk_alloc_callbacks())));
		}

		vk::CommandBufferAllocateInfo bufalloc{};
		bufalloc.setCommandPool(*memory_.cmdpool);
		bufalloc.setCommandBufferCount(scast<uint32_t>(swapchain_.framebuffers.size()));
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

			buffer.drawIndexed(scast<uint32_t>(memory_.index.indices.size()), 1, 0, 0, 0);

			buffer.endRenderPass();

			buffer.end();
		}
	}

	void renderwindow::create_sync()
	{
		for(size_t i{0}; i < sync_t::MAX_FRAMES; i++) {
			vk::SemaphoreCreateInfo semaphoreinfo{};

			sync_.images[i] = MFW_VKRES(device_.device->createSemaphoreUnique(move(semaphoreinfo), &__vk_alloc_callbacks()));
			sync_.renders[i] = MFW_VKRES(device_.device->createSemaphoreUnique(move(semaphoreinfo), &__vk_alloc_callbacks()));

			vk::FenceCreateInfo fenceinfo{};
			fenceinfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

			sync_.fences[i] = MFW_VKRES(device_.device->createFenceUnique(move(fenceinfo), &__vk_alloc_callbacks()));
		}
	}

	void renderwindow::create_buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memoryflags, vk::UniqueBuffer &buffer, vk::UniqueDeviceMemory &memory)
	{
		vk::BufferCreateInfo buffercreate{};
		buffercreate.setSize(scast<uint32_t>(size));
		buffercreate.setUsage(usage);
		buffercreate.setSharingMode(vk::SharingMode::eExclusive);
		buffer = MFW_VKRES(device_.device->createBufferUnique(move(buffercreate), &__vk_alloc_callbacks()));

		vk::MemoryRequirements required{};
		device_.device->getBufferMemoryRequirements(*buffer, &required);

		vk::MemoryAllocateInfo allocinfo{};
		allocinfo.setAllocationSize(required.size);

		vk::PhysicalDeviceMemoryProperties props{};
		device_.physical_device.getMemoryProperties(&props);

		for(uint32_t i{0}; i < props.memoryTypeCount; i++) {
			if(bcast(required.memoryTypeBits & (1 << i)) && bcast(props.memoryTypes[i].propertyFlags & memoryflags)) {
				allocinfo.setMemoryTypeIndex(i);
				break;
			}
		}

		memory = MFW_VKRES(device_.device->allocateMemoryUnique(move(allocinfo), &__vk_alloc_callbacks()));

		device_.device->bindBufferMemory(*buffer, *memory, 0);
	}

	void renderwindow::begin_single_tile_commands(vk::UniqueCommandBuffer &cmdbuffer)
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

	void renderwindow::end_single_tile_commands(vk::UniqueCommandBuffer &cmdbuffer)
	{
		cmdbuffer->end();

		vk::SubmitInfo submitinfo{};
		submitinfo.setCommandBufferCount(1);
		submitinfo.setPCommandBuffers(&(*cmdbuffer));

		device_.queues.graphics().queue.submit(1, &submitinfo, nullptr);

		device_.queues.graphics().queue.waitIdle();
	}

	void renderwindow::copy_buffer(vk::UniqueBuffer &src, vk::UniqueBuffer &dst, size_t size)
	{
		vk::UniqueCommandBuffer cmdbuffer{};
		begin_single_tile_commands(cmdbuffer);

		vk::BufferCopy buffcopy{};
		buffcopy.setSize(scast<vk::DeviceSize>(size));
		cmdbuffer->copyBuffer(*src, *dst, 1, &buffcopy);

		end_single_tile_commands(cmdbuffer);
	}

	void renderwindow::create_vertex_buffer()
	{
		vk::UniqueBuffer stagingbuffer{};
		vk::UniqueDeviceMemory stagingmemory{};

		size_t vertisize{sizeof(mesh) * memory_.vertex.vertices.size()};
		create_buffer(vertisize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingbuffer, stagingmemory);

		void *data{nullptr};
		device_.device->mapMemory(stagingmemory.get(), 0, scast<vk::DeviceSize>(vertisize), vk::MemoryMapFlags{0}, &data);
		memcpy(data, memory_.vertex.vertices.data(), vertisize);
		device_.device->unmapMemory(*stagingmemory);

		create_buffer(vertisize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, memory_.vertex.buffer, memory_.vertex.memory);

		copy_buffer(stagingbuffer, memory_.vertex.buffer, vertisize);
	}

	void renderwindow::create_index_buffer()
	{
		vk::UniqueBuffer stagingbuffer{};
		vk::UniqueDeviceMemory stagingmemory{};

		size_t indisize{sizeof(uint16_t) * memory_.index.indices.size()};
		create_buffer(indisize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingbuffer, stagingmemory);

		void *data{nullptr};
		device_.device->mapMemory(stagingmemory.get(), 0, scast<vk::DeviceSize>(indisize), vk::MemoryMapFlags{0}, &data);
		memcpy(data, memory_.index.indices.data(), indisize);
		device_.device->unmapMemory(*stagingmemory);

		create_buffer(indisize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, memory_.index.buffer, memory_.index.memory);

		copy_buffer(stagingbuffer, memory_.index.buffer, indisize);
	}

	void renderwindow::create_memory_buffers()
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
		memory_.cmdpool = MFW_VKRES(device_.device->createCommandPoolUnique(move(cmdpool), &__vk_alloc_callbacks()));

		create_vertex_buffer();
		create_index_buffer();
	}

	void renderwindow::recreate_swapchain()
	{
		device_.device->waitIdle();

		pipeline_.reset();
		swapchain_.reset();

		create_swapchain(device_.physical_device);
		create_pipeline();
		create_frame_buffers();
	}

	int64_t renderwindow::window_proc(uint32_t msg, uint64_t param1, int64_t param2)
	{
		if(msg == WM_SIZE) {
			swapchain_.resized = true;
			return __super::window_proc(msg, param1, param2);
		} else if(msg == WM_PAINT) {
			if(minimized()) {
				return 0;
			}

			device_.device->waitForFences(1, &(*sync_.fences[sync_.frame]), true, UINT64_MAX);

			uint32_t imageidx{0};
			vk::Result result{device_.device->acquireNextImageKHR(*swapchain_.swapchain, UINT64_MAX, (*sync_.images[sync_.frame]), nullptr, &imageidx)};

			if(result == vk::Result::eErrorOutOfDateKHR) {
				recreate_swapchain();
				return 0;
			}

			vk::SubmitInfo submitinfo{};

			const vk::PipelineStageFlags stageflags{vk::PipelineStageFlagBits::eColorAttachmentOutput};
			submitinfo.setPWaitDstStageMask(&stageflags);

			submitinfo.setCommandBufferCount(1);
			submitinfo.setPCommandBuffers(&(*swapchain_.cmdbuffers[imageidx]));

			submitinfo.setWaitSemaphoreCount(1);
			submitinfo.setPWaitSemaphores(&(*sync_.images[sync_.frame]));

			submitinfo.setSignalSemaphoreCount(1);
			submitinfo.setPSignalSemaphores(&(*sync_.renders[sync_.frame]));

			device_.device->resetFences(1, &(*sync_.fences[sync_.frame]));

			device_.queues.graphics().queue.submit(1, &submitinfo, (*sync_.fences[sync_.frame]));

			vk::PresentInfoKHR presentinfo{};
			presentinfo.setWaitSemaphoreCount(1);
			presentinfo.setPWaitSemaphores(&(*sync_.renders[sync_.frame]));

			presentinfo.setSwapchainCount(1);
			presentinfo.setPSwapchains(&(*swapchain_.swapchain));

			presentinfo.setPImageIndices(&imageidx);

			result = device_.queues.present().queue.presentKHR(&presentinfo);

			if(result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || swapchain_.resized) {
				swapchain_.resized = false;
				recreate_swapchain();
			}

			sync_.frame = ((sync_.frame + 1) % sync_t::MAX_FRAMES);

			return 0;
		}

		return __super::window_proc(msg, param1, param2);
	}
};