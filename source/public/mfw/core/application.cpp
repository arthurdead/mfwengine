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

#if MFW_CORE_BUILD & MFW_BUILD_STATIC_FLAG || defined __MFW_CORE_USING_IMPLIB || !defined __MFW_CORE_IS_DELAY_LOADED
	#define __MFW_APPLICATION_CORE_AVAILABLE
#endif

#if defined __MFW_CORE_IS_DELAY_LOADED && defined __MFW_CORE_USING_IMPLIB
extern "C" __attribute__((__visibility__("internal"))) void __core_tramp_resolve_all();
#endif

#ifdef __MFW_APPLICATION_CORE_AVAILABLE
	#if MFW_OS_IS(LINUX) && !defined __MFW_CORE_USING_IMPLIB
		#define __MFW_LOGGING_ENABLED
	#elif MFW_OS_IS(WINDOWS)
		#define __MFW_LOGGING_ENABLED
	#endif
#endif

#if defined __MFW_LOGGING_ENABLED
	#include <public/mfw/core/logging_interface.hpp>
#endif

#if MFW_COMPILER_FLAGGED(CLANG)
	MFW_WARNING_DISABLE_UNIX("-Wmissing-prototypes")
	MFW_WARNING_DISABLE_UNIX("-Wmissing-variable-declarations")
#endif

namespace mfw::core
{
	MFW_SHARED_LOCAL const exit_status exit_status::success{exit_status::exit_codes::success};
	MFW_SHARED_LOCAL const exit_status exit_status::fatal{exit_status::exit_codes::fatal};
	
	namespace __application_internal
	{
	#if MFW_BUILD & MFW_BUILD_SHARED_FLAG
		#if defined __MFW_LOGGING_ENABLED
		MFW_DECLARE_LOG_CONTEXT(log_application, u8"core/application"_p)
		#endif
	
		template <typename ...Args>
		static void print(ucstring_view fmt, Args... args)
		{
		#if defined __MFW_LOGGING_ENABLED
			log_application().print(fmt, forward<Args>(args)...);
		#else
			ucstring str{};
			format(str, fmt, forward<Args>(args)...);
			str += u8'\n';
			::MFW_STD_NAMESPACE::printf(c_str(str));
		#endif
		}
	#endif
	}
	
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
	
		pstring exepath{};
		exepath.assign(exefile, exefile+len);
		return exepath;
	}
	
	bool core_load_library(const searchpath &name)
	{
		::mfw::core::library *lib{
	#ifdef __MFW_APPLICATION_CORE_AVAILABLE
			::mfw::core::library::load_library(name)
	#else
			__application_internal::core_load_library_ptr(name)
	#endif
		};
		return lib != nullptr;
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
		#if MFW_CORE_BUILD == MFW_BUILD_SHARED && defined __MFW_CORE_IS_DELAY_LOADED
			#if MFW_OS_IS(LINUX)
		static void *core_lib_dlopen()
		{
			void *core_dl{dlmopen(LM_ID_BASE, "core/bin/" __MFW_TARGET_TRIPLE "/core.so", RTLD_LAZY|RTLD_GLOBAL)};
			if(!core_dl) {
				ucstring reason{uc_str(dlerror())};
				__application_internal::print(u8"could not load core library: {}"_sv, reason);
			}
			return core_dl;
		}
			#endif
	
		static bool load_core_lib()
		{
			#if MFW_OS == MFW_OS_WINDOWS
			if(!LoadLibraryExW(L"core/bin/" __MFW_TARGET_TRIPLE L"/core.dll", nullptr, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS)) {
				__application_internal::print(u8"could not load core library"_sv);
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
					__application_internal::print(u8"core library missing library::load_library symbol"_sv);
					return false;
				}
				
				constexpr const char *core_update_name{"_ZN3mfw4core6updateEv"};
				core_update_ptr = reinterpret_cast<core_update_t>(dlsym(core_dl, core_update_name));
				if(!core_update_ptr) {
					__application_internal::print(u8"core library missing update symbol"_sv);
					return false;
				}
				
				constexpr const char *core_initialize_name{"_ZN3mfw4core10initializeEv"};
				core_initialize_ptr = reinterpret_cast<core_initialize_t>(dlsym(core_dl, core_initialize_name));
				if(!core_initialize_ptr) {
					__application_internal::print(u8"core library missing initialize symbol"_sv);
					return false;
				}
				
				constexpr const char *core_shutdown_name{"_ZN3mfw4core8shutdownEv"};
				core_shutdown_ptr = reinterpret_cast<core_shutdown_t>(dlsym(core_dl, core_shutdown_name));
				if(!core_shutdown_ptr) {
					__application_internal::print(u8"core library missing shutdown symbol"_sv);
					return false;
				}
				
				constexpr const char *core_filesystem_instance_name{"_ZN3mfw4core10interfaces10filesystem8instanceEv"};
				core_filesystem_instance_ptr = reinterpret_cast<core_filesystem_instance_t>(dlsym(core_dl, core_filesystem_instance_name));
				if(!core_filesystem_instance_ptr) {
					__application_internal::print(u8"core library missing interfaces::filesystem::instance symbol"_sv);
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

		static bool load_libraries()
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

	#if MFW_BUILD & MFW_BUILD_SHARED_FLAG
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
			
			main_status = core_initialize();
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

			#ifdef __MFW_APPLICATION_CORE_AVAILABLE
			main_status.add_error(error_count());
			main_status.add_warning(warning_count());
			#endif

			#ifdef __MFW_LOGGING_ENABLED
			if(main_status.absolutely_succeded()) {
				log_application().set_severity(log_context::severity::success);
			} else if(main_status.succeded()) {
				log_application().set_severity(log_context::severity::warning);
			} else {
				log_application().set_severity(log_context::severity::error);
			}
			#endif
			__application_internal::print(u8"exited with code: {} [warnings: {}, errors: {}]"_sv, main_status.code(), main_status.warnings(), main_status.errors());

			exit_status core_status{core_shutdown()};
			main_status += core_status;

			return main_status;
		#endif
		}
	#endif
	}
}

#if MFW_BUILD == MFW_BUILD_SHARED && !defined MFW_APPLICATION_MAIN_DEFINED
MFW_SHARED_LOCAL ::mfw::core::exit_status application_main(
	#if MFW_BUILD == MFW_BUILD_SHARED && MFW_OS == MFW_OS_WINDOWS
bool thread
	#endif
)
{
	return {};
}

MFW_SHARED_LOCAL ::mfw::core::exit_status application_exit(
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
MFW_ATTRIBUTE(__constructor__) void __shared_start()
{
	::mfw::core::__application_internal::call_main();
}

MFW_ATTRIBUTE(__destructor__) void __shared_exit()
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
extern "C" {
	MFW_SHARED_EXPORT ::mfw::uint32_t NvOptimusEnablement{0x00000001};
	MFW_SHARED_EXPORT ::mfw::int32_t AmdPowerXpressRequestHighPerformance{1};
}
	#if MFW_OS_IS(LINUX) && defined __MFW_CORE_IS_DELAY_LOADED && defined __MFW_CORE_USING_IMPLIB
extern "C" void *__load_core_lib(const char *)
{
	return ::mfw::core::__application_internal::core_lib_dlopen();
}
	#endif
	#if MFW_OS == MFW_OS_WINDOWS && MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
namespace mfw::core::__application_internal
{
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
}
extern "C" {
	const PfnDliHook __pfnDliFailureHook2{::mfw::core::__application_internal::dll_failed_hook};
	const PfnDliHook __pfnDliNotifyHook2{::mfw::core::__application_internal::dll_notify_hook};
}
	#endif
#endif