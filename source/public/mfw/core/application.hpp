#ifndef MFW_PUBLIC_CORE_APPLICATION_HPP
#define MFW_PUBLIC_CORE_APPLICATION_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/string_view.hpp>
#include <public/mfw/stl/array.hpp>
#include <public/mfw/core/searchpath.hpp>

namespace mfw::core
{
	MFW_VISIBILITY_LOCAL_PUSH()

	struct exit_status final
	{
	public:
		enum class exit_codes : stl::uchar_t
		{
			success = 0,
			fatal = 1,
		};
		MFW_CLASS_ENUM(exit_codes)
		
		exit_status() noexcept = default;
		exit_status(stl::uint8_t code, stl::uint8_t warnings = {}, stl::uint8_t errors = {}) noexcept;
		exit_status(exit_codes code, stl::uint8_t warnings = {}, stl::uint8_t errors = {}) noexcept;
		
		static const exit_status success;
		static const exit_status fatal;
	
		stl::uint8_t &code() noexcept;
		stl::uint8_t code() const noexcept;

		stl::uint8_t &warnings() noexcept;
		stl::uint8_t warnings() const noexcept;

		stl::uint8_t &errors() noexcept;
		stl::uint8_t errors() const noexcept;
		
		bool succeded() const noexcept;
		bool absolutelySucceded() const noexcept;
		bool wasFatal() const noexcept;
		bool failed() const noexcept;

		operator bool() const noexcept;
		bool operator!() const noexcept;

		exit_status &set_fatal() noexcept;
		exit_status &set_failed() noexcept;
		
		exit_status &append(exit_status status) noexcept;
		
		exit_status &operator+=(exit_status status) noexcept;
	
	private:
		enum /*class*/ values_index : stl::uchar_t
		{
			exit_code,
			warnings_,
			errors_,
			reserved1,
			count,
		};
		stl::array<stl::uint8_t, values_index::count> m_values{};
	};
	
	extern stl::pstring executable_path() noexcept;
	
#if MFW_BUILD_FLAGGED(EXECUTABLE)
	extern bool core_load_library(const searchpath &name) noexcept;
	extern exit_status core_update() noexcept;
#endif

	MFW_VISIBILITY_LOCAL_POP()
}

MFW_VISIBILITY_LOCAL_PUSH()

#if MFW_BUILD_FLAGGED(SHARED)
extern ::mfw::core::exit_status application_main(
	#if MFW_BUILD == MFW_BUILD_SHARED && MFW_OS == MFW_OS_WINDOWS
bool thread
	#endif
) noexcept;

extern ::mfw::core::exit_status application_exit(
	#if MFW_BUILD == MFW_BUILD_SHARED && MFW_OS == MFW_OS_WINDOWS
bool thread
	#endif
) noexcept;
#endif

#if MFW_BUILD_FLAGGED(EXECUTABLE)
extern bool application_load_libraries() noexcept;
#endif

MFW_VISIBILITY_LOCAL_POP()

#if MFW_OS_IS(WASI)
	#define __MFW_OS_TARGET "wasi"
#elif MFW_OS_IS(LINUX)
	#define __MFW_OS_TARGET "linux"
#elif MFW_OS_IS(ANDROID)
	#define __MFW_OS_TARGET "android"
#elif MFW_OS_IS(WINDOWS)
	#define __MFW_OS_TARGET "windows"
#elif MFW_OS_IS(MACOS)
	#define __MFW_OS_TARGET "macos"
#else
	#error
#endif

#if MFW_CONFIGURATION_IS(DEBUG)
	#define __MFW_CONFIGURATION_TARGET "debug"
#elif MFW_CONFIGURATION_IS(RELEASE)
	#define __MFW_CONFIGURATION_TARGET "release"
#else
	#error
#endif

#if MFW_PROCESSOR_IS(X86_64)
	#define __MFW_PROCESSOR_TARGET "x86_64"
#elif MFW_PROCESSOR_IS(X86)
	#define __MFW_PROCESSOR_TARGET "x86"
#elif MFW_PROCESSOR_IS(ARM)
	#define __MFW_PROCESSOR_TARGET "arm"
#elif MFW_PROCESSOR_IS(AARCH64)
	#define __MFW_PROCESSOR_TARGET "aarch64"
#elif MFW_PROCESSOR_IS(WASM64)
	#define __MFW_PROCESSOR_TARGET "wasm64"
#elif MFW_PROCESSOR_IS(WASM64)
	#define __MFW_PROCESSOR_TARGET "wasm32"
#elif MFW_PROCESSOR_IS(ASMJS)
	#define __MFW_PROCESSOR_TARGET "asmjs"
#else
	#error
#endif

#define __MFW_TARGET_TRIPLE __MFW_OS_TARGET "_" __MFW_PROCESSOR_TARGET "_" __MFW_CONFIGURATION_TARGET

#endif