#ifndef MFW_PUBLIC_RENDERER_HPP
#define MFW_PUBLIC_RENDERER_HPP

#pragma once

#if MFW_RENDERER_BUILD & MFW_BUILD_SHARED
	#ifdef MFW_BUILDING_RENDERER
		#define MFW_RENDERER_API MFW_SHARED_EXPORT
	#else
		#define MFW_RENDERER_API MFW_SHARED_IMPORT
	#endif
	#define MFW_RENDERER_CALL MFW_CALL_SHARED
#else
	#define MFW_RENDERER_API 
	#define MFW_RENDERER_CALL 
#endif

namespace mfw
{
	namespace renderer
	{
		namespace interfaces {}
		using namespace interfaces;
		namespace literals {}
		using namespace literals;

	#if MFW_RENDERER_BUILD & MFW_BUILD_STATIC && MFW_OS_IS(WINDOWS)
		extern core::exit_status renderer_main(
		#if MFW_OS_IS(WINDOWS)
			bool thread
		#endif
		);

		extern core::exit_status renderer_exit(
		#if MFW_OS_IS(WINDOWS)
			bool thread
		#endif
		);
	#endif
	}

	namespace literals
	{
		using namespace renderer::literals;
	}
}

#endif