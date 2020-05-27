#include <public/mfw/core/application.hpp>
#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/iterator.hpp>
#include <public/mfw/core/core.hpp>
#if MFW_BUILD & MFW_BUILD_EXECUTABLE_FLAG
	#include <public/mfw/core/filesystem_interface.hpp>
	#include <public/mfw/core/library.hpp>
#endif
#if MFW_OS == MFW_OS_WINDOWS
	#include <Windows.h>
	#include <delayimp.h>
#elif MFW_OS == MFW_OS_LINUX
	#include <unistd.h>
	#include <climits>
	#include <dlfcn.h>
#endif

#if MFW_OS == MFW_OS_WINDOWS || MFW_CORE_BUILD & MFW_BUILD_STATIC_FLAG
	#define __MFW_APPLICATION_CORE_AVAILABLE
#endif

#ifdef __MFW_APPLICATION_CORE_AVAILABLE
	#define __MFW_APPLICATION_ENABLE_LOGGING
#endif

#ifdef __MFW_APPLICATION_ENABLE_LOGGING
	#if MFW_BUILD & MFW_BUILD_SHARED_FLAG
		#include <public/mfw/core/logging_interface.hpp>
	#endif
#endif

#if MFW_COMPILER == MFW_COMPILER_CLANG
	MFW_WARNING_DISABLE("-Wmissing-prototypes")
	MFW_WARNING_DISABLE("-Wmissing-variable-declarations")
#endif

namespace mfw::core
{
	const exit_status exit_status::success{exit_status::exit_codes::success};
	const exit_status exit_status::fatal{exit_status::exit_codes::fatal};
	
#if MFW_BUILD & MFW_BUILD_EXECUTABLE_FLAG
	namespace __application_internal
	{
	#ifndef __MFW_APPLICATION_CORE_AVAILABLE
		using core_load_library_t = decltype(core::library::load_library) *;
		static core_load_library_t core_load_library_ptr{nullptr};
		
		using core_update_t = decltype(update) *;
		static core_update_t core_update_ptr{nullptr};
		
		using core_initialize_t = decltype(initialize) *;
		static core_initialize_t core_initialize_ptr{nullptr};
		
		using core_shutdown_t = decltype(shutdown) *;
		static core_shutdown_t core_shutdown_ptr{nullptr};
		
		using core_filesystem_instance_t = decltype(interfaces::filesystem::instance) *;
		static core_filesystem_instance_t core_filesystem_instance_ptr{nullptr};
	#endif
	
		static interfaces::filesystem &core_filesystem_instance()
		{
		#ifdef __MFW_APPLICATION_CORE_AVAILABLE
			return interfaces::filesystem::instance();
		#else
			return core_filesystem_instance_ptr();
		#endif
		}
	
		static exit_status core_initialize()
		{
		#ifdef __MFW_APPLICATION_CORE_AVAILABLE
			return initialize();
		#else
			return core_initialize_ptr();
		#endif
		}
		
		static exit_status core_shutdown()
		{
		#ifdef __MFW_APPLICATION_CORE_AVAILABLE
			return shutdown();
		#else
			return core_shutdown_ptr();
		#endif
		}
	}
	
	pstring executable_path()
	{
	#if MFW_OS == MFW_OS_WINDOWS
		wchar_t exefile[MAX_PATH]{L'\0'};
		size_t len{GetModuleFileNameW(nullptr, exefile, size(exefile))};
	#elif MFW_OS == MFW_OS_LINUX
		char exefile[PATH_MAX]{'\0'};
		ssize_t len{readlink("/proc/self/exe", exefile, size(exefile))};
	#else
		#error
	#endif
	
		pstring exepath{MFW_PATH_FROM_CHARARRAY(exefile, len)};
		return exepath;
	}
	
	bool core_load_library(ucstring_view name)
	{
		return
	#ifdef __MFW_APPLICATION_CORE_AVAILABLE
		::mfw::core::library::load_library(name)
	#else
		__application_internal::core_load_library_ptr(name)
	#endif
		!= nullptr;
	}
	
	exit_status core_update()
	{
	#ifdef __MFW_APPLICATION_CORE_AVAILABLE
		return ::mfw::core::update();
	#else
		return __application_internal::core_update_ptr();
	#endif
	}
#endif
	
	namespace __application_internal
	{
	#if MFW_BUILD & MFW_BUILD_EXECUTABLE_FLAG
		#if MFW_OS == MFW_OS_WINDOWS
			#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
		static FARPROC MFW_CALL_SHARED dll_failed_hook(uint32_t dliNotify, PDelayLoadInfo pdli)
		{
			MFW_DEBUGBREAK();
			return nullptr;
		}
		static FARPROC MFW_CALL_SHARED dll_notify_hook(uint32_t dliNotify, PDelayLoadInfo pdli)
		{
			if(pdli->dwLastError == ERROR_SUCCESS) {
				return nullptr;
			}

			MFW_DEBUGBREAK();
			return nullptr;
		}
			#endif
		#endif

		static bool load_libraries()
		{
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

			MFW_MESSAGE("get rid of the above")

		#if MFW_CORE_BUILD == MFW_BUILD_SHARED
			#if MFW_OS == MFW_OS_WINDOWS
			if(!LoadLibraryExW(L"core/bin/" __MFW_OS_TARGET L"_" __MFW_PROCESSOR_TARGET L"_" __MFW_CONFIGURATION_TARGET L"/core.dll", nullptr, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS)) {
				return false;
			}
			#elif MFW_OS == MFW_OS_LINUX
			void *core_dl{dlopen("core/bin/" __MFW_OS_TARGET "_" __MFW_PROCESSOR_TARGET "_" __MFW_CONFIGURATION_TARGET "/core.so", RTLD_NOW|RTLD_GLOBAL|RTLD_DEEPBIND)};
				#ifndef __MFW_APPLICATION_CORE_AVAILABLE
			if(core_dl) {
				const char *core_load_library_name{"_ZN3mfw4core7library12load_libraryERKNS0_10searchpathE"};
				core_load_library_ptr = reinterpret_cast<core_load_library_t>(dlsym(core_dl, core_load_library_name));
				if(!core_load_library_ptr) {
					return false;
				}
				
				const char *core_update_name{"_ZN3mfw4core6updateEv"};
				core_update_ptr = reinterpret_cast<core_update_t>(dlsym(core_dl, core_update_name));
				if(!core_update_ptr) {
					return false;
				}
				
				const char *core_initialize_name{"_ZN3mfw4core10initializeEv"};
				core_initialize_ptr = reinterpret_cast<core_initialize_t>(dlsym(core_dl, core_initialize_name));
				if(!core_initialize_ptr) {
					return false;
				}
				
				const char *core_shutdown_name{"_ZN3mfw4core8shutdownEv"};
				core_shutdown_ptr = reinterpret_cast<core_shutdown_t>(dlsym(core_dl, core_shutdown_name));
				if(!core_shutdown_ptr) {
					return false;
				}
				
				const char *core_filesystem_instance_name{"_ZN3mfw4core10interfaces10filesystem8instanceEv"};
				core_filesystem_instance_ptr = reinterpret_cast<core_filesystem_instance_t>(dlsym(core_dl, core_filesystem_instance_name));
				if(!core_filesystem_instance_ptr) {
					return false;
				}
			} else {
				return false;
			}
				#endif
			#else
				#error
			#endif
		#endif

		#if MFW_OS == MFW_OS_WINDOWS
			wchar_t exefile[MAX_PATH]{L'\0'};
			size_t len{GetModuleFileNameW(nullptr, exefile, size(exefile))};
		#elif MFW_OS == MFW_OS_LINUX
			char exefile[PATH_MAX]{'\0'};
			ssize_t len{readlink("/proc/self/exe", exefile, size(exefile))};
		#else
			#error
		#endif

			pstring exepath{executable_path()};
			exepath.remove_filename();
			__application_internal::core_filesystem_instance().initialize(exepath);

			if(!application_load_libraries()) {
				return false;
			}
			
			return true;
		}
	#endif

	#if MFW_BUILD & MFW_BUILD_SHARED_FLAG
		#ifdef __MFW_APPLICATION_ENABLE_LOGGING
			MFW_DECLARE_LOG_CONTEXT(log_application, u8"core/application"_p)
		#endif
	
		static exit_status call_exit(
		#if MFW_BUILD == MFW_BUILD_SHARED && MFW_OS == MFW_OS_WINDOWS
		bool thread
		#endif
		)
		{
			exit_status status{application_exit(
		#if MFW_BUILD == MFW_BUILD_SHARED && MFW_OS == MFW_OS_WINDOWS
			thread
		#endif
			)};

			return status;
		}

		static exit_status call_main(
		#if MFW_BUILD == MFW_BUILD_SHARED && MFW_OS == MFW_OS_WINDOWS
		bool thread
		#endif
		)
		{
			exit_status main_status{};

		#if MFW_BUILD & MFW_BUILD_EXECUTABLE_FLAG
			if(!load_libraries()) {
				main_status.set_failed();
				return main_status;
			}
			
			main_status = __application_internal::core_initialize();
		#endif

			if(main_status.succeded()) {
				main_status += application_main(
			#if MFW_BUILD == MFW_BUILD_SHARED && MFW_OS == MFW_OS_WINDOWS
				thread
			#endif
				);
			}

		#if MFW_BUILD == MFW_BUILD_SHARED
			return main_status;
		#elif MFW_BUILD & MFW_BUILD_EXECUTABLE_FLAG
			exit_status exit{call_exit()};
			main_status += exit;

			#ifdef __MFW_APPLICATION_ENABLE_LOGGING
				#if MFW_BUILD & MFW_BUILD_EXECUTABLE_FLAG
			main_status.add_error(error_count());
			main_status.add_warning(warning_count());
				#endif

			if(main_status.absolutely_succeded()) {
				log_application().set_severity(log_context::severity::success);
			} else if(main_status.succeded()) {
				log_application().set_severity(log_context::severity::warning);
			} else {
				log_application().set_severity(log_context::severity::error);
			}
			log_application().print(u8"exited with code: {} [warnings: {}, errors: {}]"_sv, main_status.code(), main_status.warnings(), main_status.errors());
			#endif

			exit_status core_status{__application_internal::core_shutdown()};
			main_status += core_status;

			return main_status;
		#endif
		}
	#endif
	}
}

#if MFW_BUILD == MFW_BUILD_SHARED && !defined MFW_APPLICATION_MAIN_DEFINED
::mfw::core::exit_status application_main(
	#if MFW_BUILD == MFW_BUILD_SHARED && MFW_OS == MFW_OS_WINDOWS
bool thread
	#endif
)
{
	return {};
}

::mfw::core::exit_status application_exit(
	#if MFW_BUILD == MFW_BUILD_SHARED && MFW_OS == MFW_OS_WINDOWS
bool thread
	#endif
)
{
	return {};
}
#endif

#if MFW_BUILD == MFW_BUILD_SHARED
	#if MFW_OS == MFW_OS_WINDOWS
::mfw::stl::int32_t MFW_CALL_SHARED DllMain(_In_ HINSTANCE hinstDLL, _In_ ::mfw::stl::uint32_t fdwReason, _In_ const void *lpvReserved)
{
	::mfw::core::exit_status status{};

	switch(fdwReason) {
		case DLL_THREAD_ATTACH: {
			status = ::mfw::core::__application_internal::call_main(true);
			break;
		}
		case DLL_PROCESS_ATTACH: {
			status = ::mfw::core::__application_internal::call_main(false);
			break;
		}
		case DLL_THREAD_DETACH: {
			status = ::mfw::core::__application_internal::call_exit(true);
			break;
		}
		case DLL_PROCESS_DETACH: {
			status = ::mfw::core::__application_internal::call_exit(false);
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
void __shared_start() MFW_ATTRIBUTE(__constructor__);
void __shared_exit() MFW_ATTRIBUTE(__destructor__);

void __shared_start()
{
	::mfw::core::__application_internal::call_main();
}

void __shared_exit()
{
	::mfw::core::__application_internal::call_exit();
}
	#endif
#elif MFW_BUILD & MFW_BUILD_EXECUTABLE_FLAG
	#if MFW_OS == MFW_OS_WINDOWS
		#if MFW_CHARACTERSET == MFW_CHARACTERSET_UNICODE
			#if MFW_BUILD == MFW_BUILD_EXECUTABLE
::mfw::stl::int32_t MFW_CALL_SHARED wmain(::mfw::stl::int32_t argc, const wchar_t *argv[])
{
	::mfw::core::exit_status status{::mfw::core::__application_internal::call_main()};
	return status.code();
}
			#elif MFW_BUILD == MFW_BUILD_EXECUTABLE_WINDOWS
::mfw::stl::int32_t MFW_CALL_SHARED wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ wchar_t *lpCmdLine, _In_ ::mfw::stl::int32_t nCmdShow)
{
	::mfw::core::exit_status status{::mfw::core::__application_internal::call_main()};
	return status.code();
}
			#else
				#error
			#endif
		#elif MFW_CHARACTERSET == MFW_CHARACTERSET_MULTIBYTE
			#if MFW_BUILD == MFW_BUILD_EXECUTABLE
::mfw::stl::int32_t MFW_CALL_SHARED main(::mfw::stl::int32_t argc, const char *argv[])
{
	::mfw::core::exit_status status{::mfw::core::__application_internal::call_main()};
	return status.code();
}
			#elif MFW_BUILD == MFW_BUILD_EXECUTABLE_WINDOWS
::mfw::stl::int32_t MFW_CALL_SHARED WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ char *lpCmdLine, _In_ ::mfw::stl::int32_t nCmdShow)
{
	::mfw::core::exit_status status{::mfw::core::__application_internal::call_main()};
	return status.code();
}
			#else
				#error
			#endif
		#else
			#error
		#endif
	#elif MFW_OS == MFW_OS_LINUX
::mfw::stl::int32_t MFW_CALL_SHARED main(::mfw::stl::int32_t, const char *[], const char *[])
{
	::mfw::core::exit_status status{::mfw::core::__application_internal::call_main()};
	return status.code();
}
	#else
		#error
	#endif
#elif !(MFW_BUILD & MFW_BUILD_STATIC_FLAG)
	#error
#endif

#if MFW_BUILD & MFW_BUILD_EXECUTABLE_FLAG
	#if MFW_OS == MFW_OS_WINDOWS && MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
extern "C" {
	const PfnDliHook __pfnDliFailureHook2{::mfw::core::__application_internal::dll_failed_hook};
	const PfnDliHook __pfnDliNotifyHook2{::mfw::core::__application_internal::dll_notify_hook};
}
	#endif
#endif