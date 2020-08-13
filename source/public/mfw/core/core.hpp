#ifndef MFW_PUBLIC_CORE_HPP
#define MFW_PUBLIC_CORE_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/string_view.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/stdint.hpp>
#if MFW_OS_IS(LINUX)
	#include <public/mfw/stl/vector.hpp>
#endif
#include <public/mfw/core/application.hpp>

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
	extern MFW_CORE_API exit_status MFW_CORE_CALL initialize() noexcept;
	extern MFW_CORE_API exit_status MFW_CORE_CALL update() noexcept;
	extern MFW_CORE_API exit_status MFW_CORE_CALL shutdown() noexcept;

	extern MFW_CORE_API void MFW_CORE_CALL terminate() noexcept;

	extern MFW_CORE_API void MFW_CORE_CALL expand_environment_variables(stl::osstring_view src, stl::osstring &dst) noexcept;

#if MFW_OS_IS(LINUX)
	extern MFW_CORE_API void MFW_CORE_CALL expand_shell(stl::osstring_view src, stl::vector<stl::osstring> &dst) noexcept;
#endif

	extern MFW_CORE_API stl::int32_t MFW_CORE_CALL get_last_error() noexcept;
	extern MFW_CORE_API void MFW_CORE_CALL get_error_string(stl::int32_t code, stl::osstring &str) noexcept;

	extern MFW_CORE_API stl::int64_t MFW_CORE_CALL time_now() noexcept;

	enum class os_layer_t : stl::uchar_t
	{
		none,
	#if MFW_OS_IS(WINDOWS)
		wine,
		reactos,
	#endif
	#if MFW_OS_IS(MACOS)
		darling,
	#endif
	#if MFW_OS_IS(LINUX)
		wsl,
		cygwin,
	#endif
	#if MFW_OS_IS(ANDROID)
		anbox,
		shashlik,
	#endif
	};
	extern MFW_CORE_API os_layer_t MFW_CORE_CALL get_os_layer() noexcept;

	extern void get_last_error_string(stl::osstring &str) noexcept;

	inline void get_last_error_string(stl::osstring &str) noexcept {
		stl::int32_t code{get_last_error()};
		get_error_string(code, str);
	}
}

#endif