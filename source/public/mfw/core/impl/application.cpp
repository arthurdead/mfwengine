#include <public/mfw/core/application.hpp>
#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/core/core.hpp>
#if MFW_BUILD_FLAGGED(EXECUTABLE)
	#include <public/mfw/core/filesystem_interface.hpp>
	#include <public/mfw/core/library.hpp>
#endif
#if MFW_OS_IS(WINDOWS)
	#include <public/mfw/stl/windows/windows.hpp>
	#include <delayimp.h>
#elif MFW_OS_IS(LINUX)
	#include <unistd.h>
	#include <dlfcn.h>
	#include <public/mfw/stl/limits.hpp>
#endif

#if MFW_CORE_BUILD & MFW_BUILD_STATIC_FLAG || defined __MFW_CORE_USING_IMPLIB || !defined __MFW_CORE_IS_DELAY_LOADED
	#define _MFW_APPLICATION_CORE_AVAILABLE
#endif

#if defined __MFW_CORE_IS_DELAY_LOADED && defined __MFW_CORE_USING_IMPLIB
extern "C" MFW_VISIBILITY_LOCAL void __core_tramp_resolve_all() MFW_NOTHROW;
#endif

#ifdef _MFW_APPLICATION_CORE_AVAILABLE
	#if MFW_OS_IS(LINUX) && !defined __MFW_CORE_USING_IMPLIB
		#define _MFW_LOGGING_ENABLED
	#elif MFW_OS_IS(WINDOWS)
		#define _MFW_LOGGING_ENABLED
	#endif
#endif

#if defined _MFW_LOGGING_ENABLED
	#include <public/mfw/core/logging_interface.hpp>
#endif

#if MFW_COMPILER_FLAGGED(CLANG)
	MFW_WARNING_DISABLE_UNIX("-Wmissing-prototypes")
	MFW_WARNING_DISABLE_UNIX("-Wmissing-variable-declarations")
#endif

namespace mfw::core
{
	const exit_status exit_status::success{exit_status::exit_codes::success};
	const exit_status exit_status::fatal{exit_status::exit_codes::fatal};

	exit_status::exit_status(stl::uint8_t code, stl::uint8_t warnings, stl::uint8_t errors) noexcept {
		m_values[values_index::exit_code] = code;
		m_values[values_index::warnings_] = warnings;
		m_values[values_index::errors_] = errors;
	}
	exit_status::exit_status(exit_codes code, stl::uint8_t warnings, stl::uint8_t errors) noexcept
		: exit_status{static_cast<stl::uint8_t>(code), warnings, errors} {}

	stl::uint8_t &exit_status::code() noexcept
	{ return m_values[values_index::exit_code]; }
	stl::uint8_t exit_status::code() const noexcept
	{ return m_values[values_index::exit_code]; }

	stl::uint8_t &exit_status::warnings() noexcept
	{ return m_values[values_index::warnings_]; }
	stl::uint8_t exit_status::warnings() const noexcept
	{ return m_values[values_index::warnings_]; }

	stl::uint8_t &exit_status::errors() noexcept
	{ return m_values[values_index::errors_]; }
	stl::uint8_t exit_status::errors() const noexcept
	{ return m_values[values_index::errors_]; }
	
	bool exit_status::succeded() const noexcept
	{ return !was_fatal() && errors() == 0; }
	bool exit_status::absolutely_succeded() const noexcept
	{ return succeded() && warnings() == 0; }
	bool exit_status::was_fatal() const noexcept
	{ return code() == exit_codes::fatal; }
	bool exit_status::failed() const noexcept
	{ return was_fatal() || errors() > 0; }

	exit_status::operator bool() const noexcept
	{ return succeded(); }
	bool exit_status::operator!() const noexcept
	{ return !succeded(); }

	exit_status &exit_status::set_fatal() noexcept
	{ m_values[values_index::exit_code] = static_cast<stl::uint8_t>(exit_codes::fatal); return *this; }
	exit_status &exit_status::set_failed() noexcept
	{ set_fatal(); return *this; }
	
	exit_status &exit_status::append(exit_status status) noexcept {
		stl::uint8_t code{status.code()};
		if(code != static_cast<stl::uint8_t>(exit_codes::success)) {
			this->code() = code;
		}
		warnings() += status.warnings();
		errors() += status.errors();
		return *this;
	}
	
	exit_status &exit_status::operator+=(exit_status status) noexcept {
		append(status);
		return *this;
	}
	
	namespace __public_application_cpp_internal
	{
	#if MFW_BUILD_FLAGGED(SHARED)
		#if defined _MFW_LOGGING_ENABLED
		MFW_DECLARE_LOG_CONTEXT(log_application, MFW_T("core/application"_p))
		#endif
	
		template <typename... Args>
		static MFW_VISIBILITY_LOCAL void print(stl::osstring_view fmt, Args... args) noexcept
		{
		#if defined _MFW_LOGGING_ENABLED
			log_application().print(fmt, stl::forward<Args>(args)...);
		#else
			stl::osstring str{};
			stl::format(str, fmt, forward<Args>(args)...);
			str.append(1, static_cast<stl::oschar_t>('\n'));
			::MFW_STD_NAMESPACE::fputs(str.c_str(), ::MFW_STD_NAMESPACE::stdout);
		#endif
		}
	#endif
	}
	
#if MFW_BUILD_FLAGGED(EXECUTABLE)
	namespace __public_application_cpp_internal
	{
	#ifndef _MFW_APPLICATION_CORE_AVAILABLE
		using core_load_library_t = decltype(core::library::load_library) *;
		static core_load_library_t core_load_library_ptr{nullptr};
		
		using core_update_t = decltype(update) *;
		static core_update_t core_update_ptr{nullptr};
		
		using core_initialize_t = decltype(initialize) *;
		static core_initialize_t core_initialize_ptr{nullptr};
		
		using core_shutdown_t = decltype(shutdown) *;
		static core_shutdown_t core_shutdown_ptr{nullptr};
		
		using core_filesystem_instance_t = decltype(filesystem::instance) *;
		static core_filesystem_instance_t core_filesystem_instance_ptr{nullptr};
	#endif

		MFW_VISIBILITY_LOCAL_PUSH()
	
		static filesystem &core_filesystem_instance() noexcept
		{
		#ifdef _MFW_APPLICATION_CORE_AVAILABLE
			return filesystem::instance();
		#else
			return core_filesystem_instance_ptr();
		#endif
		}
	
		static exit_status core_initialize() noexcept
		{
		#ifdef _MFW_APPLICATION_CORE_AVAILABLE
			return initialize();
		#else
			return core_initialize_ptr();
		#endif
		}
		
		static exit_status coreShutdown() noexcept
		{
		#ifdef _MFW_APPLICATION_CORE_AVAILABLE
			return shutdown();
		#else
			return core_shutdown_ptr();
		#endif
		}

		MFW_VISIBILITY_LOCAL_POP()
	}

	MFW_VISIBILITY_LOCAL_PUSH()
	
	stl::pstring executable_path() noexcept
	{
	#if MFW_OS_IS(WINDOWS)
		wchar_t exefile[MAX_PATH]{L'\0'};
		stl::size_t len{GetModuleFileNameW(nullptr, exefile, stl::size(exefile))};
	#elif MFW_OS_IS(LINUX)
		stl::osstring exefile{};
		stl::ssize_t len{-1};
		while(len == -1) {
			exefile.resize(exefile.size()+1);
			len = readlink("/proc/self/exe", exefile.data(), exefile.size());
		}
	#else
		#error
	#endif
	
		stl::pstring exepath{};
		exepath.assign(exefile);
		return exepath;
	}
	
	bool core_load_library(const searchpath &name) noexcept
	{
		::mfw::core::Library *lib{
		#ifdef _MFW_APPLICATION_CORE_AVAILABLE
			::mfw::core::library::load_library(name)
		#else
			__public_application_cpp_internal::core_load_library_ptr(name)
		#endif
		};
		return lib != nullptr;
	}
	
	exit_status core_update() noexcept
	{
	#ifdef _MFW_APPLICATION_CORE_AVAILABLE
		return ::mfw::core::update();
	#else
		return __public_application_cpp_internal::core_update_ptr();
	#endif
	}

	MFW_VISIBILITY_LOCAL_POP()
#endif
	
	namespace __public_application_cpp_internal
	{
		MFW_VISIBILITY_LOCAL_PUSH()

	#if MFW_BUILD_FLAGGED(EXECUTABLE)
		#if MFW_CORE_BUILD == MFW_BUILD_SHARED && defined __MFW_CORE_IS_DELAY_LOADED
			#if MFW_OS_IS(LINUX)
		static void *core_lib_dlopen() noexcept
		{
			void *core_dl{dlmopen(LM_ID_BASE, "core/bin/" __MFW_TARGET_TRIPLE "/core.so", RTLD_LAZY|RTLD_GLOBAL)};
			if(!core_dl) {
				stl::osstring reason{};
				reason.assign(dlerror());
				__public_application_cpp_internal::print(MFW_T("could not load core library: {}"_sv), reason);
			}
			return core_dl;
		}
			#endif
	
		static bool load_core_lib() noexcept
		{
			#if MFW_OS_IS(WINDOWS)
			if(!LoadLibraryExW(MFW_T("core/bin/" __MFW_TARGET_TRIPLE L"/core.dll"), nullptr, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS)) {
				__public_application_cpp_internal::print(MFW_T("could not load core library"_sv));
				return false;
			}
			#elif MFW_OS == MFW_OS_LINUX
			void *core_dl{core_lib_dlopen()};
			if(core_dl) {
				#ifdef __MFW_CORE_USING_IMPLIB
				//__core_tramp_resolve_all();
				#else
				constexpr const char *core_load_library_name{"_ZN3mfw4core7library12load_libraryERKNS0_10searchpathE"};
				core_load_library_ptr = reinterpret_cast<core_load_library_t>(dlsym(core_dl, core_load_library_name));
				if(!core_load_library_ptr) {
					__public_application_cpp_internal::print(u8"core library missing library::load_library symbol"_sv);
					return false;
				}
				
				constexpr const char *core_update_name{"_ZN3mfw4core6updateEv"};
				core_update_ptr = reinterpret_cast<core_update_t>(dlsym(core_dl, core_update_name));
				if(!core_update_ptr) {
					__public_application_cpp_internal::print(u8"core library missing update symbol"_sv);
					return false;
				}
				
				constexpr const char *core_initialize_name{"_ZN3mfw4core10initializeEv"};
				core_initialize_ptr = reinterpret_cast<core_initialize_t>(dlsym(core_dl, core_initialize_name));
				if(!core_initialize_ptr) {
					__public_application_cpp_internal::print(u8"core library missing initialize symbol"_sv);
					return false;
				}
				
				constexpr const char *core_shutdown_name{"_ZN3mfw4core8shutdownEv"};
				core_shutdown_ptr = reinterpret_cast<core_shutdown_t>(dlsym(core_dl, core_shutdown_name));
				if(!core_shutdown_ptr) {
					__public_application_cpp_internal::print(u8"core library missing shutdown symbol"_sv);
					return false;
				}
				
				constexpr const char *core_filesystem_instance_name{"_ZN3mfw4core10interfaces10filesystem8instanceEv"};
				core_filesystem_instance_ptr = reinterpret_cast<core_filesystem_instance_t>(dlsym(core_dl, core_filesystem_instance_name));
				if(!core_filesystem_instance_ptr) {
					__public_application_cpp_internal::print(u8"core library missing interfaces::filesystem::instance symbol"_sv);
					return false;
				}
				#endif
			}
			return core_dl != nullptr;
			#else
				#error
			#endif
		}
		#endif

		static bool load_libraries() noexcept
		{
		#if MFW_CORE_BUILD == MFW_BUILD_SHARED && defined __MFW_CORE_IS_DELAY_LOADED
			if(!load_core_lib()) {
				return false;
			}
		#endif
			
			if(!application_load_libraries()) {
				return false;
			}
			
			return true;
		}
	#endif

	#if MFW_BUILD_FLAGGED(SHARED)
		static exit_status call_exit(
		#if MFW_BUILD_IS(SHARED) && MFW_OS_IS(WINDOWS)
		bool thread
		#endif
		) noexcept
		{
			exit_status status{application_exit(
		#if MFW_BUILD_IS(SHARED) && MFW_OS_IS(WINDOWS)
			thread
		#endif
			)};

			return status;
		}

		static exit_status call_main(
		#if MFW_BUILD_IS(SHARED) && MFW_OS_IS(WINDOWS)
		bool thread
		#endif
		) noexcept
		{
			exit_status main_status{};

		#if MFW_BUILD_FLAGGED(EXECUTABLE)
			if(!load_libraries()) {
				main_status.set_failed();
				return main_status;
			}
			
			main_status = core_initialize();
		#endif

			if(main_status.succeded()) {
				main_status += application_main(
			#if MFW_BUILD_IS(SHARED) && MFW_OS_IS(WINDOWS)
				thread
			#endif
				);
			}

		#if MFW_BUILD_IS(SHARED)
			return main_status;
		#elif MFW_BUILD_FLAGGED(EXECUTABLE)
			exit_status exit{call_exit()};
			main_status += exit;

			#ifdef _MFW_APPLICATION_CORE_AVAILABLE
			main_status.errors() += error_count();
			main_status.warnings() += warning_count();
			#endif

			#ifdef _MFW_LOGGING_ENABLED
			if(main_status.absolutely_succeded()) {
				log_application().set_severity(log_context::severity::success);
			} else if(main_status.succeded()) {
				log_application().set_severity(log_context::severity::warning);
			} else {
				log_application().set_severity(log_context::severity::error);
			}
			#endif
			__public_application_cpp_internal::print(MFW_T("exited with code: {} [warnings: {}, errors: {}]"_sv), main_status.code(), main_status.warnings(), main_status.errors());

			exit_status core_status{core_shutdown()};
			main_status += core_status;

			return main_status;
		#endif
		}
	#endif

		MFW_VISIBILITY_LOCAL_POP()
	}
}

MFW_VISIBILITY_LOCAL_PUSH()

#if MFW_BUILD_IS(SHARED) && !defined MFW_APPLICATION_MAIN_DEFINED
::mfw::core::exit_status application_main(
	#if MFW_BUILD_IS(SHARED) && MFW_OS_IS(WINDOWS)
bool thread
	#endif
) noexcept
{
	return ::mfw::core::exit_status::success;
}

::mfw::core::exit_status application_exit(
	#if MFW_BUILD_IS(SHARED) && MFW_OS_IS(WINDOWS)
bool thread
	#endif
) noexcept
{
	return ::mfw::core::exit_status::success;
}
#endif

#if MFW_BUILD_IS(SHARED)
	#if MFW_OS_IS(WINDOWS)
::mfw::stl::int32_t MFW_CALL_SHARED DllMain(_In_ HINSTANCE hinstDLL, _In_ ::mfw::stl::uint32_t fdwReason, _In_ const void *lpvReserved) MFW_NOTHROW
{
	::mfw::core::exit_status status{};

	switch(fdwReason) {
		case DLL_THREAD_ATTACH: {
			status = ::mfw::core::__public_application_cpp_internal::call_main(true);
			break;
		}
		case DLL_PROCESS_ATTACH: {
			status = ::mfw::core::__public_application_cpp_internal::call_main(false);
			break;
		}
		case DLL_THREAD_DETACH: {
			status = ::mfw::core::__public_application_cpp_internal::call_exit(true);
			break;
		}
		case DLL_PROCESS_DETACH: {
			status = ::mfw::core::__public_application_cpp_internal::call_exit(false);
			break;
		}
	}

	if(status.succeded()) {
		return 1;
	} else {
		return 0;
	}
}
	#else
MFW_ATTRIBUTE(__constructor__(101)) void MFW_CALL_SHARED __shared_start() noexcept
{
	::mfw::core::__public_application_cpp_internal::call_main();
}

MFW_ATTRIBUTE(__destructor__(101)) void MFW_CALL_SHARED __shared_exit() noexcept
{
	::mfw::core::__public_application_cpp_internal::call_exit();
}
	#endif
#elif MFW_BUILD_FLAGGED(EXECUTABLE)
	#if MFW_OS_IS(WINDOWS)
		#if MFW_CHARACTERSET_IS(UNICODE)
			#if MFW_BUILD_IS(EXECUTABLE)
::mfw::stl::int32_t MFW_CALL_SHARED wmain(::mfw::stl::int32_t argc, const wchar_t *argv[]) MFW_NOTHROW
{
	::mfw::core::exit_status status{::mfw::core::__public_application_cpp_internal::call_main()};
	return status.code();
}
			#elif MFW_BUILD_IS(EXECUTABLE_WINDOWS)
::mfw::stl::int32_t MFW_CALL_SHARED wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ wchar_t *lpCmdLine, _In_ ::mfw::stl::int32_t nCmdShow) MFW_NOTHROW
{
	::mfw::core::exit_status status{::mfw::core::__public_application_cpp_internal::call_main()};
	return status.code();
}
			#else
				#error
			#endif
		#elif MFW_CHARACTERSET_IS(MULTIBYTE)
			#if MFW_BUILD_IS(EXECUTABLE)
::mfw::stl::int32_t MFW_CALL_SHARED main(::mfw::stl::int32_t argc, const char *argv[]) MFW_NOTHROW
{
	::mfw::core::exit_status status{::mfw::core::__public_application_cpp_internal::call_main()};
	return status.code();
}
			#elif MFW_BUILD_IS(EXECUTABLE_WINDOWS)
::mfw::stl::int32_t MFW_CALL_SHARED WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ char *lpCmdLine, _In_ ::mfw::stl::int32_t nCmdShow) MFW_NOTHROW
{
	::mfw::core::exit_status status{::mfw::core::__public_application_cpp_internal::call_main()};
	return status.code();
}
			#else
				#error
			#endif
		#else
			#error
		#endif
	#elif MFW_OS_IS(LINUX)
::mfw::stl::int32_t MFW_CALL_SHARED main(::mfw::stl::int32_t, const char *[], const char *[]) MFW_NOTHROW
{
	::mfw::core::exit_status status{::mfw::core::__public_application_cpp_internal::call_main()};
	return status.code();
}
	#else
		#error
	#endif
#elif !(MFW_BUILD_FLAGGED(STATIC))
	#error
#endif

MFW_VISIBILITY_LOCAL_POP()

#if MFW_BUILD_FLAGGED(EXECUTABLE)
extern "C" {
	MFW_MESSAGE("move this somewhere else")
	MFW_SHARED_EXPORT ::mfw::stl::uint32_t NvOptimusEnablement{0x00000001};
	MFW_SHARED_EXPORT ::mfw::stl::int32_t AmdPowerXpressRequestHighPerformance{1};
}
	#if MFW_OS_IS(LINUX) && defined __MFW_CORE_IS_DELAY_LOADED && defined __MFW_CORE_USING_IMPLIB
extern "C" MFW_VISIBILITY_LOCAL void *__load_core_lib(const char *) MFW_NOTHROW
{ return ::mfw::core::__public_application_cpp_internal::core_lib_dlopen(); }
	#endif
	#if MFW_OS_IS(WINDOWS) && MFW_CONFIGURATION_IS(DEBUG)
namespace mfw::core::__public_application_cpp_internal
{
	static FARPROC MFW_CALL_SHARED dll_failed_hook(uint32_t dliNotify, PDelayLoadInfo pdli) noexcept
	{
		MFW_DEBUGBREAK();
		return nullptr;
	}
	
	static FARPROC MFW_CALL_SHARED dll_notify_hook(uint32_t dliNotify, PDelayLoadInfo pdli) noexcept
	{
		if(pdli->dwLastError == ERROR_SUCCESS) {
			return nullptr;
		}

		MFW_DEBUGBREAK();
		return nullptr;
	}
}
extern "C" {
	const PfnDliHook __pfnDliFailureHook2{::mfw::core::__public_application_cpp_internal::dll_failed_hook};
	const PfnDliHook __pfnDliNotifyHook2{::mfw::core::__public_application_cpp_internal::dll_notify_hook};
}
	#endif
#endif