#ifndef __MFW_SCRIPTING_H
#define __MFW_SCRIPTING_H

#pragma once

#if MFW_SCRIPTING_BUILDTYPE == MFW_BUILDTYPE_SHARED
#ifdef MFW_BUILDING_SCRIPTING
#define MFW_SCRIPTING_API MFW_SHAREDEXPORT
#else
#define MFW_SCRIPTING_API MFW_SHAREDIMPORT
#endif
#define MFW_SCRIPTING_CALL MFW_SHAREDCALL
#else
#define MFW_SCRIPTING_API 
#define MFW_SCRIPTING_CALL 
#endif

namespace mfw
{
	namespace scripting
	{
		namespace interfaces {};
		using namespace interfaces;
		namespace literals {};
		using namespace literals;
	};

	namespace literals
	{
		using namespace scripting::literals;
	};
};

#endif