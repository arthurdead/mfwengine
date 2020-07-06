#include <public/mfw/core/application.hpp>
#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/core/core.hpp>
#if MFW_BUILD_FLAGGED(EXECUTABLE)
	//#include <public/mfw/core/filesystem_interface.hpp>
	//#include <public/mfw/core/library.hpp>
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
	const ExitStatus ExitStatus::success{ExitStatus::exit_codes_t::success};
	const ExitStatus ExitStatus::fatal{ExitStatus::exit_codes_t::fatal};
	
	namespace __public_application_cpp_internal MFW_VISIBILITY_LOCAL
	{
	#if MFW_BUILD_FLAGGED(SHARED)
		#if defined _MFW_LOGGING_ENABLED
		MFW_DECLARE_LOG_CONTEXT(log_application, u8"core/application"_p)
		#endif
	
		template <typename... Args>
		static void print(stl::osstring_view fmt, Args... args) noexcept
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
	namespace __public_application_cpp_internal MFW_VISIBILITY_LOCAL
	{
	#ifndef _MFW_APPLICATION_CORE_AVAILABLE
		using core_load_library_t = decltype(core::Library::load_library) *;
		static core_load_library_t core_load_library_ptr{nullptr};
		
		using core_update_t = decltype(update) *;
		static core_update_t core_update_ptr{nullptr};
		
		using core_initialize_t = decltype(initialize) *;
		static core_initialize_t core_initialize_ptr{nullptr};
		
		using core_shutdown_t = decltype(shutdown) *;
		static core_shutdown_t core_shutdown_ptr{nullptr};
		
		using core_filesystem_instance_t = decltype(FileSystem::instance) *;
		static core_filesystem_instance_t core_filesystem_instance_ptr{nullptr};
	#endif
	
		static FileSystem &coreFileSystemInstance() noexcept
		{
		#ifdef _MFW_APPLICATION_CORE_AVAILABLE
			return FileSystem::instance();
		#else
			return core_filesystem_instance_ptr();
		#endif
		}
	
		static ExitStatus coreInitialize() noexcept
		{
		#ifdef _MFW_APPLICATION_CORE_AVAILABLE
			return initialize();
		#else
			return core_initialize_ptr();
		#endif
		}
		
		static ExitStatus coreShutdown() noexcept
		{
		#ifdef _MFW_APPLICATION_CORE_AVAILABLE
			return shutdown();
		#else
			return core_shutdown_ptr();
		#endif
		}
	}
	
	MFW_VISIBILITY_LOCAL pstring executablePath() noexcept
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
	
	MFW_VISIBILITY_LOCAL bool coreLoadLibrary(const SearchPath &name) noexcept
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
	
	MFW_VISIBILITY_LOCAL ExitStatus coreUpdate() noexcept
	{
	#ifdef _MFW_APPLICATION_CORE_AVAILABLE
		return ::mfw::core::update();
	#else
		return __public_application_cpp_internal::core_update_ptr();
	#endif
	}
#endif
	
	namespace __public_application_cpp_internal MFW_VISIBILITY_LOCAL
	{
	#if MFW_BUILD_FLAGGED(EXECUTABLE)
		#if MFW_CORE_BUILD == MFW_BUILD_SHARED && defined __MFW_CORE_IS_DELAY_LOADED
			#if MFW_OS_IS(LINUX)
		static void *coreLibDLopen() noexcept
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
	
		static bool loadCoreLib() noexcept
		{
			#if MFW_OS_IS(WINDOWS)
			if(!LoadLibraryExW(MFW_T("core/bin/" __MFW_TARGET_TRIPLE L"/core.dll"), nullptr, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS)) {
				__public_application_cpp_internal::print(MFW_T("could not load core library"_sv));
				return false;
			}
			#elif MFW_OS == MFW_OS_LINUX
			void *core_dl{coreLibDLopen()};
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

		static bool loadLibraries() noexcept
		{
		#if MFW_CORE_BUILD == MFW_BUILD_SHARED && defined __MFW_CORE_IS_DELAY_LOADED
			if(!loadCoreLib()) {
				return false;
			}
		#endif
			
			if(!applicationLoadLibraries()) {
				return false;
			}
			
			return true;
		}
	#endif

	#if MFW_BUILD_FLAGGED(SHARED)
		static ExitStatus callExit(
		#if MFW_BUILD_IS(SHARED) && MFW_OS_IS(WINDOWS)
		bool thread
		#endif
		) noexcept
		{
			ExitStatus status{applicationExit(
		#if MFW_BUILD_IS(SHARED) && MFW_OS_IS(WINDOWS)
			thread
		#endif
			)};

			return status;
		}

		static ExitStatus callMain(
		#if MFW_BUILD_IS(SHARED) && MFW_OS_IS(WINDOWS)
		bool thread
		#endif
		) noexcept
		{
			ExitStatus main_status{};

		#if MFW_BUILD_FLAGGED(EXECUTABLE)
			if(!loadLibraries()) {
				main_status.setFailed();
				return main_status;
			}
			
			main_status = coreInitialize();
		#endif

			if(main_status.succeded()) {
				main_status += applicationMain(
			#if MFW_BUILD_IS(SHARED) && MFW_OS_IS(WINDOWS)
				thread
			#endif
				);
			}

		#if MFW_BUILD_IS(SHARED)
			return main_status;
		#elif MFW_BUILD_FLAGGED(EXECUTABLE)
			ExitStatus exit{callExit()};
			main_status += exit;

			#ifdef _MFW_APPLICATION_CORE_AVAILABLE
			main_status.errors() += errorCount();
			main_status.warnings() += warningCount();
			#endif

			#ifdef _MFW_LOGGING_ENABLED
			if(main_status.absolutelySucceded()) {
				log_application().setSeverity(LogContext::severity_t::success);
			} else if(main_status.succeded()) {
				log_application().setSeverity(LogContext::severity_t::warning);
			} else {
				log_application().setSeverity(LogContext::severity_t::error);
			}
			#endif
			__public_application_cpp_internal::print(MFW_T("exited with code: {} [warnings: {}, errors: {}]"_sv), main_status.code(), main_status.warnings(), main_status.errors());

			ExitStatus core_status{coreShutdown()};
			main_status += core_status;

			return main_status;
		#endif
		}
	#endif
	}
}

#if MFW_BUILD_IS(SHARED) && !defined MFW_APPLICATION_MAIN_DEFINED
MFW_VISIBILITY_LOCAL ::mfw::core::ExitStatus applicationMain(
	#if MFW_BUILD_IS(SHARED) && MFW_OS_IS(WINDOWS)
bool thread
	#endif
) noexcept
{
	return ::mfw::core::ExitStatus::success;
}

MFW_VISIBILITY_LOCAL ::mfw::core::ExitStatus applicationExit(
	#if MFW_BUILD_IS(SHARED) && MFW_OS_IS(WINDOWS)
bool thread
	#endif
) noexcept
{
	return ::mfw::core::ExitStatus::success;
}
#endif

#if MFW_BUILD_IS(SHARED)
	#if MFW_OS_IS(WINDOWS)
MFW_VISIBILITY_LOCAL ::mfw::stl::int32_t MFW_CALL_SHARED DllMain(_In_ HINSTANCE hinstDLL, _In_ ::mfw::stl::uint32_t fdwReason, _In_ const void *lpvReserved) MFW_NOTHROW
{
	::mfw::core::ExitStatus status{};

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
MFW_ATTRIBUTE(__constructor__) MFW_VISIBILITY_LOCAL void MFW_CALL_SHARED __shared_start() noexcept
{
	::mfw::core::__public_application_cpp_internal::call_main();
}

MFW_ATTRIBUTE(__destructor__) MFW_VISIBILITY_LOCAL void MFW_CALL_SHARED __shared_exit() noexcept
{
	::mfw::core::__public_application_cpp_internal::call_exit();
}
	#endif
#elif MFW_BUILD_FLAGGED(EXECUTABLE)
	#if MFW_OS_IS(WINDOWS)
		#if MFW_CHARACTERSET_IS(UNICODE)
			#if MFW_BUILD_IS(EXECUTABLE)
MFW_VISIBILITY_LOCAL ::mfw::stl::int32_t MFW_CALL_SHARED wmain(::mfw::stl::int32_t argc, const wchar_t *argv[]) MFW_NOTHROW
{
	::mfw::core::ExitStatus status{::mfw::core::__public_application_cpp_internal::call_main()};
	return status.code();
}
			#elif MFW_BUILD_IS(EXECUTABLE_WINDOWS)
MFW_VISIBILITY_LOCAL ::mfw::stl::int32_t MFW_CALL_SHARED wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ wchar_t *lpCmdLine, _In_ ::mfw::stl::int32_t nCmdShow) MFW_NOTHROW
{
	::mfw::core::ExitStatus status{::mfw::core::__public_application_cpp_internal::call_main()};
	return status.code();
}
			#else
				#error
			#endif
		#elif MFW_CHARACTERSET_IS(MULTIBYTE)
			#if MFW_BUILD_IS(EXECUTABLE)
MFW_VISIBILITY_LOCAL ::mfw::stl::int32_t MFW_CALL_SHARED main(::mfw::stl::int32_t argc, const char *argv[]) MFW_NOTHROW
{
	::mfw::core::ExitStatus status{::mfw::core::__public_application_cpp_internal::call_main()};
	return status.code();
}
			#elif MFW_BUILD_IS(EXECUTABLE_WINDOWS)
MFW_VISIBILITY_LOCAL ::mfw::stl::int32_t MFW_CALL_SHARED WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ char *lpCmdLine, _In_ ::mfw::stl::int32_t nCmdShow) MFW_NOTHROW
{
	::mfw::core::ExitStatus status{::mfw::core::__public_application_cpp_internal::callMain()};
	return status.code();
}
			#else
				#error
			#endif
		#else
			#error
		#endif
	#elif MFW_OS_IS(LINUX)
MFW_VISIBILITY_LOCAL ::mfw::stl::int32_t MFW_CALL_SHARED main(::mfw::stl::int32_t, const char *[], const char *[]) MFW_NOTHROW
{
	::mfw::core::ExitStatus status{::mfw::core::__public_application_cpp_internal::callMain()};
	return status.code();
}
	#else
		#error
	#endif
#elif !(MFW_BUILD_FLAGGED(STATIC))
	#error
#endif

#if MFW_BUILD_FLAGGED(EXECUTABLE)
extern "C" {
	MFW_MESSAGE("move this somewhere else")
	MFW_SHARED_EXPORT ::mfw::stl::uint32_t NvOptimusEnablement{0x00000001};
	MFW_SHARED_EXPORT ::mfw::stl::int32_t AmdPowerXpressRequestHighPerformance{1};
}
	#if MFW_OS_IS(LINUX) && defined __MFW_CORE_IS_DELAY_LOADED && defined __MFW_CORE_USING_IMPLIB
extern "C" MFW_VISIBILITY_LOCAL void *__load_core_lib(const char *) MFW_NOTHROW
{ return ::mfw::core::__public_application_cpp_internal::coreLibDLopen(); }
	#endif
	#if MFW_OS_IS(WINDOWS) && MFW_CONFIGURATION_IS(DEBUG)
namespace mfw::core::__public_application_cpp_internal MFW_VISIBILITY_LOCAL
{
	static FARPROC MFW_CALL_SHARED dllFailedHook(uint32_t dliNotify, PDelayLoadInfo pdli) noexcept
	{
		MFW_DEBUGBREAK();
		return nullptr;
	}
	
	static FARPROC MFW_CALL_SHARED dllNotifyHook(uint32_t dliNotify, PDelayLoadInfo pdli) noexcept
	{
		if(pdli->dwLastError == ERROR_SUCCESS) {
			return nullptr;
		}

		MFW_DEBUGBREAK();
		return nullptr;
	}
}
extern "C" {
	const PfnDliHook __pfnDliFailureHook2{::mfw::core::__public_application_cpp_internal::dllFailedHook};
	const PfnDliHook __pfnDliNotifyHook2{::mfw::core::__public_application_cpp_internal::dllNotifyHook};
}
	#endif
#endif