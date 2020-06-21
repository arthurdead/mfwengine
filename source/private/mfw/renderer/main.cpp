#include <public/mfw/core/application.hpp>

#if MFW_OS_IS(WINDOWS)
	#if MFW_COMPILER_FLAGGED(MSVC)
		MFW_WARNING_PUSH()
		MFW_WARNING_DISABLE(4464)
	#endif
	#include <glslang/OGLCompilersDLL/InitializeDll.h>
	#if MFW_COMPILER_FLAGGED(MSVC)
		MFW_WARNING_POP()
	#endif
#endif

#if MFW_OS_IS(WINDOWS)
::mfw::core::exit_status renderer_main(
	#if MFW_OS_IS(WINDOWS)
	bool thread
	#endif
)
{
	mfw::core::exit_status status{};

	if(!thread) {
		if(!glslang::InitProcess()) {
			code = ::mfw::core::exit_code::error;
		}
	} else {
		if(!glslang::InitThread()) {
			code = ::mfw::core::exit_code::error;
		}
	}

	return status;
}

::mfw::core::exit_status renderer_exit(
	#if MFW_OS_IS(WINDOWS)
	bool thread
	#endif
)
{
	::mfw::core::exit_status status{};

	if(!thread) {
		if(!glslang::DetachProcess()) {
			code = ::mfw::core::exit_code::error;
		}
	} else {
		if(!glslang::DetachThread()) {
			code = ::mfw::core::exit_code::error;
		}
	}

	return status;
}

	#if MFW_RENDERER_BUILD == MFW_BUILD_SHARED
::mfw::core::exit_status application_main(
		#if MFW_OS_IS(WINDOWS)
	bool thread
		#endif
)
{
	return renderer_main(
		#if MFW_OS_IS(WINDOWS)
		thread
		#endif
	);
}

::mfw::core::exit_status application_exit(
		#if MFW_OS_IS(WINDOWS)
bool thread
		#endif
)
{
	return renderer_exit(
		#if MFW_OS_IS(WINDOWS)
		thread
		#endif
	);
}
	#endif
#endif