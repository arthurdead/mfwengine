#ifndef __MFW_PUBLIC_CORE_APPLICATION_H
#define __MFW_PUBLIC_CORE_APPLICATION_H

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/string_view.hpp>
#include <public/mfw/core/searchpath.hpp>

namespace mfw::core
{
	struct exit_status
	{
	public:
		enum class exit_codes : uchar_t
		{
			success,
			fatal,
		};
		MFW_CLASS_ENUM(exit_codes)
		
		exit_status() = default;
		exit_status(uint8_t code, uint8_t warnings = {}, uint8_t errors = {})
			{ set_code(code); add_warning(warnings); add_error(errors); }
		exit_status(exit_codes code, uint8_t warnings = {}, uint8_t errors = {})
			: exit_status{static_cast<uint8_t>(code), warnings, errors} {}
		
		static const exit_status success;
		static const exit_status fatal;
	
		uint8_t warnings() const { return values[2]; }
		uint8_t errors() const { return values[3]; }
		uint8_t code() const { return values[1]; }
		
		bool succeded() const { return !was_fatal() && errors() == 0; }
		bool absolutely_succeded() const { return succeded() && warnings() == 0; }
		bool was_fatal() const { return code() == exit_codes::fatal; }
		bool failed() const { return was_fatal() || errors() > 0; }

		operator bool() const { return succeded(); }
		bool operator!() const { return !succeded(); }

		void set_fatal() { set_code(static_cast<uint8_t>(exit_codes::fatal)); }
		void set_failed(bool err=false) {
			set_fatal();
			if(err) {
				add_error();
			}
		}
		void set_code(uint8_t code) { values[1] = code; }
		void add_error(uint8_t c=uint8_t{1}) { values[3] += c; }
		void add_warning(uint8_t c=uint8_t{1}) { values[2] += c; }
		
		void append(exit_status status) {
			uint8_t code{status.code()};
			if(code != 0) {
				set_code(code);
			}
			add_warning(status.warnings());
			add_error(status.errors());
		}
		
		exit_status &operator+=(exit_status status) {
			append(status);
			return *this;
		}
	
		uint8_t values[4];
	};
	
	pstring executable_path();
	
#if MFW_BUILD & MFW_BUILD_EXECUTABLE_FLAG
	bool core_load_library(const searchpath &name);
	exit_status core_update();
#endif
}

#if MFW_BUILD & MFW_BUILD_SHARED_FLAG
extern MFW_SHARED_LOCAL ::mfw::core::exit_status application_main(
	#if MFW_BUILD == MFW_BUILD_SHARED && MFW_OS == MFW_OS_WINDOWS
bool thread
	#endif
);

extern MFW_SHARED_LOCAL ::mfw::core::exit_status application_exit(
	#if MFW_BUILD == MFW_BUILD_SHARED && MFW_OS == MFW_OS_WINDOWS
bool thread
	#endif
);
#endif

#if MFW_BUILD & MFW_BUILD_EXECUTABLE_FLAG
extern bool application_load_libraries();
#endif

#if MFW_OS == MFW_OS_LINUX
	#define __MFW_OS_TARGET "linux"
#elif MFW_OS == MFW_OS_WINDOWS
	#define __MFW_OS_TARGET "windows"
#else
	#error
#endif

#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
	#define __MFW_CONFIGURATION_TARGET "debug"
#else
	#define __MFW_CONFIGURATION_TARGET "release"
#endif

#if MFW_PROCESSOR == MFW_PROCESSOR_X86_64
	#define __MFW_PROCESSOR_TARGET "x86_64"
#elif MFW_PROCESSOR == MFW_PROCESSOR_X86
	#define __MFW_PROCESSOR_TARGET "x86"
#else
	#error
#endif

#define __MFW_TARGET_TRIPLE __MFW_OS_TARGET "_" __MFW_PROCESSOR_TARGET "_" __MFW_CONFIGURATION_TARGET

#endif