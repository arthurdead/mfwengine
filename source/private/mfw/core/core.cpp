#include <public/mfw/core/core.hpp>
#include <private/mfw/core/globals_internal.hpp>
#include <public/mfw/core/application.hpp>
//#include <public/mfw/core/library.hpp>
#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/defines.hpp>
#if MFW_OS_IS(WINDOWS)
	#include <Windows.h>
#elif MFW_LIBC_FLAGGED(UNIX)
	#include <wordexp.h>
	#include <errno.h>
	#if MFW_CONFIGURATION_IS(DEBUG)
		#include <malloc.h>
		#include <mcheck.h>
	#endif
	#include <signal.h>
	#if MFW_STDC_IS(DEFAULT)
		#include <cstring>
	#else
		#error
	#endif
#endif
#if MFW_STDCPP_IS(DEFAULT)
	#include <chrono>
#else
	#error
#endif

namespace mfw::core
{
	namespace __private_core_cpp_internal MFW_VISIBILITY_LOCAL
	{
		using namespace ::MFW_STD_NAMESPACE::chrono;

	#if MFW_CONFIGURATION_IS(DEBUG)
		static void debugInit() noexcept
		{
		#if MFW_OS_IS(WINDOWS)
			_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
			_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
			_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_WNDW);

			_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
			_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
			_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);

			_CrtSetDbgFlag(
				//_CRTDBG_CHECK_CRT_DF |
				//_CRTDBG_CHECK_ALWAYS_DF |
				//_CRTDBG_LEAK_CHECK_DF |
				_CRTDBG_ALLOC_MEM_DF// |
				//_CRTDBG_DELAY_FREE_MEM_DF
			);

			_CrtSetBreakAlloc(16);
		#elif MFW_LIBC_FLAGGED(UNIX)
			mcheck(nullptr);
			mcheck_pedantic(nullptr);
			mtrace();
			mallopt(M_CHECK_ACTION, 3);
		#else
			#error
		#endif
		}
	#endif

		static bool should_terminate{false};
		static os_layer_t os_layer{os_layer_t::native};
	}

	MFW_CORE_API void MFW_CORE_CALL terminate() noexcept
	{
		__private_core_cpp_internal::should_terminate = true;
	}

	MFW_CORE_API ExitStatus MFW_CORE_CALL initialize() noexcept
	{
	#if MFW_CONFIGURATION_IS(DEBUG)
		__private_core_cpp_internal::debugInit();
	#endif
	
	#if MFW_LIBC_FLAGGED(UNIX)
		//signal(SIGCHLD, SIG_IGN);
	#endif

		allocateAllGlobals();

		sortInitializers();

		ExitStatus status{initializeAllGlobals()};
		if(__private_core_cpp_internal::should_terminate) {
			status = ExitStatus::fatal;
		}
		return status;
	}

	MFW_CORE_API ExitStatus MFW_CORE_CALL update() noexcept
	{
		ExitStatus status{updateAllGlobals()};
		if(__private_core_cpp_internal::should_terminate) {
			status = ExitStatus::fatal;
		}
		return status;
	}

	MFW_CORE_API ExitStatus MFW_CORE_CALL shutdown() noexcept
	{
		ExitStatus status{shutdownAllGlobals()};
		deallocateAllGlobals();
		//Library::unload_all_libraries();
		//Library::remove_all_directories();
		if(__private_core_cpp_internal::should_terminate) {
			status = ExitStatus::fatal;
		}
		return status;
	}

#if MFW_LIBC_FLAGGED(UNIX)
	MFW_CORE_API void MFW_CORE_CALL expand_shell(stl::osstring_view src, stl::vector<stl::osstring> &dst) noexcept
	{
		wordexp_t exp{};
		wordexp(src.data(), &exp, 0);
		for(size_t i{0}; i < exp.we_wordc; ++i) {
			stl::osstring &tmp{dst.emplace_back()};
			tmp.assign(exp.we_wordv[i]);
		}
		wordfree(&exp);
	}
#endif

	MFW_CORE_API void MFW_CORE_CALL expandEnvironmentVariables(stl::osstring_view src, stl::osstring &dst) noexcept
	{
	#if MFW_OS == MFW_OS_WINDOWS
		const wchar_t *src_data{c_str(src)};
		stl::uint32_t count{ExpandEnvironmentStringsW(src_data, nullptr, 0)};
		dst.resize(count);
		ExpandEnvironmentStringsW(src_data, c_str(dst), static_cast<stl::uint32_t>(dst.size()));
	#else
		wordexp_t exp{};
		wordexp(src.data(), &exp, 0);
		if(exp.we_wordc) {
			dst.assign(exp.we_wordv[0]);
		} else {
			dst = src;
		}
		wordfree(&exp);
	#endif
	}

	MFW_CORE_API stl::int32_t MFW_CORE_CALL get_last_error() noexcept
	{
	#if MFW_OS_IS(WINDOWS)
		return GetLastError();
	#elif MFW_LIBC_FLAGGED(UNIX)
		return errno;
	#else
		#error
	#endif
	}

	MFW_CORE_API void MFW_CORE_CALL get_error_string(stl::int32_t code, stl::osstring &str) noexcept
	{
	#if MFW_OS_IS(WINDOWS)
		str.resize(128);
		stl::uint32_t len{FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), c_str(str), static_cast<stl::uint32_t>(str.size()), nullptr)};
		str.resize(static_cast<stl::size_t>(len));

		stl::osstring_view find{L"\r\n"_sv};
		stl::size_t pos{str.find(find.data(), 0, find.length())};
		if(pos != stl::osstring::npos) {
			str.erase(pos, 2);
		}
	#elif MFW_LIBC_FLAGGED(UNIX)
		str.resize(128);
		strerror_r(code, str.data(), str.size());
	#else
		#error
	#endif
	}

	MFW_CORE_API stl::int64_t MFW_CORE_CALL time_now() noexcept
	{
		using clock_t = __private_core_cpp_internal::high_resolution_clock;
		using duration_t = __private_core_cpp_internal::microseconds;
		
		using time_micro_t = __private_core_cpp_internal::time_point<clock_t, duration_t>;
		time_micro_t time_point{__private_core_cpp_internal::time_point_cast<time_micro_t::duration>(clock_t::now())};
		
		return time_point.time_since_epoch().count();
	}

	MFW_CORE_API os_layer_t MFW_CORE_CALL getOSLayer() noexcept
	{
		return __private_core_cpp_internal::os_layer;
	}
}