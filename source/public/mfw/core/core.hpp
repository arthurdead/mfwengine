#ifndef __MFW_PUBLIC_CORE_H
#define __MFW_PUBLIC_CORE_H

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/string_view.hpp>
#include <public/mfw/stl/string.hpp>
#if MFW_OS == MFW_OS_LINUX
	#include <public/mfw/stl/vector.hpp>
#endif

#if MFW_CORE_BUILD & MFW_BUILD_SHARED_FLAG
	#ifdef MFW_BUILDING_CORE
		#define MFW_CORE_API MFW_SHARED_EXPORT
	#else
		#define MFW_CORE_API MFW_SHARED_IMPORT
	#endif
	#define MFW_CORE_CALL MFW_CALL_SHARED
#else
	#define MFW_CORE_API 
	#define MFW_CORE_CALL 
#endif

namespace mfw::core
{
	struct exit_status;

	MFW_CORE_API exit_status MFW_CORE_CALL initialize();
	MFW_CORE_API exit_status MFW_CORE_CALL update();
	MFW_CORE_API exit_status MFW_CORE_CALL shutdown();

	MFW_CORE_API void MFW_CORE_CALL terminate();

	MFW_CORE_API void MFW_CORE_CALL expand_env_vars(const ucstring_view &src, ucstring &dst);

#if MFW_OS == MFW_OS_LINUX
	MFW_CORE_API void MFW_CORE_CALL expand_shell(const ucstring_view &src, vector<ucstring> &dst);
#endif

	MFW_CORE_API int32_t MFW_CORE_CALL get_last_error();
	MFW_CORE_API void MFW_CORE_CALL get_error_string(int32_t code, ucstring &str);

	MFW_CORE_API int64_t MFW_CORE_CALL time_now();

	inline void get_last_error_string(ucstring &str)
	{
		int32_t code{get_last_error()};
		get_error_string(code, str);
	}
}

#endif