#include <public/mfw/core/serializable.hpp>

namespace mfw::core
{
	serializable_parser_callbacks::const_optional_child serializable_parser_callbacks::get_inherit(string_view_type name) const
	{ return stl::nullopt; }

	const serializable_parser_callbacks::include_dirs_t &serializable_parser_callbacks::include_dirs() const noexcept
	{ return m_include_dirs; }

	serializable::serializable(const serializable &other) noexcept
	{ operator=(other); }

	bool serializable::operator!=(const serializable &other) const noexcept
	{ return !operator==(other); }

	string_type &serializable::name() noexcept
	{ return m_name; }
	const string_type &serializable::name() const noexcept
	{ return m_name; }

	univalue &serializable::value() noexcept
	{ return m_value; }
	const univalue &serializable::value() const noexcept
	{ return m_value; }

	bool serializable::has_value() const noexcept
	{ return !m_value.empty(); }
	
	bool serializable::get_value_bool() const noexcept {
		if(m_value.empty()) {
			return true;
		} else {
			return m_value.get_bool();
		}
	}

	optional_child serializable::parent() noexcept
	{ return m_parent; }
	const_optional_child serializable::parent() const noexcept
	{ return m_parent; }

	bool serializable::has_parent() const noexcept
	{ return m_parent.has_value(); }

	optional_child serializable::get_flags() noexcept {
		if(m_flags) {
			return optional_child{*m_flags.get()};
		} else {
			return stl::nullopt;
		}
	}
	const_optional_child serializable::get_flags() const noexcept {
		if(m_flags) {
			return const_optional_child{*m_flags.get()};
		} else {
			return stl::nullopt;
		}
	}

	bool serializable::has_flags() const noexcept
	{ return static_cast<bool>(m_flags); }

	optional_child serializable::get_flag(string_view_type name) noexcept {
		optional_child tmp{get_flags()};
		if(tmp.has_value()) {
			return tmp->get_child(name);
		} else {
			return stl::nullopt;
		}
	}

	const_optional_child serializable::get_flag(string_view_type name) const noexcept {
		const_optional_child tmp{get_flags()};
		if(tmp.has_value()) {
			return tmp->get_child(name);
		} else {
			return stl::nullopt;
		}
	}

	bool serializable::get_flag_bool(string_view_type name) const noexcept {
		const_optional_child tmp{get_flags()};
		if(tmp.has_value()) {
			return tmp->get_child_bool(name);
		} else {
			return false;
		}
	}

	string_type &serializable::condition() noexcept
	{ return m_condition; }
	const string_type &serializable::condition() const noexcept
	{ return m_condition; }

	bool serializable::has_condition() const noexcept
	{ return !m_condition.empty(); }

	serializable &serializable::create_child(string_view_type name) noexcept
	{ return create_child(name, cend()); }
	serializable &serializable::create_child(string_type &&name) noexcept
	{ return create_child(stl::move(name), cend()); }

	const_optional_child serializable::get_child(stl::size_t i) const noexcept {
		if(i < m_childs.size()) {
			return const_optional_child{m_childs[i]};
		} else {
			reutrn stl::nullopt;
		}
	}
	const_optional_child serializable::operator[](stl::size_t i) const noexcept
	{ return get_child(i); }
	const_optional_child serializable::operator[](string_view_type name) const noexcept {
		const_optional_child tmp{get_child(name)};
		if(tmp.has_value()) {
			return tmp;
		} else {
			return stl::nullopt;
		}
	}
	serializable &serializable::operator[](string_view_type name) noexcept
	{ return child(name); }
	serializable &serializable::operator[](string_type &&name) noexcept
	{ return child(stl::move(name)); }
	bool serializable::has_child(string_view_type name) const noexcept
	{ return get_child(name).has_value(); }

	bool serializable::get_child_bool(string_view_type name) const noexcept {
		const_optional_child child_{get_child(name)};
		if(child_.has_value()) {
			return child->get_value_bool();
		} else {
			return false;
		}
	}

	bool serializable::has_flag(string_view_type name) const noexcept
	{ return (has_flags() && get_flags()->has_child(name)); }
	serializable &serializable::add_flag(string_view_type name) noexcept
	{ flags().child(name); return *this; }
	serializable &serializable::add_flag(string_type &&name) noexcept
	{ flags().child(stl::move(name)); return *this; }

	const_optional_value serializable::get_value(string_view_type name) const noexcept {
		const_optional_child child_{get_child(name)};
		if(child_.has_value()) {
			return const_optional_value{child->m_value};
		} else {
			return stl::nullopt;
		}
	}

	optional_value serializable::get_value(string_view_type name) noexcept {
		optional_child child_{get_child(name)};
		if(child_.has_value()) {
			return optional_value{child->m_value};
		} else {
			return stl::nullopt;
		}
	}

	serializable &serializable::remove_all() noexcept
	{ m_childs.clear(); return *this; }

	serializable &serializable::erase(iterator it) noexcept
	{ m_childs.erase(it); return *this; }
	serializable &serializable::erase(const_iterator it) noexcept
	{ m_childs.erase(it); return *this; }

	stl::size_t serializable::size() const noexcept
	{ return m_childs.size(); }

	bool serializable::empty() const noexcept
	{ return m_childs.empty(); }

	serializable::const_iterator serializable::cbegin() const noexcept
	{ return m_childs.cbegin(); }
	serializable::const_iterator serializable::cend() const noexcept
	{ return m_childs.cend(); }

	serializable::const_reverse_iterator serializable::crbegin() const noexcept
	{ return m_childs.crbegin(); }
	serializable::const_reverse_iterator serializable::crend() const noexcept
	{ return m_childs.crend(); }

	serializable::iterator serializable::begin() noexcept
	{ return m_childs.begin(); }
	serializable::iterator serializable::end() noexcept
	{ return m_childs.end(); }

	serializable::reverse_iterator serializable::rbegin() noexcept
	{ return m_childs.rbegin(); }
	serializable::reverse_iterator serializable::rend() noexcept
	{ return m_childs.rend(); }

	serializable::const_iterator serializable::begin() const noexcept
	{ return m_childs.begin(); }
	serializable::const_iterator serializable::end() const noexcept
	{ return m_childs.end(); }

	serializable::const_reverse_iterator serializable::rbegin() const noexcept
	{ return m_childs.rbegin(); }
	serializable::const_reverse_iterator serializable::rend() const noexcept
	{ return m_childs.rend(); }

	serializable &serializable::find_or_emplace(string_view_type name) noexcept
	{ return find_or_emplace(name, cend()); }

	serializable &serializable::find_or_emplace(string_view_type name, const_iterator it) noexcept
	{
		optional_child tmp{get_child(name)};
		if(tmp.has_value()) {
			return *tmp;
		} else {
			return create_child(name, it);
		}
	}

	serializable &serializable::find_or_emplace(string_type &&name) noexcept
	{ return find_or_emplace(stl::move(name), cend()); }

	serializable &serializable::find_or_emplace(string_type &&name, const_iterator it) noexcept
	{
		optional_child tmp{get_child(name)};
		if(tmp.has_value()) {
			return *tmp;
		} else {
			return create_child(stl::move(name), it);
		}
	}

	optional_child serializable::get_child(string_view_type name) noexcept
	{
		iterator it{find(str)};
		if(it != end()) {
			return optional_child{*it};
		} else {
			return stl::nullopt;
		}
	}

	const_optional_child serializable::get_child(string_view_type name) const noexcept
	{
		const_iterator it{find(str)};
		if(it != cend()) {
			return const_optional_child{*it};
		} else {
			return stl::nullopt;
		}
	}

	serializable &serializable::child(string_view_type name) noexcept
	{
		optional_child tmp{get_child(name)};
		if(!tmp.has_value()) {
			return *tmp;
		} else {
			return create_child(name);
		}
	}

	serializable &serializable::child(string_type &&name) noexcept
	{
		optional_child tmp{get_child(name)};
		if(!tmp.has_value()) {
			return *tmp;
		} else {
			return create_child(stl::move(name));
		}
	}

	const_optional_child serializable::child(string_view_type name) const noexcept
	{ return get_child(name); }

	const_optional_child serializable::flags() const noexcept {
		if(m_flags) {
			return const_optional_child{*m_flags.get()};
		} else {
			return stl::nullopt;
		}
	}

	serializable &serializable::flags() noexcept {
		if(!m_flags) {
			return create_flags();
		} else {
			return *m_flags.get();
		}
	}

	void serializable::create_child(string_view_type name, const_iterator it) noexcept {
		serializable &child{create_child_base(name, it)};
		child.m_name = name;
		return child;
	}

	void serializable::create_child(string_type &&name, const_iterator it) noexcept {
		serializable &child{create_child_base(name, it)};
		child.m_name = stl::move(name);
		return child;
	}

	void serializable::clear() noexcept {
		m_name.clear();
		m_value.clear();
		m_flags.reset(nullptr);
		m_condition.clear();
		m_childs.clear();
	}
}