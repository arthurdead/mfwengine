#include <private/mfw/renderer/opengl/shader.hpp>
#include <public/mfw/core/filesystem_interface.hpp>
#include <public/mfw/core/logging_interface.hpp>

namespace mfw::renderer
{
	MFW_DECLARE_LOG_CONTEXT(log_shader_opengl, u8"renderer/render_api/opengl/shader"_p)

	namespace __shader_internal
	{
		MFW_MESSAGE("remove both")

		static void open_binary_file(vector<byte> &bin, const pstring &file)
		{
			core::interfaces::file *file_{core::interfaces::filesystem::instance().open_file({file}, core::open_flags::read)};
			if(!file_) {
				return;
			}

			size_t filesize{file_->size()};
			if(filesize == 0) {
				return;
			}

			bin.resize(filesize);
			file_->read(bin.data(), filesize);

			delete file_;
		}

		static void save_binary_file(const vector<byte> &bin, const core::searchpath &search)
		{
			core::interfaces::file *file_{core::interfaces::filesystem::instance().open_file(search, core::open_flags::all)};
			if(!file_) {
				return;
			}

			size_t filesize{bin.size()};
			file_->write(bin.data(), filesize);

			delete file_;
		}
	}

	bool shader_opengl::link_program(GLuint prog, ucstring_view name)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		glLinkProgram(prog);

		if(!program_linked(prog, name)) {
			return false;
		}

		GLint len{0};
		glGetProgramiv(prog, GL_PROGRAM_BINARY_LENGTH, &len);

		vector<byte> prog_bin{};
		prog_bin.resize(static_cast<size_t>(len));
		glGetProgramBinary(prog, len, nullptr, &program_bin_format, prog_bin.data());

		ucstring file{name};
		file += u8".bin"_sv;

		__shader_internal::save_binary_file(prog_bin, {file, u8"shaders_program_bin"_sv});

		return true;
	}

	bool shader_opengl::compile_shader(GLuint shader, const pstring &file)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		ucstring code{};
		filesys.open_text_file(file, code);

		ucstring str{u8"#version {} core\n"_fmt(__MFW_LATEST_GLSL_VERSION)};
		size_t it{0};
		code.insert(it, str);
		it += str.length();

		str = u8"#extension GL_KHR_vulkan_glsl : enable\n"_sv;
		code.insert(it, str);
		it += str.length();

		const char *sources[]{
			c_str(code),
		};
		glShaderSource(shader, size(sources), sources, nullptr);
		glCompileShaderIncludeARB(shader, 0, nullptr, nullptr);

		if(!shader_compiled(shader, file)) {
			return false;
		}

		return true;
	}

	void shader_opengl::initialize(bool spirv, uint32_t vendor, uint32_t device)
	{
		if(!spirv) {
			glGetIntegerv(GL_SHADER_BINARY_FORMATS, reinterpret_cast<GLint *>(&shader_bin_format));
		} else {
			//shader_bin_format = static_cast<GLenum>(GL_SHADER_BINARY_FORMAT_SPIR_V);
			shader_bin_format = static_cast<GLenum>(GL_SHADER_BINARY_FORMAT_SPIR_V_ARB);
		}

		glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, reinterpret_cast<GLint *>(&program_bin_format));

		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		if(!spirv) {
			filesys.add_searchpath({u8"bin/shaders_glsl_vendor_{}_{}_{}_opengl"_fmt(vendor, device, shader_bin_format), u8"shaders_bin"_sv}, {{}, u8"shaders"_sv});
		}
		filesys.add_searchpath({u8"bin/programs_vendor_{}_{}_{}_opengl"_fmt(vendor, device, program_bin_format), u8"shaders_program_bin"_sv}, {{}, u8"shaders"_sv});
	}

	void shader_opengl::compile_shaders(bool spirv)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		if(spirv) {
			compile_spirv_shaders(spirv_shader_target::opengl, spirv_shader_source::glsl);
		} else {
			filesys.add_searchpath({u8"glsl"_sv, u8"shaders_src"_sv}, {{}, u8"shaders"_sv});

			filesys.print_searchmap();

			vector<pstring> files{};
			filesys.glob({u8"*.*"_sv, u8"shaders_src"_sv}, files);

			struct shader_vars_t {
				GLuint prog{0};
				vector<GLuint> shaders{0};
				~shader_vars_t() {
					for(GLuint shader : shaders) {
						if(prog && shader) {
							glDetachShader(prog, shader);
						}
						if(shader) {
							glDeleteShader(shader);
						}
					}
					if(prog) {
						glDeleteProgram(prog);
					}
				}
			};

			using shader_src_map_t = unordered_map<ucstring, shader_vars_t>;
			shader_src_map_t src_map{};

			for(const pstring &file : files) {
				type type_{get_file_type(file)};
				if(type_ == type::unknown) {
					continue;
				}

				ucstring name{as_string<ucstring>(file.filename().replace_extension())};

				shader_src_map_t::iterator it{src_map.find(name)};
				if(it == src_map.end()) {
					it = src_map.emplace(shader_src_map_t::value_type{name, {}}).first;
					it->second.prog = glCreateProgram();
					glProgramParameteri(it->second.prog, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
				}

				shader_vars_t &vars{it->second};
				GLuint &shader{vars.shaders.emplace_back()};
				shader = glCreateShader(get_gl_type(type_));
				glAttachShader(vars.prog, shader);

				compile_shader(shader, file);
			}

			for(shader_src_map_t::value_type &it : src_map) {
				link_program(it.second.prog, it.first);
			}
		}
	}

	bool shader_opengl::shader_compiled(GLuint shader, const pstring &filename)
	{
		GLint compiled{GL_FALSE};
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if(!compiled) {
		#if MFW_CONFIGURATION_IS(DEBUG)
			GLint length{0};
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
			if(length) {
				ucstring info{};
				info.resize(static_cast<size_t>(length), u8'\0');
				glGetShaderInfoLog(shader, length, nullptr, reinterpret_cast<char *>(info.data()));

				ucstring file_str{as_string<ucstring>(filename.filename())};
				file_str += u8'(';
				replace_all(info, u8"0("_sv, file_str);

				info.erase(info.end()-2, info.end());

				glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_LOW, static_cast<GLsizei>(info.length()), c_str(info));
			}
		#endif
		}
		return compiled;
	}

	bool shader_opengl::program_linked(GLuint prog, ucstring_view name)
	{
		GLint linked{GL_FALSE};
		glGetProgramiv(prog, GL_LINK_STATUS, &linked);
		if(linked == GL_FALSE) {
		#if MFW_CONFIGURATION_IS(DEBUG)
			GLint length{0};
			glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &length);
			if(length) {
				ucstring info{};
				info.resize(static_cast<size_t>(length), u8'\0');
				glGetProgramInfoLog(prog, length, nullptr, reinterpret_cast<char *>(info.data()));

				/*
				info.insert(0, u8"Program "_sv);
				info += name;
				info += u8" Failed to link {\n"_sv;

				size_t pos{0};
				while((pos = info.find(u8'\n', pos)) != ucstring::npos) {
					info.replace(pos, 1, u8"\n    "_sv);
					pos++;
				}

				info.replace(info.end()-5, info.end(), u8"  }"_sv);
				*/

				glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_LOW, static_cast<GLsizei>(info.length()), c_str(info));
			}
		#endif
		}
		return linked;
	}

	void shader_opengl::shutdown()
	{
		
	}

	shader_opengl::shader_opengl(ucstring_view name)
		: name_{name}
	{
		
	}

	void shader_opengl::reset()
	{
		for(data_t &data_ : data) {
			if(program && data_.id) {
				glDetachShader(program, data_.id);
			}
			if(data_.id) {
				glDeleteShader(data_.id);
			}
		}

		if(program) {
			glDeleteProgram(program);
		}
	}

	void shader_opengl::activate()
	{
		glUseProgram(program);
	}

	bool shader_opengl::load_files()
	{
		program = glCreateProgram();
		glProgramParameteri(program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
	#if MFW_CONFIGURATION_IS(DEBUG)
		glObjectLabel(GL_PROGRAM, program, static_cast<GLsizei>(name_.length()), c_str(name_));
	#endif

		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		core::interfaces::file *file_{filesys.open_file({name_+u8"*.bin"_s, u8"shaders_program_bin"_sv}, core::open_flags::read)};
		if(file_) {
			size_t filesize{file_->size()};
			if(filesize == 0) {
				return false;
			}

			vector<byte> bin{};
			bin.resize(filesize);
			file_->read(bin.data(), filesize);

			delete file_;

			glProgramBinary(program, program_bin_format, bin.data(), static_cast<GLsizei>(bin.size()));

			if(!program_linked(program, name_)) {
				return false;
			}
		} else {
			vector<pstring> files{};
			filesys.glob({name_+u8".*"_s, u8"shaders_src"_sv}, files);

			for(const pstring &file : files) {
				type type_{get_file_type(file)};
				if(type_ == type::unknown) {
					return false;
				}

				core::searchpath bin_file{file.filename()};
				bin_file.path.concat(u8".bin"_sv);
				bin_file.name_ = u8"shaders_bin"_sv;
				if(filesys.exists(bin_file)) {
					bin_file = filesys.resolve(bin_file);
					if(!load_file(type_, bin_file.dir(), file_type::binary)) {
						return false;
					}
				} else {
					if(!load_file(type_, file, file_type::source)) {
						return false;
					}
				}
			}

			if(!link_program(program, name_)) {
				return false;
			}
		}

		return true;
	}

	GLenum shader_opengl::get_gl_type(type type_)
	{
		switch(type_) {
			case type::vertex: { return GL_VERTEX_SHADER; }
			case type::fragment: { return GL_FRAGMENT_SHADER; }
		}
		return 0;
	}

	bool shader_opengl::load_file(type type_, const pstring &file, file_type file_type_)
	{
		GLenum gl_type{get_gl_type(type_)};
		if(gl_type == 0) {
			return false;
		}

		data[type_].id = glCreateShader(gl_type);
	#if MFW_CONFIGURATION_IS(DEBUG)
		glObjectLabel(GL_SHADER, data[type_].id, static_cast<GLsizei>(file.native().length()), c_str(file));
	#endif
		glAttachShader(program, data[type_].id);

		switch(file_type_) {
			case file_type::binary: {
				vector<byte> code{};
				__shader_internal::open_binary_file(code, file);
				if(code.empty()) {
					return false;
				}

				glShaderBinary(1, &data[type_].id, shader_bin_format, code.data(), static_cast<GLsizei>(code.size()));

				if(!shader_compiled(data[type_].id, file)) {
					return false;
				}

				glSpecializeShader(data[type_].id, "main", 0, nullptr, nullptr);

				break;
			}
			case file_type::source: {
				if(!compile_shader(data[type_].id, file)) {
					return false;
				}
				break;
			}
		}

		return true;
	}
}