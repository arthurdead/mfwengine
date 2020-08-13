#ifndef MFW_PUBLIC_CORE_PROCESS_HPP
#define MFW_PUBLIC_CORE_PROCESS_HPP

#pragma once

#include <public/mfw/core/core.hpp>
#include <public/mfw/core/searchpath.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/core/file_interface.hpp>

namespace mfw::core
{
	class MFW_VISIBILITY_LOCAL process final
	{
	public:
	#if MFW_OS_IS(WINDOWS)
		using handle_t = HANDLE;
		static constexpr handle_t invalid_handle{INVALID_HANDLE};
	#elif MFW_OS_IS(LINUX)
		using handle_t = stl::int32_t;
		static constexpr handle_t invalid_handle{-1};
	#else
		#error
	#endif

		using stdout_handle_t = file *;
		static constexpr stdout_handle_t invalid_stdout_handle{nullptr};

		using string_type = stl::osstring;
		using string_view_type = stl::osstring_view;
		using searchpath_view_type = searchpath_view;

		process() noexcept = default;
		process &operator=(process &&) noexcept = default;
		process(process &&) noexcept = default;
		MFW_CORE_API process & MFW_CORE_CALL operator=(const process &other) noexcept;
		process(const process &other) noexcept;

		MFW_CORE_API MFW_CORE_CALL ~process() noexcept;
		
		//static MFW_CORE_API bool MFW_CORE_CALL find(const searchpath &search, process &proc) noexcept;
		static MFW_CORE_API bool MFW_CORE_CALL find(string_view_type name, process &proc) noexcept;
		static MFW_CORE_API bool MFW_CORE_CALL find(string_view_type name, stl::vector<process> &procs) noexcept;
		static MFW_CORE_API stl::pstring MFW_CORE_CALL get_path(string_view_type cmd) noexcept;

		MFW_CORE_API process & MFW_CORE_CALL set_workingdir(searchpath_view_type search) noexcept;
		const stl::pstring &workingdir() const noexcept;
		string_type &args() noexcept;
		const string_type &args() const noexcept;
		MFW_CORE_API process & MFW_CORE_CALL set_shell() noexcept;
		MFW_CORE_API process & MFW_CORE_CALL set_shell(string_view_type cmd) noexcept;
		MFW_CORE_API process & MFW_CORE_CALL set_shell(string_type &&cmd) noexcept;
		MFW_CORE_API process & MFW_CORE_CALL set_path(searchpath_view_type search) noexcept;
		const stl::pstring &path() const noexcept;
		bool shell_command() const noexcept;
		const string_type &cmd() const noexcept;
		string_type &cmd() noexcept;
		MFW_CORE_API ucstring MFW_CORE_CALL args_absolute() const noexcept;

		MFW_CORE_API bool MFW_CORE_CALL start(bool wait=false) noexcept;
		MFW_CORE_API bool MFW_CORE_CALL running() noexcept;
		MFW_CORE_API process & MFW_CORE_CALL wait() noexcept;
		MFW_CORE_API process & MFW_CORE_CALL close(bool kill=false) noexcept;
		process &kill() noexcept;

		MFW_CORE_API int32_t MFW_CORE_CALL exit_code() noexcept;
		MFW_CORE_API stl::int32_t MFW_CORE_CALL exit_code() const noexcept;

		MFW_CORE_API const string_type & MFW_CORE_CALL output(stl::size_t max = 0) noexcept;
		const string_type &output() const noexcept;

	private:
		void capture_vars(stl::size_t max = 0) noexcept;

		stl::pstring m_workingdir{};
		string_type m_args{};
		string_type m_cmd{};
		stl::pstring m_path{};
		string_type m_output{};
	#if MFW_OS_IS(LINUX)
		stl::int32_t m_status{0};
	#endif
		bool m_shell_command{false};
		bool m_can_kill{true};
		handle_t m_handle{invalid_handle};
		stdout_handle_t m_stdout_handle{invalid_stdout_handle};
	};
}

#endif