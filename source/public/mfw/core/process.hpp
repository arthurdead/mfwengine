#ifndef __MFW_PUBLIC_CORE_PROCESS_H
#define __MFW_PUBLIC_CORE_PROCESS_H

#pragma once

#include <public/mfw/core/core.hpp>
#include <public/mfw/core/searchpath.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/core/file_interface.hpp>

namespace mfw::core
{
	class process
	{
	public:
	#if MFW_OS == MFW_OS_WINDOWS
		using handle_t = HANDLE;
		static inline constexpr handle_t invalid_handle{INVALID_HANDLE};
	#elif MFW_OS == MFW_OS_LINUX
		using handle_t = int32_t;
		static inline constexpr handle_t invalid_handle{-1};
	#else
		#error
	#endif

		using stdout_handle_t = interfaces::file *;
		static inline constexpr stdout_handle_t invalid_stdout_handle{nullptr};

		process() = default;

		MFW_CORE_API process & MFW_CORE_CALL operator=(const process &other);
		process(const process &other) { operator=(other); }

		MFW_CORE_API MFW_CORE_CALL ~process();
		
		//static MFW_CORE_API bool MFW_CORE_CALL find(const searchpath &search, process &proc);
		static MFW_CORE_API bool MFW_CORE_CALL find(ucstring_view name, process &proc);
		static MFW_CORE_API bool MFW_CORE_CALL find(ucstring_view name, vector<process> &procs);
		static MFW_CORE_API pstring MFW_CORE_CALL get_path(const ucstring &cmd);

		MFW_CORE_API void MFW_CORE_CALL set_workingdir(const searchpath &search);
		const pstring &workingdir() const { return workingdir_; }
		MFW_CORE_API void MFW_CORE_CALL set_args(const ucstring &args);
		void append_args(const ucstring &args) { args_ += args; }
		const ucstring &args() const { return args_; }
		MFW_CORE_API void MFW_CORE_CALL set_shell();
		MFW_CORE_API void MFW_CORE_CALL set_shell(const ucstring &cmd);
		MFW_CORE_API void MFW_CORE_CALL set_path(const searchpath &search);
		const pstring &path() const { return path_; }
		bool shell_command() const { return shell_command_; }
		void set_cmd(const ucstring &cmd) { cmd_ = cmd; }
		const ucstring &cmd() const { return cmd_; }
		MFW_CORE_API ucstring MFW_CORE_CALL args_absolute() const;

		MFW_CORE_API bool MFW_CORE_CALL start(bool wait=false);
		MFW_CORE_API bool MFW_CORE_CALL running();
		MFW_CORE_API void MFW_CORE_CALL wait();
		MFW_CORE_API void MFW_CORE_CALL close(bool kill=false);
		void kill() { close(true); }

		MFW_CORE_API int32_t MFW_CORE_CALL exit_code();
		int32_t exit_code() const
		{ return const_cast<process *>(this)->exit_code(); }

		MFW_CORE_API const ucstring & MFW_CORE_CALL output(size_t max = 0);
		const ucstring &output(size_t max = 0) const
		{ return const_cast<process *>(this)->output(max); }

	private:
		void capture_vars(size_t max = 0);

		pstring workingdir_{};
		ucstring args_{};
		ucstring cmd_{};
		pstring path_{};
		ucstring output_{};
	#if MFW_OS == MFW_OS_LINUX
		int32_t status_{0};
	#endif
		bool shell_command_{false};
		bool can_kill_{true};
		handle_t handle{invalid_handle};
		stdout_handle_t stdout_handle{invalid_stdout_handle};
	};
}

#endif