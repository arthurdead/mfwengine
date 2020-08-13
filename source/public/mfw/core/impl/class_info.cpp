#include <public/mfw/core/classinfo.hpp>

namespace mfw::core
{
	class_info::class_info(string_view_type name) noexcept
		: m_name{name} {}
	class_info::class_info(string_type &&name) noexcept
		: m_name{stl::move(name)} {}

	class_info::member_variable_info &class_info::member_variable_info::clear() noexcept
	{
		m_name.clear();
		m_offset = 0;
		m_var = nullptr;
		m_set_func = nullptr;
		m_get_func = nullptr;
		return *this;
	}

	class_info &class_info::clear() noexcept
	{
		super::clear();
		m_name.clear();
		m_variables.clear();
		m_functions.clear();
		return *this;
	}

	class_info::optional_variable class_info::variable(string_view_type name) noexcept
	{
		for(member_variable_info &it : m_variables) {
			if(it.m_name == name) {
				return optional_variable{it};
			}
		}
		return stl::nullopt;
	}

	class_info::const_optional_variable class_info::variable(string_view_type name) const noexcept
	{
		for(const member_variable_info &it : m_variables) {
			if(it.m_name == name) {
				return const_optional_variable{it};
			}
		}
		return stl::nullopt;
	}

	class_info::optional_function class_info::function(string_view_type name) noexcept
	{
		for(func_info_type &it : m_functions) {
			if(it.m_name == name) {
				return optional_function{it};
			}
		}
		return stl::nullopt;
	}

	class_info::const_optional_function class_info::function(string_view_type name) const noexcept
	{
		for(const func_info_type &it : m_functions) {
			if(it.m_name == name) {
				return const_optional_function{it};
			}
		}
		return stl::nullopt;
	}

	string_type &class_info::name() noexcept
	{ return m_name; }
	const string_type &class_info::name() const noexcept
	{ return m_name; }

	stl::vector<func_info_type> &class_info::functions() noexcept
	{ return m_functions; }
	const stl::vector<func_info_type> &class_info::functions() const noexcept
	{ return m_functions; }

	stl::vector<member_variable_info> &class_info::variables() noexcept
	{ return m_variables; }
	const stl::vector<member_variable_info> &class_info::variables() const noexcept
	{ return m_variables; }
}