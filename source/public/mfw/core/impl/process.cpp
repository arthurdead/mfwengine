#include <public/mfw/core/process.hpp>

namespace mfw::core
{
	MFW_VISIBILITY_LOCAL_PUSH()

	process::process(const process &other) noexcept
	{ operator=(other); }

	const stl::pstring &process::workingdir() const noexcept
	{ return m_workingdir; }

	process::string_type &process::args() noexcept
	{ return m_args; }
	const process::string_type &process::args() const noexcept
	{ return m_args; }

	const stl::pstring &process::path() const noexcept
	{ return m_path; }
	bool process::shell_command() const noexcept
	{ return m_shell_command; }
	const process::string_type &process::cmd() const noexcept
	{ return m_cmd; }
	process::string_type &process::cmd() noexcept
	{ return m_cmd; }

	process &process::kill() noexcept
	{ close(true); return *this; }

	const process::string_type &process::output() const noexcept
	{ return m_output; }

	process::~process() noexcept {
		if(m_can_kill) {
			kill();
		}
	}

	void process::set_shell(string_type &&cmd) noexcept
	{
		set_shell();
		m_cmd = stl::move(cmd);
	}

	void process::set_path(searchpath_view_type search) noexcept
	{
		m_shell_command = false;
		m_cmd.clear();
		m_path = filesystem::instance().resolve(search);
	}

	MFW_VISIBILITY_LOCAL_POP()
}