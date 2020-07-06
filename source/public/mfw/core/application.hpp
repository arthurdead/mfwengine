#ifndef __MFW_PUBLIC_CORE_APPLICATION_HPP
#define __MFW_PUBLIC_CORE_APPLICATION_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/string_view.hpp>
#include <public/mfw/stl/array.hpp>
#include <public/mfw/core/searchpath.hpp>

namespace mfw::core
{
	struct MFW_VISIBILITY_LOCAL ExitStatus
	{
	public:
		enum class exit_codes_t : stl::uchar_t
		{
			success = 0,
			fatal = 1,
		};
		MFW_CLASS_ENUM(exit_codes_t)
		
		ExitStatus() noexcept = default;
		ExitStatus(stl::uint8_t code) noexcept
		{ this->code() = code; }
		ExitStatus(exit_codes_t code) noexcept
		{ this->code() = static_cast<stl::uint8_t>(code); }
		
		static const ExitStatus success;
		static const ExitStatus fatal;
	
		stl::uint8_t &code() noexcept
		{ return m_values[values_index::exit_code]; }
		stl::uint8_t code() const noexcept
		{ return m_values[values_index::exit_code]; }

		stl::uint8_t &warnings() noexcept
		{ return m_values[values_index::warnings_]; }
		stl::uint8_t warnings() const noexcept
		{ return m_values[values_index::warnings_]; }

		stl::uint8_t &errors() noexcept
		{ return m_values[values_index::errors_]; }
		stl::uint8_t errors() const noexcept
		{ return m_values[values_index::errors_]; }
		
		bool succeded() const noexcept
		{ return !wasFatal() && errors() == 0; }
		bool absolutelySucceded() const noexcept
		{ return succeded() && warnings() == 0; }
		bool wasFatal() const noexcept
		{ return code() == exit_codes_t::fatal; }
		bool failed() const noexcept
		{ return wasFatal() || errors() > 0; }

		operator bool() const noexcept
		{ return succeded(); }
		bool operator!() const noexcept
		{ return !succeded(); }

		ExitStatus &setFatal() noexcept
		{ m_values[values_index::exit_code] = static_cast<stl::uint8_t>(exit_codes_t::fatal); return *this; }
		ExitStatus &setFailed() noexcept
		{ setFatal(); return *this; }
		
		void append(ExitStatus status) noexcept {
			stl::uint8_t code{status.code()};
			if(code != static_cast<stl::uint8_t>(exit_codes_t::success)) {
				this->code() = code;
			}
			warnings() += status.warnings();
			errors() += status.errors();
		}
		
		ExitStatus &operator+=(ExitStatus status) noexcept {
			append(status);
			return *this;
		}
	
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
	
	MFW_VISIBILITY_LOCAL stl::pstring executablePath() noexcept;
	
#if MFW_BUILD_FLAGGED(EXECUTABLE)
	MFW_VISIBILITY_LOCAL bool coreLoadLibrary(const SearchPath &name) noexcept;
	MFW_VISIBILITY_LOCAL ExitStatus coreUpdate() noexcept;
#endif
}

#if MFW_BUILD_FLAGGED(SHARED)
extern MFW_VISIBILITY_LOCAL ::mfw::core::ExitStatus applicationMain(
	#if MFW_BUILD == MFW_BUILD_SHARED && MFW_OS == MFW_OS_WINDOWS
bool thread
	#endif
) noexcept;

extern MFW_VISIBILITY_LOCAL ::mfw::core::ExitStatus applicationExit(
	#if MFW_BUILD == MFW_BUILD_SHARED && MFW_OS == MFW_OS_WINDOWS
bool thread
	#endif
) noexcept;
#endif

#if MFW_BUILD_FLAGGED(EXECUTABLE)
extern MFW_VISIBILITY_LOCAL bool applicationLoadLibraries() noexcept;
#endif

#if MFW_OS_IS(WEB)
	#define __MFW_OS_TARGET "web"
#elif MFW_OS_IS(LINUX)
	#define __MFW_OS_TARGET "linux"
#elif MFW_OS_IS(ANDROID)
	#define __MFW_OS_TARGET "android"
#elif MFW_OS_IS(WINDOWS)
	#define __MFW_OS_TARGET "windows"
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
#elif MFW_PROCESSOR_IS(WASM)
	#define __MFW_PROCESSOR_TARGET "wasm"
#else
	#error
#endif

#define __MFW_TARGET_TRIPLE __MFW_OS_TARGET "_" __MFW_PROCESSOR_TARGET "_" __MFW_CONFIGURATION_TARGET

#endif