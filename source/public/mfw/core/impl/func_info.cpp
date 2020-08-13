namespace mfw::core
{
	void func_info::clear() noexcept
	{
		m_name.clear();
		m_args_info.clear();
		m_return_info.clear();
		m_this_info.clear();
		m_funcptr = nullptr;
		m_virtualindex = static_cast<stl::size_t>(-1);
	}

	string_type &func_info::name() noexcept
	{ return m_name; }
	const string_type &func_info::name() const noexcept
	{ return m_name; }

	func_info::type_info_type &func_info::this_info() noexcept
	{ return m_this_info; }
	const func_info::type_info_type &func_info::this_info() const noexcept
	{ return m_this_info; }

	func_info::type_info_type &func_info::return_info() noexcept
	{ return m_return_info; }
	const func_info::type_info_type &func_info::return_info() const noexcept
	{ return m_return_info; }

	func_info::arg_vec_t &func_info::args() noexcept
	{ return m_args_info; }
	const func_info::arg_vec_t &func_info::args() const noexcept
	{ return m_args_info; }

	func_info::type_info_type &func_info::arg(stl::size_t i) noexcept
	{ return m_args_info[i]; }
	const func_info::type_info_type &func_info::arg(stl::size_t i) const noexcept
	{ return m_args_info[i]; }

	func_info::type_info_type &func_info::operator[](stl::size_t i) noexcept
	{ return m_args_info[i]; }
	const func_info::type_info_type &func_info::operator[](stl::size_t i) const noexcept
	{ return m_args_info[i]; }

	void func_info::erase(iterator it) noexcept
	{ m_args_info.erase(it); }
	void func_info::erase(const_iterator it) noexcept
	{ m_args_info.erase(it); }

	stl::size_t func_info::size() const noexcept
	{ return m_args_info.size(); }

	bool func_info::empty() const noexcept
	{ return m_args_info.empty(); }

	func_info::const_iterator func_info::cbegin() const noexcept
	{ return m_args_info.cbegin(); }
	func_info::const_iterator func_info::cend() const noexcept
	{ return m_args_info.cend(); }

	func_info::const_reverse_iterator func_info::crbegin() const noexcept
	{ return m_args_info.crbegin(); }
	func_info::const_reverse_iterator func_info::crend() const noexcept
	{ return m_args_info.crend(); }

	func_info::iterator func_info::begin() noexcept
	{ return m_args_info.begin(); }
	func_info::iterator func_info::end() noexcept
	{ return m_args_info.end(); }

	func_info::reverse_iterator func_info::rbegin() noexcept
	{ return m_args_info.rbegin(); }
	func_info::reverse_iterator func_info::rend() noexcept
	{ return m_args_info.rend(); }

	func_info::const_iterator func_info::begin() const noexcept
	{ return m_args_info.begin(); }
	func_info::const_iterator func_info::end() const noexcept
	{ return m_args_info.end(); }

	func_info::const_reverse_iterator func_info::rbegin() const noexcept
	{ return m_args_info.rbegin(); }
	func_info::const_reverse_iterator func_info::rend() const noexcept
	{ return m_args_info.rend(); }
}