#include <private/mfw/renderer/vulkan/spirv_shader.hpp>
#include <public/mfw/core/filesystem_interface.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <private/mfw/renderer/vulkan/shaderc.hpp>

#include <private/mfw/renderer/opengl/shader.hpp>

#define __MFW_LATEST_SPIRV_VERSION shaderc_spirv_version_1_5
#define __MFW_LATEST_VULKAN_VERSION shaderc_env_version_vulkan_1_2
#define __MFW_LATEST_OPENGL_VERSION shaderc_env_version_opengl_4_5

namespace mfw::renderer
{
	MFW_DECLARE_LOG_CONTEXT(log_spirv_shader, u8"renderer/render_api/vulkan/spirv_shader"_p)

	spirv_shader_type get_spirv_type(const pstring &ext)
	{
		ucstring str{as_string<ucstring>(ext)};
		return get_spirv_type(ucstring_view{str.c_str(), str.length()});
	}

	spirv_shader_type get_spirv_type(ucstring_view ext)
	{
		if(ext.find(u8".vert"_sv) != ucstring_view::npos) {
			return spirv_shader_type::vertex;
		} else if(ext.find(u8".frag"_sv) != ucstring_view::npos) {
			return spirv_shader_type::fragment;
		}
		return spirv_shader_type::unknown;
	}

	void compile_spirv_shaders(spirv_shader_target target, spirv_shader_source source)
	{
		shaderc::Compiler compiler{};

		shaderc::CompileOptions options{};
		switch(source) {
			case spirv_shader_source::glsl: { 
				options.SetForcedVersionProfile(__MFW_LATEST_GLSL_VERSION, shaderc_profile_core);
				options.SetSourceLanguage(shaderc_source_language_glsl);
				break;
			}
			case spirv_shader_source::hlsl: { options.SetSourceLanguage(shaderc_source_language_hlsl); break; }
		}
		switch(target) {
			case spirv_shader_target::vulkan: { options.SetTargetEnvironment(shaderc_target_env_vulkan, __MFW_LATEST_VULKAN_VERSION); break; }
			case spirv_shader_target::opengl: { options.SetTargetEnvironment(shaderc_target_env_opengl, __MFW_LATEST_OPENGL_VERSION); break; }
		}
		options.SetTargetSpirv(__MFW_LATEST_SPIRV_VERSION);
		options.SetAutoBindUniforms(true);
		options.SetAutoMapLocations(true);
	#if MFW_CONFIGURATION_IS(DEBUG)
		options.SetGenerateDebugInfo();
		options.SetWarningsAsErrors();
		options.SetOptimizationLevel(shaderc_optimization_level_zero);
	#else
		options.SetSuppressWarnings();
		options.SetOptimizationLevel(shaderc_optimization_level_performance);
	#endif

		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		switch(source) {
			case spirv_shader_source::glsl: {
				filesys.add_searchpath({u8"glsl"_sv, u8"shaders_src"_sv}, {{}, u8"shaders"_sv});
				switch(target) {
					case spirv_shader_target::vulkan: {
						filesys.add_searchpath({u8"bin/glsl_spirv_vulkan"_sv, u8"shaders_bin"_sv}, {{}, u8"shaders"_sv});
						break;
					}
					case spirv_shader_target::opengl: {
						filesys.add_searchpath({u8"bin/shaders_glsl_spirv_opengl"_sv, u8"shaders_bin"_sv}, {{}, u8"shaders"_sv});
						break;
					}
				}
				break;
			}
			case spirv_shader_source::hlsl: {
				filesys.add_searchpath({u8"hlsl"_sv, u8"shaders_src"_sv}, {{}, u8"shaders"_sv});
				switch(target) {
					case spirv_shader_target::vulkan: {
						filesys.add_searchpath({u8"bin/hlsl_spirv_vulkan"_sv, u8"shaders_bin"_sv}, {{}, u8"shaders"_sv});
						break;
					}
					case spirv_shader_target::opengl: {
						filesys.add_searchpath({u8"bin/shaders_hlsl_spirv_opengl"_sv, u8"shaders_bin"_sv}, {{}, u8"shaders"_sv});
						break;
					}
				}
				break;
			}
		}

		filesys.print_searchmap();

		vector<pstring> files{};
		filesys.glob({u8"*.*"_sv, u8"shaders_src"_sv}, files);

		for(const pstring &file : files) {
			spirv_shader_type type_{get_spirv_type(file)};
			if(type_ == spirv_shader_type::unknown) {
				continue;
			}

			#define __MFW_SHADERC_KIND_INVALID static_cast<shaderc_shader_kind>(-1)

			shaderc_shader_kind kind{__MFW_SHADERC_KIND_INVALID};
			switch(type_) {
				case spirv_shader_type::vertex: { kind = shaderc_vertex_shader; break; }
				case spirv_shader_type::fragment: { kind = shaderc_fragment_shader; break; }
			}
			if(kind == __MFW_SHADERC_KIND_INVALID) {
				continue;
			}

			ucstring code{};
			filesys.open_text_file(file, code);

			shaderc::SpvCompilationResult res{};

			if(source == spirv_shader_source::glsl) {
				res = compiler.CompileGlslToSpv(c_str(code), kind, c_str(file), "main", options);
			} else {
				continue;
			}

			if(res.GetCompilationStatus() != shaderc_compilation_status_success) {
				::MFW_STD_NAMESPACE::string std_message{res.GetErrorMessage()};
				if(!std_message.empty()) {
					std_message.erase(std_message.cend()-1);

					ucstring msg{};
					msg.assign(uc_str(std_message), std_message.length());

					MFW_MESSAGE("TODO proper color code")

					log_spirv_shader().warning(u8"{}"_sv, msg);
				}
			} else {
				pstring bin_file{file.filename()};
				bin_file.concat(u8".bin"_s);

				core::interfaces::file *file_{filesys.open_file({bin_file, u8"shaders_bin"_sv}, core::open_flags::all)};

				for(uint32_t it : res) {
					file_->write(&it, sizeof(uint32_t), 1);
				}

				delete file_;

				log_spirv_shader().info(u8"compiled {}"_sv, file);
			}
		}
	}
}