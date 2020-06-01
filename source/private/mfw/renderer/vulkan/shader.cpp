#include <private/mfw/renderer/vulkan/shader.hpp>
#include <public/mfw/core/filesystem_interface.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <shaderc/shaderc.hpp>

namespace mfw::renderer::vulkan
{
	MFW_DECLARE_LOG_CONTEXT(log_shader, u8"renderer/vulkan/shader"_p)

	static void __open_binary_file(vector<uint32_t> &bin, const core::searchpath &search)
	{
		core::interfaces::file *file_{core::interfaces::filesystem::instance().open_file(search, core::open_flags::read)};
		if(!file_) {
			return;
		}

		size_t filesize{file_->size()};
		if(filesize == 0) {
			return;
		}

		bin.resize(static_cast<size_t>(filesize), 0);
		file_->read(const_cast<uint32_t *>(bin.data()), static_cast<size_t>(filesize));

		delete file_;
	}

	void shader::initialize()
	{
		shaderc::Compiler compiler{};

		shaderc::CompileOptions options{};
		//options.SetForcedVersionProfile(150, shaderc_profile_core);
		options.SetSourceLanguage(shaderc_source_language_glsl);
		//options.SetTargetEnvironment(shaderc_target_env_vulkan, 0);
		options.SetAutoBindUniforms(true);
		options.SetAutoMapLocations(true);
	#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
		options.SetGenerateDebugInfo();
		options.SetWarningsAsErrors();
		options.SetOptimizationLevel(shaderc_optimization_level_zero);
	#else
		options.SetSuppressWarnings();
		options.SetOptimizationLevel(shaderc_optimization_level_performance);
	#endif

		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		filesys.add_searchpath({u8"shaders"_sv, u8"shaders"_sv}, {{}, u8"root"_sv});

		filesys.print_searchmap();

		vector<pstring> files{};
		filesys.glob({u8"*.*"_sv, u8"shaders"_sv}, files);

		for(const pstring &file : files) {
			pstring ext{file.extension()};

			type type_{};
			if(ext == u8".vert"_sv) {
				type_ = type::vertex;
			} else if(ext == u8".frag"_sv) {
				type_ = type::fragment;
			} else {
				continue;
			}

			shaderc_shader_kind kind{};
			if(type_ == type::vertex) {
				kind = shaderc_vertex_shader;
			} else if(type_ == type::fragment) {
				kind = shaderc_fragment_shader;
			}

			ucstring code{};
			filesys.open_text_file(file, code);

			shaderc::SpvCompilationResult res{compiler.CompileGlslToSpv(c_str(code), kind, c_str(file), "main", options)};

			if(res.GetCompilationStatus() != shaderc_compilation_status_success) {
				::MFW_STD_NAMESPACE::string std_message{res.GetErrorMessage()};
				std_message.erase(std_message.cend()-1);

				MFW_MESSAGE("get rid of this rcast")
				log_shader().warning(u8"{}"_sv, uc_str(std_message));
			} else {
				core::interfaces::file *file_{filesys.open_file({as_string<ucstring>(file) + u8".bin"_s}, core::open_flags::all)};

				for(uint32_t it : res) {
					file_->write(&it, sizeof(uint32_t), 1);
				}

				delete file_;

				log_shader().info(u8"compiled {}"_sv, file);
			}
		}
	}

	void shader::shutdown()
	{
		
	}

	shader::stages_t shader::stages() const
	{
		stages_t stages{};
		for(size_t i{0}; i < data.size(); i++) {
			stages[i] = data[i].stageinfo;
		}
		return stages;
	}

	void shader::reset()
	{
		for(data_t &data_ : data) {
			data_.module_.reset(nullptr);
		}
	}

	bool shader::load_files(const vk::Device &device)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		vector<pstring> files{};
		filesys.glob({name_+u8".*.bin"_s, u8"shaders"_sv}, files);

		for(const pstring &file : files) {
			pstring ext{file.extension()};

			type type_{};
			if(ext == u8".vert.bin"_sv) {
				type_ = type::vertex;
			} else if(ext == u8".frag.bin"_sv) {
				type_ = type::fragment;
			} else {
				return false;
			}

			if(!load_file(device, type_, {file})) {
				return false;
			}
		}

		return true;
	}

	bool shader::load_file(const vk::Device &device, type type_, const core::searchpath &search)
	{
		if(data[type_].module_) {
			return true;
		}

		vector<uint32_t> code{};
		__open_binary_file(code, search);
		if(code.empty()) {
			return false;
		}

		data[type_].module_.reset(nullptr);

		vk::ShaderModuleCreateInfo moduleinfo{};
		moduleinfo.setCodeSize(code.size());
		moduleinfo.setPCode(reinterpret_cast<const uint32_t *>(code.data()));
		data[type_].module_ = MFW_VKRES(device.createShaderModuleUnique(move(moduleinfo), &__vk_alloc_callbacks()));

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