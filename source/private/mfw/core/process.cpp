#include <public/mfw/core/process.hpp>
#include <public/mfw/core/filesystem_interface.hpp>
#include <private/mfw/core/file.hpp>
#include <public/mfw/core/environment.hpp>
#if MFW_OS_IS(WINDOWS)
	#include <public/mfw/stl/windows/windows.h>
#elif MFW_OS_IS(LINUX)
	#include <public/mfw/stl/vector.hpp>
	#include <unistd.h>
	#include <signal.h>
	#include <sys/wait.h>
	#include <dirent.h>
	#if MFW_STDC_IS(DEFAULT)
		#include <cstring>
	#else
		#error
	#endif
#endif

#include <public/mfw/core/logging_interface.hpp>

namespace mfw::core
{
	MFW_VISIBILITY_LOCAL_PUSH()

	MFW_DECLARE_LOG_CONTEXT(log_process, MFW_T("core/process"_p))

	namespace __private_process_cpp_internal
	{
		MFW_VISIBILITY_LOCAL_PUSH()

		static stl::pstring get_shell_path() noexcept
		{
		#if MFW_OS_IS(LINUX)
			environment_var var{MFW_T("SHELL"_sv)};
			if(var.is_set()) {
				return stl::pstring{var.value()};
			} else {
				return MFW_T("/bin/sh"_p);
			}
		#else
			return MFW_T("C:\\Windows\\cmd.exe"_p);
		#endif
		}
		
		static void begin_shell_arg(stl::osstring &str) noexcept
		{
		#if MFW_OS_IS(LINUX)
			str += MFW_T("-c \""_sv);
		#elif MFW_OS_IS(WINDOWS)
			str += MFW_T("/c \""_sv);
		#else
			#error
		#endif
		}
		
		static void end_shell_arg(stl::osstring &str) noexcept
		{
		#if MFW_OS_IS(LINUX) || \
			MFW_OS_IS(WINDOWS)
			str += MFW_T('"');
		#else
			#error
		#endif
		}
		
		static void args_absolute(stl::osstring &tmp, const process &proc) noexcept
		{
			bool shell_command{proc.shell_command()};
			const stl::osstring &cmd{proc.cmd()};
			const stl::osstring &args{proc.args()};
			
			if(shell_command) {
				begin_shell_arg(tmp);
				if(!cmd.empty()) {
					tmp.append(cmd);
					if(!args.empty()) {
						tmp.append(1, MFW_T(' '));
					}
				}
			}
			tmp.append(args);
			if(shell_command) {
				end_shell_arg(tmp);
			}
		}
		
	#if MFW_OS == MFW_OS_LINUX
		struct char_auto_destroy final
		{
			using char_type = stl::oschar_t;
			using string_type = stl::osstring;
			using string_view_type = stl::osstring_view;
		
			char_auto_destroy() noexcept = default;
			char_auto_destroy(char_auto_destroy &&) noexcept = default;
			char_auto_destroy &operator=(char_auto_destroy &&) noexcept = default;
			char_auto_destroy(nullptr_t) noexcept {}
			
			char_auto_destroy &assign(string_view_type src) noexcept {
				destroy();
				if(!src.empty()) {
					stl::size_t len{src.length()+1};
					m_ptr = new char_type[len]{MFW_T('\0')};
					::MFW_STD_NAMESPACE::strncpy(m_ptr, src.data(), len);
				}
				return *this;
			}
			
			char_auto_destroy &assign(const char_type *src) noexcept {
				string_view_type tmp{src};
				assign(tmp);
				return *this;
			}
			
			char_auto_destroy &assign(string_type &&src) noexcept {
				string_view_type tmp{src};
				assign(tmp);
				src.clear();
				return *this;
			}
			
			char_auto_destroy &assign(stl::pstring &&src) noexcept {
				string_view_type tmp{src};
				assign(tmp);
				src.clear();
				return *this;
			}

			char_auto_destroy &assign(stl::pstring_view src) noexcept {
				string_view_type tmp{src};
				assign(tmp);
				return *this;
			}
			
			char_auto_destroy(const char_auto_destroy &other) noexcept
			{ assign(other.ptr); }

			char_auto_destroy &operator=(const char_auto_destroy &other) noexcept;
			{ assign(other.ptr); return *this; }
			
			char_auto_destroy(string_view_type src) noexcept
			{ assign(src); }
			
			char_auto_destroy(const char_type *src) noexcept
			{ assign(src); }
			
			char_auto_destroy(string_type &&src) noexcept
			{ assign(stl::move(src)); }
			
			char_auto_destroy(stl::pstring_view src) noexcept
			{ assign(src); }

			char_auto_destroy(stl::pstring &&src) noexcept
			{ assign(stl::move(src)); }
			
			char_auto_destroy &destroy() noexcept {
				if(m_ptr) {
					delete[] m_ptr;
				}
				m_ptr = nullptr;
				return *this;
			}
			
			~char_auto_destroy() noexcept
			{
				//destroy();
			}
			
			operator bool() const noexcept
			{ return !!m_ptr; }
			
			operator char_type *() noexcept
			{ return m_ptr; }
			operator const char_type *() const noexcept
			{ return m_ptr; }

			const char_type *c_str() const noexcept
			{ return m_ptr; }
			
			string_view_type string_view() const noexcept
			{ return string_view_type{m_ptr}; }
		
		private:
			char_type *m_ptr{nullptr};
		};
		
		class vector_argv final : public stl::vector<char_auto_destroy>
		{
		public:
			using super = stl::vector<char_auto_destroy>;

			using super::vector;
		
			const char_auto_destroy::char_t **data() const noexcept
			{ return reinterpret_cast<const char_auto_destroy::char_t **>(super::data()); }
			
			const char_auto_destroy::char_t *path() const noexcept
			{ return front().c_str(); }
		};
	
		static void str_to_argv(stl::osstring_view str, vector_argv &argv) noexcept
		{
			if(str.empty()) {
				return;
			}

			stl::vector<stl::osstring> dst{};
			expand_shell(str, dst);
			for(const stl::osstring &src : dst) {
				if(src.empty()) {
					continue;
				}
				argv.emplace_back(src);
			}
			argv.emplace_back();
		}
	#endif

		MFW_VISIBILITY_LOCAL_POP()
	}
	
	MFW_CORE_API process & MFW_CORE_CALL process::operator=(const process &other) noexcept
	{
		kill();

		m_workingdir = other.m_workingdir;
		m_args = other.m_args;
		m_path = other.m_path;
		m_output = other.m_output;
		m_status = other.m_status;
		m_handle = other.m_handle;
		m_shell_command = other.m_shell_command;
		m_cmd = other.m_cmd;
		m_stdout_handle = other.m_stdout_handle;
		m_can_kill = other.m_can_kill;

		const_cast<process &>(other).m_handle = invalid_handle;
		const_cast<process &>(other).m_stdout_handle = invalid_stdout_handle;
		const_cast<process &>(other).kill();

		return *this;
	}

	MFW_CORE_API void MFW_CORE_CALL process::set_workingdir(searchpath_view_type search) noexcept
	{
		m_workingdir = interfaces::filesystem::instance().resolve(search, false);
	}
	
	MFW_CORE_API void MFW_CORE_CALL process::set_shell() noexcept
	{
		m_shell_command = true;
		m_path = __private_process_cpp_internal::get_shell_path();
		m_cmd.clear();
	}
	
	MFW_CORE_API process::string_type MFW_CORE_CALL process::args_absolute() const noexcept
	{
		string_type tmp{};
		__process_internal::args_absolute(tmp, *this, false);
		return tmp;
	}

	MFW_CORE_API bool MFW_CORE_CALL process::start(bool w) noexcept
	{
		kill();
		m_output.clear();
	#if MFW_OS_IS(LINUX)
		m_status = 0;
	#endif

		if(m_path.empty()) {
			return false;
		}

	#if MFW_OS_IS(WINDOWS)
		HANDLE out_read{nullptr};
		HANDLE out_write{nullptr};

		if(output) {
			SECURITY_ATTRIBUTES security{};
			security.nLength = sizeof(SECURITY_ATTRIBUTES);
			security.bInheritHandle = TRUE;
			security.lpSecurityDescriptor = nullptr;

			CreatePipe(&out_read, &out_write, &security, 0);

			SetHandleInformation(out_read, HANDLE_FLAG_INHERIT, 0);
		}

		STARTUPINFOW sinfo{};
		sinfo.cb = sizeof(STARTUPINFOW);
		sinfo.wShowWindow = SW_HIDE;
		if(output) {
			sinfo.dwFlags = STARTF_USESTDHANDLES;
			sinfo.hStdOutput = out_write;
			sinfo.hStdError = out_write;
		}

		PROCESS_INFORMATION pinfo{};

		core::filesystem &filesys{core::filesystem::instance()};

		pstring filepath{filesys.resolve(search)};

		ucstring command{};
		if(!filepath.empty()) {
			command += filepath.ucstring();
			command += u' ';
		}
		command += args;

		pstring workdir{filesys.get_working_dir()};
		if(!CreateProcessW(nullptr, c_str(command), nullptr, nullptr, (output ? TRUE : FALSE), CREATE_NO_WINDOW|CREATE_UNICODE_ENVIRONMENT, nullptr, c_str(workdir), &sinfo, &pinfo)) {

			if(output) {
				CloseHandle(out_read);
				CloseHandle(out_write);
			}

			CloseHandle(pinfo.hThread);
			CloseHandle(pinfo.hProcess);

			return EXIT_FAILURE;
		}

		::std::chrono::milliseconds time{INFINITE};
		WaitForSingleObject(pinfo.hProcess, static_cast<unsigned long>(time.count()));

		if(output) {
			LARGE_INTEGER large{};
			GetFileSizeEx(out_read, &large);

			if(large.QuadPart > 0) {
				ucstring str{};
				str.resize(static_cast<size_t>(large.QuadPart));

				if(ReadFile(out_read, str.data(), static_cast<unsigned long>(str.size()), nullptr, nullptr)) {
					convert(str, *output);

					replace_all(*output, u"\r\n"_sv, u"\n"_sv);
				}
			}

			CloseHandle(out_read);
			CloseHandle(out_write);
		}

		uint32_t ret{EXIT_FAILURE};
		GetExitCodeProcess(pinfo.hProcess, reinterpret_cast<unsigned long *>(&ret));

		CloseHandle(pinfo.hThread);
		CloseHandle(pinfo.hProcess);

		return static_cast<int32_t>(ret);
	#elif MFW_OS_IS(LINUX)
		::MFW_STD_NAMESPACE::fflush(::MFW_STD_NAMESPACE::stdout);
		stl::int32_t fd[2]{-1};
		::MFW_STD_NAMESPACE::pipe2(fd, 0);
		handle = ::MFW_STD_NAMESPACE::fork();
		if(handle == -1) {
			::MFW_STD_NAMESPACE::close(fd[0]);
			::MFW_STD_NAMESPACE::close(fd[1]);
			return false;
		} else {
			::MFW_STD_NAMESPACE::setpgid(handle, handle);
			if(handle == 0) {
				::MFW_STD_NAMESPACE::close(fd[0]);
				::MFW_STD_NAMESPACE::dup3(fd[1], ::MFW_STD_NAMESPACE::STDOUT_FILENO, 0);
				::MFW_STD_NAMESPACE::dup3(fd[1], ::MFW_STD_NAMESPACE::STDERR_FILENO, 0);
				interfaces::filesystem &filesys{filesystem::instance()};
				pstring lastwork{};
				if(!workingdir_.empty()) {
					lastwork = filesys.get_working_dir();
					filesys.set_working_dir({workingdir_});
				}
				__process_internal::argv_t argv{};
				string_type tmp{};
				__process_internal::args_absolute(tmp, *this, true);
				__process_internal::copy_to_argv(m_path, argv, m_shell_command);
				__process_internal::str_to_argv(tmp, argv, m_shell_command);
				bool error{execvpe(argv.path(), argv.data(), environ) == -1};
				argv.clear();
				if(!lastwork.empty()) {
					filesys.set_working_dir({lastwork});
				}
				if(error) {
					exit(W_EXITCODE(1, SIGTERM));
				} else {
					exit(W_EXITCODE(0, SIGTERM));
				}
			} else {
				::close(fd[1]);
				FILE *stream{fdopen(fd[0], "rb")};
				stdout_handle = new file{stream, {}};
				if(w) {
					wait();
					if(WIFSTOPPED(status_) || WIFSIGNALED(status_)) {
						return false;
					}
				}
				return true;
			}
		}
	#else
		#error
	#endif
	}

	MFW_CORE_API void MFW_CORE_CALL process::close(bool kill) noexcept
	{
		if(handle == invalid_handle) {
			return;
		}

		capture_vars();

		delete stdout_handle;
	#if MFW_OS == MFW_OS_LINUX
		//::kill(-handle, kill ? SIGKILL : SIGTERM);
		::killpg(handle, kill ? SIGKILL : SIGTERM);
		int32_t tmp{0};
		waitpid(handle, &tmp, 0);
	#else
		#error
	#endif

		stdout_handle = invalid_stdout_handle;
		handle = invalid_handle;
	}

	MFW_CORE_API bool MFW_CORE_CALL process::running() noexcept
	{
		if(handle == invalid_handle) {
			return false;
		}

	#if MFW_OS == MFW_OS_LINUX
		int32_t ret{waitpid(handle, &status_, WNOHANG|WUNTRACED|WEXITED|WCONTINUED)};
		if(ret == -1) {
			kill();
			return false;
		} else if(ret == 0) {
			return true;
		} else {
			bool running{!(WIFEXITED(status_) || WIFSTOPPED(status_) || WIFSIGNALED(status_) || WIFCONTINUED(status_))};
			if(!running) {
				kill();
			}
			return running;
		}
	#else
		#error
	#endif
	}

	MFW_CORE_API void MFW_CORE_CALL process::wait() noexcept
	{
		if(handle == invalid_handle) {
			return;
		}

		while(running()) {}
	}

	void process::capture_vars(size_t max)
	{
		exit_code();
		output(max);
	}

	MFW_CORE_API int32_t MFW_CORE_CALL process::exit_code() noexcept
	{
		if(handle != invalid_handle) {
		#if MFW_OS == MFW_OS_LINUX
			waitpid(handle, &status_, WNOHANG|WUNTRACED|WEXITED|WCONTINUED);
		#else
			#error
		#endif
		}

	#if MFW_OS == MFW_OS_LINUX
		return WEXITSTATUS(status_);
	#else
		#error
	#endif
	}

	MFW_CORE_API const ucstring & MFW_CORE_CALL process::output(size_t max) noexcept
	{
		if(stdout_handle != invalid_stdout_handle) {
			output_.clear();
			if(max == 0) {
				while(true) {
					char8_t c{u8'\0'};
					size_t read{stdout_handle->read(&c, 1)};
					if(read == 0) {
						break;
					}
					output_ += c;
				}
			} else {
				output_.resize(max);
				stdout_handle->read(output_.data(), output_.size());
			}
			if(!output_.empty()) {
				output_.pop_back();
			}
		}

		return output_;
	}
	
	MFW_CORE_API pstring MFW_CORE_CALL process::get_path(const ucstring &cmd) noexcept
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
		
		ucstring newcmd{};
		expand_env_vars(cmd, newcmd);
		
		pstring tmp{filesys.resolve({newcmd})};
		if(!tmp.empty()) {
			return tmp;
		}
		
	#if MFW_OS == MFW_OS_LINUX
		process proc{};
		proc.set_shell(u8"which"_s);
		proc.set_args(newcmd);
		proc.start(true);
		if(proc.exit_code() != 0) {
			return tmp;
		}
		tmp = proc.output();
	#else
		#error
	#endif
	
		return tmp;
	}
	
	MFW_CORE_API bool MFW_CORE_CALL process::find(string_view_type name, process &proc) noexcept
	{
		if(name.empty()) {
			return false;
		}
		
		vector<process> procs{};
		bool found{find(name, procs)};
		if(found) {
			proc = procs[0];
		}
		return found;
	}
	
	MFW_CORE_API bool MFW_CORE_CALL process::find(string_view_type name, stl::vector<process> &procs) noexcept
	{
		if(name.empty()) {
			return false;
		}
		
	#if MFW_OS_IS(LINUX)
		using stl::filesystem::directory_iterator;
		using stl::filesystem::read_symlink;

		directory_iterator proc{MFW_T("/proc"_p)};
		for(directory_iterator it : proc) {
			pstring path{*it};

			stl::int32_t pid{0};
			if(!to_int(path.native(), pid)) {
				continue;
			}

			path = MFW_T("/proc/{}/exe"_fmt(pid));

			stl::error_code errc{};
			path = read_symlink(stl::move(path), errc);
			if(!errc) {
				continue;
			}

			

			process &proc{procs.emplace_back()};
		}

		::MFW_STD_NAMESPACE::DIR *procdir{::MFW_STD_NAMESPACE::opendir("/proc")};
		if(procdir) {
			::MFW_STD_NAMESPACE::dirent *entry{nullptr};
			while(true) {
				entry = ::MFW_STD_NAMESPACE::readdir(procdir);
				if(!entry) {
					break;
				}
				
				stl::int32_t pid{0};
				if(!to_int(entry->d_name, pid)) {
					continue;
				}
				
				char exefile[PATH_MAX]{'\0'};
				ucstring tmp{u8"/proc/{}/exe"_fmt(pid)};
				ssize_t len{readlink(c_str(tmp), exefile, size(exefile))};
				if(len == -1) {
					continue;
				}
				
				pstring exepath{};
				exepath.assign(exefile, exefile+len);
				
				tmp = MFW_T("/proc/{}/status"_fmt(pid));
				FILE *cmdline{fopen(c_str(tmp), "r")};
				len = core::file::get_handle_size(cmdline);
				tmp.resize(len);
				fread(c_str(tmp), sizeof(ucchar_t), len, cmdline);
				fclose(cmdline);
				
				tmp.erase(0, 6);
				
				len = tmp.find(u8'\n', 0);
				tmp.erase(len);
				
				if(tmp != name) {
					continue;
				}
				
				process &proc{procs.emplace_back()};
				
				proc.handle = pid;
				proc.can_kill_ = false;
				
				waitpid(pid, &proc.status_, WNOHANG|WUNTRACED|WEXITED|WCONTINUED);
				
				proc.set_path({exepath});
				
				tmp = u8"/proc/{}/cmdline"_fmt(pid);
				cmdline = fopen(c_str(tmp), "r");
				len = core::file::get_handle_size(cmdline);
				proc.args_.resize(len);
				fread(c_str(proc.args_), sizeof(ucchar_t), len, cmdline);
				fclose(cmdline);
				replace_all(proc.args_, u8'\0', u8' ');
				
				tmp = u8"/proc/{}/cwd"_fmt(pid);
				len = readlink(c_str(tmp), exefile, size(exefile));
				
				proc.workingdir_.assign(exefile, exefile+len);
			}
			closedir(procdir);
		}
		return !procs.empty();
	#else
		#error
	#endif
	}

	MFW_VISIBILITY_LOCAL_POP()
}