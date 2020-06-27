#include <private/mfw/renderer/opengl/render_api_funcs.hpp>
#include <private/mfw/renderer/opengl/context_api.hpp>
#include <private/mfw/renderer/opengl/context_api_funcs.hpp>
#include <private/mfw/renderer/opengl/opengl.hpp>

namespace mfw::renderer
{
	MFW_DECLARE_LOG_CONTEXT(log_render_api_opengl, u8"renderer/render_api/opengl"_p)

	render_api_funcs_opengl &render_api_funcs_opengl::instance() {
		return reinterpret_cast<render_api_funcs_opengl &>(interfaces::render_api_funcs::instance());
	}

	namespace __render_api_funcs_internal
	{
	#if MFW_CONFIGURATION_IS(DEBUG)
		static void debug_msg(uint32_t source, uint32_t type, uint32_t id, uint32_t severity, int32_t length, const char *message, void *userParam)
		{
			ucstring_view str_source{};
			switch(source) {
				case GL_DEBUG_SOURCE_API: { str_source = u8"GL_DEBUG_SOURCE_API"_sv; break; }
				case GL_DEBUG_SOURCE_WINDOW_SYSTEM: { str_source = u8"GL_DEBUG_SOURCE_WINDOW_SYSTEM"_sv; break; }
				case GL_DEBUG_SOURCE_SHADER_COMPILER: { str_source = u8"GL_DEBUG_SOURCE_SHADER_COMPILER"_sv; break; }
				case GL_DEBUG_SOURCE_THIRD_PARTY: { str_source = u8"GL_DEBUG_SOURCE_THIRD_PARTY"_sv; break; }
				case GL_DEBUG_SOURCE_APPLICATION: { str_source = u8"GL_DEBUG_SOURCE_APPLICATION"_sv; break; }
				case GL_DEBUG_SOURCE_OTHER: { str_source = u8"GL_DEBUG_SOURCE_OTHER"_sv; break; }
			}

			ucstring_view str_type{};
			switch(type) {
				case GL_DEBUG_TYPE_ERROR: { str_type = u8"GL_DEBUG_TYPE_ERROR"_sv; break; }
				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: { str_type = u8"GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR"_sv; break; }
				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: { str_type = u8"GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR"_sv; break; }
				case GL_DEBUG_TYPE_PORTABILITY: { str_type = u8"GL_DEBUG_TYPE_PORTABILITY"_sv; break; }
				case GL_DEBUG_TYPE_PERFORMANCE: { str_type = u8"GL_DEBUG_TYPE_PERFORMANCE"_sv; break; }
				case GL_DEBUG_TYPE_OTHER: { str_type = u8"GL_DEBUG_TYPE_OTHER"_sv; break; }
				case GL_DEBUG_TYPE_MARKER: { str_type = u8"GL_DEBUG_TYPE_MARKER"_sv; break; }
				case GL_DEBUG_TYPE_PUSH_GROUP: { str_type = u8"GL_DEBUG_TYPE_PUSH_GROUP"_sv; break; }
				case GL_DEBUG_TYPE_POP_GROUP: { str_type = u8"GL_DEBUG_TYPE_POP_GROUP"_sv; break; }
			}

			ucstring_view str_severity{};
			switch(severity) {
				case GL_DEBUG_SEVERITY_LOW: { str_severity = u8"GL_DEBUG_SEVERITY_LOW"_sv; break; }
				case GL_DEBUG_SEVERITY_MEDIUM: { str_severity = u8"GL_DEBUG_SEVERITY_MEDIUM"_sv; break; }
				case GL_DEBUG_SEVERITY_HIGH: { str_severity = u8"GL_DEBUG_SEVERITY_HIGH"_sv; break; }
				case GL_DEBUG_SEVERITY_NOTIFICATION: { str_severity = u8"GL_DEBUG_SEVERITY_NOTIFICATION"_sv; break; }
			}

			ucstring msg{uc_str(message)};

			log_render_api_opengl().resume();
			log_render_api_opengl().error(u8"{}\n{"_sv, msg);
			log_render_api_opengl().add_ident();
			log_render_api_opengl().error(u8"{}: {}"_sv, source, str_source);
			log_render_api_opengl().error(u8"{}: {}"_sv, type, str_type);
			log_render_api_opengl().error(u8"{}: {}"_sv, severity, str_severity);
			log_render_api_opengl().remove_ident();
			log_render_api_opengl().error(u8"}"_sv);
		}

		static void glad_post(void *ret, const char *name, GLADapiproc funcptr, int32_t len_args, ...)
		{
			uint32_t error{glad_glGetError()};
			if(error != GL_NO_ERROR ||
				funcptr == reinterpret_cast<GLADapiproc>(glad_glDebugMessageInsert)) {
				log_render_api_opengl().resume();
				ucstring_view str_error{};
				switch(error) {
					case GL_NO_ERROR: { str_error = u8"GL_NO_ERROR"_sv; break; }
					case GL_INVALID_ENUM: { str_error = u8"GL_INVALID_ENUM"_sv; break; }
					case GL_INVALID_VALUE: { str_error = u8"GL_INVALID_VALUE"_sv; break; }
					case GL_INVALID_OPERATION: { str_error = u8"GL_INVALID_OPERATION"_sv; break; }
					case GL_INVALID_FRAMEBUFFER_OPERATION: { str_error = u8"GL_INVALID_FRAMEBUFFER_OPERATION"_sv; break; }
					case GL_OUT_OF_MEMORY: { str_error = u8"GL_OUT_OF_MEMORY"_sv; break; }
					case GL_STACK_UNDERFLOW: { str_error = u8"GL_STACK_UNDERFLOW"_sv; break; }
					case GL_STACK_OVERFLOW: { str_error = u8"GL_STACK_OVERFLOW"_sv; break; }
				}

				log_render_api_opengl().error(u8"{}: {}"_sv, error, str_error);
			}

			log_render_api_opengl().remove_ident();
			log_render_api_opengl().info(u8"}"_sv);

			log_render_api_opengl().clear_history();
			log_render_api_opengl().resume();
		}

		static void glad_pre(const char *name, GLADapiproc funcptr, int32_t len_args, ...)
		{
			ucstring name_str{uc_str(name)};

			log_render_api_opengl().clear_history();
			log_render_api_opengl().pause();
			log_render_api_opengl().info(u8"{}\n{"_sv, name_str);
			log_render_api_opengl().add_ident();
		}
	#endif
	}

	render_api_funcs_opengl::~render_api_funcs_opengl()
	{
		gladLoaderUnloadGL();

	#if MFW_CONFIGURATION_IS(DEBUG)
		gladUninstallGLDebug();
	#endif
	}

	bool render_api_funcs_opengl::pre_initialize()
	{
		if(!detect_context_api()) {
			return false;
		}

		core::commandline &cmdline{core::commandline::instance()};

		use_spirv = cmdline.get_bool(u8"spirv"_s);

	#if MFW_CONFIGURATION_IS(DEBUG)
		gladInstallGLDebug();

		gladSetGLPreCallback(reinterpret_cast<GLADprecallback>(__render_api_funcs_internal::glad_pre));
		gladSetGLPostCallback(reinterpret_cast<GLADpostcallback>(__render_api_funcs_internal::glad_post));
	#endif

		return interfaces::context_api_funcs::instance().initialize();
	}

	bool render_api_funcs_opengl::initialize()
	{
		gladLoaderLoadGL();

		if(use_spirv) {
			shader_opengl::compile_shaders(true);
		}

		return true;
	}

	void render_api_funcs_opengl::on_context_loaded(const graphics_card &gpu, GLADloadfunc func)
	{
		if(!gladLoadGL(func)) {
			MFW_DEBUGBREAK();
		}

		log_render_api_opengl().clear_history();
		log_render_api_opengl().resume();

		ucstring name{reinterpret_cast<const ucchar_t *>(glGetString(GL_VENDOR))};
		log_render_api_opengl().info(u8"GL_VENDOR​: {}"_sv, name);
		name = reinterpret_cast<const ucchar_t *>(glGetString(GL_RENDERER));
		log_render_api_opengl().info(u8"GL_RENDERER: {}"_sv, name);
		name = reinterpret_cast<const ucchar_t *>(glGetString(GL_VERSION));
		log_render_api_opengl().info(u8"GL_VERSION: {}"_sv, name);
		name = reinterpret_cast<const ucchar_t *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
		log_render_api_opengl().info(u8"GL_SHADING_LANGUAGE_VERSION: {}"_sv, name);

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(reinterpret_cast<GLDEBUGPROC>(__render_api_funcs_internal::debug_msg), nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

		shader_opengl::initialize(use_spirv, gpu.vendor_id(), gpu.device_id());

		if(!use_spirv) {
			shader_opengl::compile_shaders(false);
		}

		constexpr float32_t vertices[]{
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f,  0.5f, 0.0f
		};

		shader.load_files();

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float32_t), nullptr);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0); 
	}

	void render_api_funcs_opengl::create_window(window &win, const graphics_card &gpu, size_t w, size_t h)
	{
		interfaces::context_api_funcs::instance().create_window(win, gpu, w, h);
	}

	void render_api_funcs_opengl::destroy_window(window &win)
	{
		interfaces::context_api_funcs::instance().destroy_window(win);
	}

	void render_api_funcs_opengl::render_window(window &win)
	{
		interfaces::context_api_funcs::instance().make_current(win);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.activate();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		interfaces::context_api_funcs::instance().swap_buffers(win);
	}
}