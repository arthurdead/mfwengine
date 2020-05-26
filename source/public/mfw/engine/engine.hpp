#ifndef __MFW_PUBLIC_ENGINE_H
#define __MFW_PUBLIC_ENGINE_H

#pragma once

#if MFW_ENGINE_BUILD & MFW_BUILD_SHARED_FLAG
	#ifdef MFW_BUILDING_ENGINE
		#define MFW_ENGINE_API MFW_SHARED_EXPORT
	#else
		#define MFW_ENGINE_API MFW_SHARED_IMPORT
	#endif
	#define MFW_ENGINE_CALL MFW_CALL_SHARED
#else
	#define MFW_ENGINE_API 
	#define MFW_ENGINE_CALL 
#endif

namespace mfw
{
	namespace engine
	{
		namespace interfaces {};
		using namespace interfaces;
		namespace literals {};
		using namespace literals;
	};

	namespace literals
	{
		using namespace engine::literals;
	};
};

#endif