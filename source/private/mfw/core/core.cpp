#include <public/mfw/core/core.hpp>
#include <private/mfw/core/globals_internal.hpp>
#include <public/mfw/core/filesystem_interface.hpp>
#include <public/mfw/core/application.hpp>
#include <public/mfw/core/library.hpp>
#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/defines.hpp>
#if MFW_OS == MFW_OS_WINDOWS
	#include <Windows.h>
#elif MFW_OS == MFW_OS_LINUX
	#include <wordexp.h>
	#include <errno.h>
	#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
		#include <malloc.h>
		#include <mcheck.h>
	#endif
	#include <signal.h>
	#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
		#include <cstring>
	#else
		#error
	#endif
#endif
#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <chrono>
#else
	#error
#endif

namespace mfw::core
{
	namespace __core_internal
	{
		using namespace ::MFW_STD_NAMESPACE::chrono;

	#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
		static void debug_init()
		{
		#if MFW_OS == MFW_OS_WINDOWS
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
		#elif MFW_OS == MFW_OS_LINUX
			mcheck(nullptr);
			mcheck_pedantic(nullptr);
			mtrace();
			mallopt(M_CHECK_ACTION, 3);
		#else
			#error
		#endif
		}
	#endif
	}

	MFW_CORE_API exit_status MFW_CORE_CALL initialize()
	{
	#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
		__core_internal::debug_init();
	#endif
	
	#if MFW_OS_IS(LINUX)
		//signal(SIGCHLD, SIG_IGN);
	#endif

		allocate_all_globals();
		exit_status status{initialize_all_globals()};
		return status;
	}

	MFW_CORE_API exit_status MFW_CORE_CALL update()
	{
		exit_status status{update_all_globals()};
		return status;
	}

	MFW_CORE_API exit_status MFW_CORE_CALL shutdown()
	{
		exit_status status{shutdown_all_globals()};
		deallocate_all_globals();
		library::unload_all_libraries();
		library::remove_all_directories();
		return status;
	}

#if MFW_OS == MFW_OS_LINUX
	MFW_CORE_API void MFW_CORE_CALL expand_shell(const ucstring_view &src, vector<ucstring> &dst)
	{
		wordexp_t exp{};
		wordexp(c_str(src), &exp, 0);
		for(size_t i{0}; i < exp.we_wordc; i++) {
			ucstring tmp{reinterpret_cast<const ucchar_t *>(exp.we_wordv[i])};
			dst.emplace_back(tmp);
		}
		wordfree(&exp);
	}
#endif

	MFW_CORE_API void MFW_CORE_CALL expand_env_vars(const ucstring_view &src, ucstring &dst)
	{
	#if MFW_OS == MFW_OS_WINDOWS
		const wchar_t *src_data{c_str(src)};
		uint32_t count{ExpandEnvironmentStringsW(src_data, nullptr, 0)};
		dst.resize(count);
		ExpandEnvironmentStringsW(src_data, c_str(dst), static_cast<uint32_t>(dst.size()));
	#else
		vector<ucstring> words{};
		expand_shell(src, words);
		if(!words.empty()) {
			dst = words[0];
		} else {
			dst = src;
		}
	#endif
	}

	MFW_CORE_API int32_t MFW_CORE_CALL get_last_error()
	{
	#if MFW_OS == MFW_OS_WINDOWS
		return GetLastError();
	#elif MFW_OS == MFW_OS_LINUX
		return errno;
	#else
		#error
	#endif
	}

	MFW_CORE_API void MFW_CORE_CALL get_error_string(int32_t code, ucstring &str)
	{
		str.resize(128, u8'\0');

	#if MFW_OS == MFW_OS_WINDOWS
		uint32_t len{FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), c_str(str), static_cast<uint32_t>(str.size()), nullptr)};
		str.resize(static_cast<size_t>(len));

		ucstring_view find{u8"\r\n"_sv};
		size_t pos{str.find(find.data(), 0, find.length())};
		if(pos != ucstring::npos) {
			str.erase(pos, 2);
		}
	#elif MFW_OS == MFW_OS_LINUX
		strerror_r(code, c_str(str), str.size());
	#else
		#error
	#endif
	}

	MFW_CORE_API int64_t MFW_CORE_CALL time_now()
	{
		using time_nano_t = __core_internal::time_point<__core_internal::system_clock, __core_internal::nanoseconds>;
		//using time_mili_t = __core_internal::time_point<__core_internal::system_clock, __core_internal::milliseconds>;

		time_nano_t time_nano{__core_internal::high_resolution_clock::now()};

	#if 0
		time_mili_t time_mili{__core_internal::time_point_cast<__core_internal::milliseconds>(time_point)};
		__core_internal::milliseconds since_epoch{time_mili.time_since_epoch()};
	#else
		__core_internal::nanoseconds since_epoch{time_nano.time_since_epoch()};
	#endif

		return since_epoch.count();
	}
}