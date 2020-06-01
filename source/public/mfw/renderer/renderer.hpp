#ifndef __MFW_PUBLIC_RENDERER_H
#define __MFW_PUBLIC_RENDERER_H

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
	}

	namespace literals
	{
		using namespace renderer::literals;
	}
}

#endif