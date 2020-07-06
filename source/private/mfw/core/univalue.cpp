#include <public/mfw/core/univalue.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/string.hpp>

#if MFW_STDC_IS(DEFAULT)
	#include <cmath>
#else
	#error
#endif

namespace mfw::core
{
	MFW_CORE_API void MFW_CORE_CALL univalue::calculate_float() noexcept
	{
		bool was_bool{false};

		using char_type = string_type::value_type;

		stl::size_t len{m_string.length()};
		if(len == 4) {
			if(m_string[0] == static_cast<char_type>('t') &&
				m_string[1] == static_cast<char_type>('r') &&
				m_string[2] == static_cast<char_type>('u') &&
				m_string[3] == static_cast<char_type>('e')) {
				m_float = 1.0f;
				was_bool = true;
			}
		} else if(len == 5) {
			if(m_string[0] == static_cast<char_type>('f') &&
				m_string[1] == static_cast<char_type>('a') &&
				m_string[2] == static_cast<char_type>('l') &&
				m_string[3] == static_cast<char_type>('s') &&
				m_string[4] == static_cast<char_type>('e')) {
				m_float = 0.0f;
				was_bool = true;
			}
		}

		if(!was_bool) {
			stl::to_float(m_string, m_float);
		}
	}

	MFW_CORE_API UniValue & MFW_CORE_CALL UniValue::setString(const string_type &value) noexcept
	{
		m_string = value;

		bool was_bool{false};

		size_t len{m_string.length()};
		if(len == 4) {
			if(m_string[0] == static_cast<char_type>('t') &&
				m_string[1] == static_cast<char_type>('r') &&
				m_string[2] == static_cast<char_type>('u') &&
				m_string[3] == static_cast<char_type>('e')) {
				m_float = 1.0f;
				was_bool = true;
			}
		} else if(len == 5) {
			if(m_string[0] == static_cast<char_type>('f') &&
				m_string[1] == static_cast<char_type>('a') &&
				m_string[2] == static_cast<char_type>('l') &&
				m_string[3] == static_cast<char_type>('s') &&
				m_string[4] == static_cast<char_type>('e')) {
				m_float = 0.0f;
				was_bool = true;
			}
		}

		if(!was_bool) {
			to_float(m_string, m_float);
		}

		//var.clear();
		return *this;
	}

	MFW_CORE_API UniValue & MFW_CORE_CALL UniValue::setBool(bool value) noexcept
	{
		m_float = (value ? 1.0f : 0.0f);
		stl::to_string(value, m_string);
		//var.clear();
		return *this;
	}

	MFW_CORE_API UniValue & MFW_CORE_CALL UniValue::setInt(int_type value) noexcept
	{
		m_float = static_cast<float_type>(value);
		stl::to_string(value, m_string);
		//var.clear();
		return *this;
	}

	MFW_CORE_API UniValue & MFW_CORE_CALL UniValue::setFloat(float_type value) noexcept
	{
		m_float = value;
		stl::to_string(value, m_string);
		//var.clear();
		return *this;
	}

	MFW_CORE_API bool MFW_CORE_CALL UniValue::isBool() const noexcept
	{
		size_t len{m_string.length()};
		if(len == 4) {
			if(m_string[0] == static_cast<char_type>('t') &&
				m_string[1] == static_cast<char_type>('r') &&
				m_string[2] == static_cast<char_type>('u') &&
				m_string[3] == static_cast<char_type>('e')) {
				return true;
			}
		} else if(len == 5) {
			if(m_string[0] == static_cast<char_type>('f') &&
				m_string[1] == static_cast<char_type>('a') &&
				m_string[2] == static_cast<char_type>('l') &&
				m_string[3] == static_cast<char_type>('s') &&
				m_string[4] == static_cast<char_type>('e')) {
				return true;
			}
		}
		return false;
	}

	MFW_CORE_API bool MFW_CORE_CALL UniValue::isFloat() const noexcept
	{
		return ((m_float != stl::numeric_limits<float_type>::max()) && (::MFW_STD_NAMESPACE::ceil(m_float) != m_float));
	}

	MFW_CORE_API bool MFW_CORE_CALL UniValue::isInt() const noexcept
	{
		return ((m_float != stl::numeric_limits<float_type>::max()) && (::MFW_STD_NAMESPACE::ceil(m_float) == m_float));
	}

	/*MFW_CORE_API void MFW_CORE_CALL UniValue::set_var(const type_holder &value)
	{
		const type_info &info{value.info()};
		if(info.is_any_int()) {
			set_int(value.convert<int_type>());
		} else if(info.is_any_float()) {
			set_float(value.convert<float_type>());
		} else if(info.is_exact<bool>()) {
			set_bool(value.convert<bool>());
		} else if(info.is_exact<ucstring>()) {
			set_string(value.get_var<ucstring>());
		} else if(info.is_exact<univalue>()) {
			*this = value.get_var<univalue>();
		} else {
			var = value;
			float_ = numeric_limits<float_type>::max();
			string_.clear();
		}
	}*/
}