#ifndef MFW_PRIVATE_RENDERER_OPENGL_SHADER_HPP
#define MFW_PRIVATE_RENDERER_OPENGL_SHADER_HPP

#pragma once

#include <private/mfw/renderer/opengl/opengl.hpp>
#include <public/mfw/core/searchpath.hpp>
#include <public/mfw/stl/array.hpp>
#include <private/mfw/renderer/vulkan/spirv_shader.hpp>

#define __MFW_LATEST_GLSL_VERSION 460

namespace mfw::renderer
{
	class shader_opengl
	{
	public:
		shader_opengl(ucstring_view name);
		~shader_opengl() { reset(); }

		using type = spirv_shader_type;

		static void compile_shaders(bool spirv);
		static void initialize(bool spirv, uint32_t vendor, uint32_t device);
		static void shutdown();

		void reset();

		bool load_files();

		void activate();

	private:
		enum class file_type : uchar_t
		{
			source,
			binary,
		};

		static type get_file_type(const pstring &ext) { return get_spirv_type(ext); }
		static GLenum get_gl_type(type type_);

		static bool compile_shader(GLuint shader, const pstring &file);
		static bool shader_compiled(GLuint shader, const pstring &filename);
		static bool program_linked(GLuint prog, ucstring_view name);
		static bool link_program(GLuint prog, ucstring_view name);

		bool load_file(type type_, const pstring &file, file_type file_type);

		ucstring name_{};
		GLuint program{0};

		static inline GLenum program_bin_format{0};
		static inline GLenum shader_bin_format{0};

		struct data_t
		{
			GLuint id{0};
		};

		array<data_t, type::count> data{};
	};
}

#endif