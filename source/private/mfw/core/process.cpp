#include <public/mfw/core/process.hpp>
#include <public/mfw/core/filesystem_interface.hpp>
#include <private/mfw/core/file.hpp>
#include <public/mfw/core/environment.hpp>
#if MFW_OS == MFW_OS_WINDOWS
	#include <Windows.h>
#elif MFW_OS == MFW_OS_LINUX
	#include <public/mfw/stl/vector.hpp>
	#include <unistd.h>
	#include <signal.h>
	#include <sys/wait.h>
	#include <dirent.h>
	#include <cstring>
#endif

#include <public/mfw/core/logging_interface.hpp>

MFW_DECLARE_LOG_CONTEXT(log_process, u8"core/process"_p)

namespace mfw::core
{
	namespace __process_internal
	{
		static pstring get_shell_path()
		{
		#if MFW_OS == MFW_OS_LINUX
			pstring path{};
			environment_var var{u8"SHELL"_sv};
			if(var.is_set()) {
				path = var.value();
			} else {
				path = u8"/bin/sh"_p;
			}
			return path;
		#else
			return pstring{u8"C:\\Windows\\cmd.exe"_p};
		#endif
		}
		
		static void begin_shell_arg(ucstring &str)
		{
		#if MFW_OS == MFW_OS_LINUX
			str += u8"-c \""_sv;
		#endif
		}
		
		static void end_shell_arg(ucstring &str)
		{
		#if MFW_OS == MFW_OS_LINUX
			str += u8'"';
		#endif
		}
		
		static void args_absolute(ucstring &tmp, const process &proc, bool exec)
		{
			bool shell_command_{proc.shell_command()};
			const ucstring &cmd_{proc.cmd()};
			const ucstring &args_{proc.args()};
			
			if(shell_command_) {
				__process_internal::begin_shell_arg(tmp);
				if(!cmd_.empty()) {
					tmp += cmd_;
					if(!args_.empty()) {
						tmp += u8' ';
					}
				}
			}
			tmp += args_;
			if(shell_command_) {
				__process_internal::end_shell_arg(tmp);
			}
		}
		
	#if MFW_OS == MFW_OS_LINUX
		struct char_auto_destroy_t
		{
		public:
			using char_t = char;
		
			char_auto_destroy_t() = default;
			char_auto_destroy_t(char_auto_destroy_t &&) = default;
			char_auto_destroy_t(nullptr_t) {}
			
			void set(ucstring_view src) {
				destroy();
				if(src.empty()) {
					return;
				}
				size_t len{src.length()+1};
				ptr = new char_t[len]{'\0'};
				strncpy(ptr, c_str(src), len);
			}
			
			void set(char_t *src) {
				set(ucstring_view{reinterpret_cast<const ucchar_t *>(src)});
			}
			
			void set(const ucstring &src) {
				set(ucstring_view{src.c_str(), src.length()});
			}
			
			void set(const pstring &src) {
				set(ucstring_view{reinterpret_cast<const ucchar_t *>(src.c_str()), src.native().length()});
			}
			
			char_auto_destroy_t(const char_auto_destroy_t &other) {
				set(other.ptr);
			}
			
			char_auto_destroy_t(ucstring_view src) {
				set(src);
			}
			
			char_auto_destroy_t(char_t *src) {
				set(src);
			}
			
			char_auto_destroy_t(const ucstring &src) {
				set(src);
			}
			
			char_auto_destroy_t(const pstring &src) {
				set(src);
			}
			
			void destroy() {
				if(ptr) {
					delete[] ptr;
				}
				ptr = nullptr;
			}
			
			~char_auto_destroy_t() {
				//destroy();
			}
			
			operator bool() const { return ptr != nullptr; }
			
			operator char_t *() { return ptr; }
			operator const char_t *() const { return ptr; }
			
			ucstring str() const {
				return ucstring{reinterpret_cast<const ucchar_t *>(ptr)};
			}
		
			char_t *ptr{nullptr};
		};
		
		class argv_t : public vector<char_auto_destroy_t>
		{
		public:
			using super = vector<char_auto_destroy_t>;
		
			char_auto_destroy_t::char_t **data() {
				return reinterpret_cast<char **>(super::data());
			}
			
			char_auto_destroy_t::char_t *path() {
				return (*this)[0].ptr;
			}
		};
	
		static void copy_to_argv(const pstring &src, argv_t &argv, bool shell)
		{
			if(src.empty()) {
				return;
			}
			argv.emplace_back(src);
		}
	
		static void str_to_argv(const ucstring &str, argv_t &argv, bool shell)
		{
			if(str.empty()) {
				return;
			}

			vector<ucstring> dst{};
			expand_shell(str, dst);
			for(const ucstring &src : dst) {
				if(src.empty()) {
					continue;
				}
				argv.emplace_back(src);
			}
			argv.emplace_back();
		}
	#endif
	}
	
	MFW_CORE_API process & MFW_CORE_CALL process::operator=(const process &other)
	{
		kill();

		workingdir_ = other.workingdir_;
		args_ = other.args_;
		path_ = other.path_;
		output_ = other.output_;
		status_ = other.status_;
		handle = other.handle;
		shell_command_ = other.shell_command_;
		cmd_ = other.cmd_;
		stdout_handle = other.stdout_handle;
		can_kill_ = other.can_kill_;

		const_cast<process &>(other).handle = invalid_handle;
		const_cast<process &>(other).stdout_handle = invalid_stdout_handle;
		const_cast<process &>(other).kill();

		return *this;
	}
	
	MFW_CORE_API MFW_CORE_CALL process::~process()
	{
		if(can_kill_) {
			kill();
		}
	}

	MFW_CORE_API void MFW_CORE_CALL process::set_workingdir(const searchpath &search)
	{
		workingdir_ = interfaces::filesystem::instance().resolve(search, false);
	}
	
	MFW_CORE_API void MFW_CORE_CALL process::set_args(const ucstring &args)
	{
		args_ = args;
	}
	
	MFW_CORE_API void MFW_CORE_CALL process::set_shell()
	{
		shell_command_ = true;
		path_ = __process_internal::get_shell_path();
		cmd_.clear();
	}
	
	MFW_CORE_API void MFW_CORE_CALL process::set_shell(const ucstring &cmd)
	{
		set_shell();
		cmd_ = cmd;
	}

	MFW_CORE_API void MFW_CORE_CALL process::set_path(const searchpath &search)
	{
		shell_command_ = false;
		cmd_.clear();
		path_ = interfaces::filesystem::instance().resolve(search);
	}
	
	MFW_CORE_API ucstring MFW_CORE_CALL process::args_absolute() const
	{
		ucstring tmp{};
		__process_internal::args_absolute(tmp, *this, false);
		return tmp;
	}

	MFW_CORE_API bool MFW_CORE_CALL process::start(bool w)
	{
		kill();
		output_.clear();
		status_ = 0;

		if(path_.empty()) {
			return false;
		}

	#if MFW_OS == MFW_OS_WINDOWS
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
	#elif MFW_OS == MFW_OS_LINUX
		fflush(stdout);
		int32_t fd[2]{-1};
		pipe2(fd, 0);
		handle = fork();
		if(handle == -1) {
			::close(fd[0]);
			::close(fd[1]);
			return false;
		} else {
			setpgid(handle, handle);
			if(handle == 0) {
				::close(fd[0]);
				dup3(fd[1], STDOUT_FILENO, 0);
				dup3(fd[1], STDERR_FILENO, 0);
				interfaces::filesystem &filesys{interfaces::filesystem::instance()};
				pstring lastwork{};
				if(!workingdir_.empty()) {
					lastwork = filesys.get_working_dir();
					filesys.set_working_dir({workingdir_});
				}
				__process_internal::argv_t argv{};
				ucstring tmp{};
				__process_internal::args_absolute(tmp, *this, true);
				__process_internal::copy_to_argv(path_, argv, shell_command_);
				__process_internal::str_to_argv(tmp, argv, shell_command_);
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

	MFW_CORE_API void MFW_CORE_CALL process::close(bool kill)
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

	MFW_CORE_API bool MFW_CORE_CALL process::running()
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

	MFW_CORE_API void MFW_CORE_CALL process::wait()
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

	MFW_CORE_API int32_t MFW_CORE_CALL process::exit_code()
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

	MFW_CORE_API const ucstring & MFW_CORE_CALL process::output(size_t max)
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
	
	MFW_CORE_API pstring MFW_CORE_CALL process::get_path(const ucstring &cmd)
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
	
	MFW_CORE_API bool MFW_CORE_CALL process::find(ucstring_view name, process &proc)
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
	
	MFW_CORE_API bool MFW_CORE_CALL process::find(ucstring_view name, vector<process> &procs)
	{
		if(name.empty()) {
			return false;
		}
		
	#if MFW_OS == MFW_OS_LINUX
		DIR *procdir{opendir("/proc")};
		if(procdir) {
			dirent *entry{nullptr};
			while(true) {
				entry = readdir(procdir);
				if(!entry) {
					break;
				}
				
				int32_t pid{0};
				if(!to_int(uc_str(entry->d_name), pid)) {
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
				
				tmp = u8"/proc/{}/status"_fmt(pid);
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
}