#include <private/mfw/renderer/vulkan/shader.hpp>
#include <public/mfw/core/filesystem_interface.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <private/mfw/renderer/vulkan/shared.hpp>

namespace mfw::renderer
{
	MFW_DECLARE_LOG_CONTEXT(log_shader_vulkan, u8"renderer/render_api/vulkan/shader"_p)

	namespace __shader_internal
	{
		MFW_MESSAGE("remove this")

		static void open_binary_file(vector<uint32_t> &bin, const core::searchpath &search)
		{
			core::interfaces::file *file_{core::interfaces::filesystem::instance().open_file(search, core::open_flags::read)};
			if(!file_) {
				return;
			}

			size_t filesize{file_->size()};
			if(filesize == 0) {
				return;
			}

			bin.resize(filesize, 0);
			file_->read(const_cast<uint32_t *>(bin.data()), filesize);

			delete file_;
		}
	}

	void shader_vulkan::compile_shaders()
	{
		compile_spirv_shaders(spirv_shader_target::vulkan, spirv_shader_source::glsl);
	}

	void shader_vulkan::shutdown()
	{
		
	}

	shader_vulkan::stages_t shader_vulkan::stages() const
	{
		stages_t stages{};
		for(size_t i{0}; i < data.size(); i++) {
			stages[i] = data[i].stageinfo;
		}
		return stages;
	}

	void shader_vulkan::reset()
	{
		for(data_t &data_ : data) {
			data_.module_.reset(nullptr);
		}
	}

	bool shader_vulkan::load_files(const vk::Device &device)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		vector<pstring> files{};
		filesys.glob({name_+u8"*.bin"_s, u8"shaders_bin"_sv}, files);

		for(const pstring &file : files) {
			type type_{get_spirv_type(file)};
			if(type_ == spirv_shader_type::unknown) {
				return false;
			}

			if(!load_file(device, type_, {file})) {
				return false;
			}
		}

		return true;
	}

	bool shader_vulkan::load_file(const vk::Device &device, type type_, const core::searchpath &search)
	{
		if(data[type_].module_) {
			return true;
		}

		vector<uint32_t> code{};
		__shader_internal::open_binary_file(code, search);
		if(code.empty()) {
			return false;
		}

		data[type_].module_.reset(nullptr);

		vk::ShaderModuleCreateInfo moduleinfo{};
		moduleinfo.setCodeSize(code.size());
		moduleinfo.setPCode(reinterpret_cast<const uint32_t *>(code.data()));
		data[type_].module_ = MFW_VKRES(device.createShaderModuleUnique(move(moduleinfo), &vulkan::alloc_callbacks()));

		const vk::ShaderModule &module_{*data[type_].module_};
		data[type_].stageinfo.setModule(module_);
		data[type_].stageinfo.module_ = module_;

		data[type_].stageinfo.setPName("main");
		if(type_ == type::vertex) {
			data[type_].stageinfo.setStage(vk::ShaderStageFlagBits::eVertex);
		} else if(type_ == type::fragment) {
			data[type_].stageinfo.setStage(vk::ShaderStageFlagBits::eFragment);
		}

		return true;
	}
}