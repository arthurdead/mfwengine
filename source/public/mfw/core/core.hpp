#ifndef __MFW_PUBLIC_CORE_HPP
#define __MFW_PUBLIC_CORE_HPP

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
	MFW_CORE_API ExitStatus MFW_CORE_CALL initialize() noexcept;
	MFW_CORE_API ExitStatus MFW_CORE_CALL update() noexcept;
	MFW_CORE_API ExitStatus MFW_CORE_CALL shutdown() noexcept;

	MFW_CORE_API void MFW_CORE_CALL terminate() noexcept;

	MFW_CORE_API void MFW_CORE_CALL expandEnvironmentVariables(stl::osstring_view src, stl::osstring &dst) noexcept;

#if MFW_OS_IS(LINUX)
	MFW_CORE_API void MFW_CORE_CALL expandShell(stl::osstring_view src, stl::vector<stl::osstring> &dst) noexcept;
#endif

	MFW_CORE_API stl::int32_t MFW_CORE_CALL getLastError() noexcept;
	MFW_CORE_API void MFW_CORE_CALL getErrorString(stl::int32_t code, stl::osstring &str) noexcept;

	MFW_CORE_API stl::int64_t MFW_CORE_CALL timeNow() noexcept;

	enum class os_layer_t : stl::uchar_t
	{
		native,
	#if MFW_OS_IS(WINDOWS)
		wine,
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
	MFW_CORE_API os_layer_t MFW_CORE_CALL get_os_layer() noexcept;

	inline void getLastErrorString(stl::osstring &str) noexcept {
		stl::int32_t code{getLastError()};
		getErrorString(code, str);
	}
}

#endif