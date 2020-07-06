#include <public/mfw/core/univalue.hpp>

#if MFW_STDC_IS(DEFAULT)
	#include <cmath>
#else
	#error
#endif

namespace mfw::core
{
	#define __MFW_UNIVALUE_OP_UNARY_IMPL(op) \
		univalue univalue::operator op (const univalue &value) const noexcept { \
			univalue tmp{*this}; \
			tmp op = value; \
			return tmp; \
		} \
		univalue univalue::operator op (univalue &&value) const noexcept { \
			univalue tmp{*this}; \
			tmp op = value; \
			return tmp; \
		}

	#define __MFW_UNIVALUE_OP_CMP_IMPL(op, var) \
		bool univalue::operator op (const univalue &value) const noexcept \
		{ return (var op value.var); }

	#define __MFW_UNIVALUE_OP_EQUAL_CMP_IMPL(op, var) \
		__MFW_UNIVALUE_OP_CMP_IMPL(op, var) \
		bool univalue::operator op =(const univalue &value) const noexcept \
		{ return (var op = value.var); }

	#define __MFW_UNIVALUE_OP_EQUAL(op, var, calc) \
		univalue &univalue::operator op =(const univalue &value) noexcept { \
			var op = value.var; \
			calc(); \
			return *this; \
		} \
		univalue &univalue::operator op =(univalue &&value) noexcept { \
			var op = value.var; \
			calc(); \
			return *this; \
		}

	#define __MFW_UNIVALUE_OP_EQUAL_INT(op) \
		univalue &univalue::operator op =(const univalue &value) noexcept { \
			m_float = static_cast<float_type>(static_cast<int_type>(m_float) op static_cast<int_type>(value.m_float)); \
			calculate_string(); \
			return *this; \
		} \
		univalue &univalue::operator op =(univalue &&value) noexcept { \
			m_float = static_cast<float_type>(static_cast<int_type>(m_float) op static_cast<int_type>(value.m_float)); \
			calculate_string(); \
			return *this; \
		}

	__MFW_UNIVALUE_OP_UNARY_IMPL(+)
	__MFW_UNIVALUE_OP_UNARY_IMPL(-)
	__MFW_UNIVALUE_OP_UNARY_IMPL(/)
	__MFW_UNIVALUE_OP_UNARY_IMPL(*)
	__MFW_UNIVALUE_OP_UNARY_IMPL(^)
	__MFW_UNIVALUE_OP_UNARY_IMPL(|)
	__MFW_UNIVALUE_OP_UNARY_IMPL(&)
	__MFW_UNIVALUE_OP_UNARY_IMPL(<<)
	__MFW_UNIVALUE_OP_UNARY_IMPL(>>)
	__MFW_UNIVALUE_OP_UNARY_IMPL(%)

	__MFW_UNIVALUE_OP_EQUAL_CMP_IMPL(<, m_float)
	__MFW_UNIVALUE_OP_EQUAL_CMP_IMPL(>, m_float)

	__MFW_UNIVALUE_OP_CMP_IMPL(&&, get_bool())
	__MFW_UNIVALUE_OP_CMP_IMPL(||, get_bool())

	__MFW_UNIVALUE_OP_EQUAL(/, m_float, calculate_string)
	__MFW_UNIVALUE_OP_EQUAL(*, m_float, calculate_string)
	__MFW_UNIVALUE_OP_EQUAL(-, m_float, calculate_string)

	__MFW_UNIVALUE_OP_EQUAL_INT(^)
	__MFW_UNIVALUE_OP_EQUAL_INT(|)
	__MFW_UNIVALUE_OP_EQUAL_INT(&)
	__MFW_UNIVALUE_OP_EQUAL_INT(<<)
	__MFW_UNIVALUE_OP_EQUAL_INT(>>)
	__MFW_UNIVALUE_OP_EQUAL_INT(%)
	__MFW_UNIVALUE_OP_EQUAL_INT(~)

	univalue &univalue::operator+=(const univalue &value) noexcept {
		if(value.is_string()) {
			m_string.append(value.m_string);
			calculate_float();
		} else {
			m_float += value.m_float;
			calculate_string();
		}
		return *this;
	}

	univalue &univalue::operator+=(univalue &&value) noexcept {
		if(value.is_string()) {
			m_string.append(value.m_string);
			calculate_float();
		} else {
			m_float += value.m_float;
			calculate_string();
		}
		return *this;
	}

	void univalue::calculate_float() noexcept
	{ stl::to_string(m_float, m_string); }

	void univalue::calculate_string() noexcept
	{ stl::to_float(m_string, m_float); }

	bool univalue::empty() const noexcept {
		return (m_string.empty() && (m_float == 0.0f || m_float == stl::numeric_limits<float_type>::max()));
	}

	univalue &univalue::clear() noexcept {
		m_string.clear();
		m_float = 0.0f;
		return *this;
	}

	univalue &univalue::operator++() noexcept {
		m_float++;
		calculate_string();
		return *this;
	}

	univalue &univalue::operator++(int) noexcept {
		++m_float;
		calculate_string();
		return *this;
	}

	univalue &univalue::operator--() noexcept {
		m_float--;
		calculate_string();
		return *this;
	}

	univalue &univalue::operator--(int) noexcept {
		--m_float;
		calculate_string();
		return *this;
	}

	univalue univalue::operator+() const noexcept {
		univalue tmp{};
		tmp.m_float = +m_float;
		tmp.calculate_string();
		return tmp;
	}

	univalue univalue::operator-() const noexcept {
		univalue tmp{};
		tmp.m_float = -m_float;
		tmp.calculate_string();
		return tmp;
	}

	bool univalue::operator!() const noexcept
	{ return !static_cast<bool>(m_float); }

	univalue::int_type univalue::operator~() const noexcept
	{ return ~static_cast<int_type>(m_float); }

	explicit univalue::operator const string_type &() const noexcept
	{ return m_string; }

	explicit univalue::operator float_type() const noexcept
	{ return m_float; }

	explicit univalue::operator int_type() const noexcept
	{ return static_cast<int_type>(m_float); }

	explicit univalue::operator bool() const noexcept
	{ return static_cast<bool>(m_float); }

	const string_type *univalue::operator->() const noexcept
	{ return &m_string; }

	bool univalue::is_string() const noexcept
	{ return (m_float == stl::numeric_limits<float_type>::max()); }

	const string_type &univalue::get_string() const noexcept
	{ return m_string; }

	float_type univalue::get_float() const noexcept
	{ return m_float; }

	int_type univalue::get_int() const noexcept
	{ return static_cast<int_type>(m_float); }

	bool univalue::get_bool() const noexcept {
		if(m_float == stl::numeric_limits<float_type>::max()) {
			return false;
		} else {
			return static_cast<bool>(m_float);
		}
	}

	explicit univalue::univalue(const string_type &value) noexcept
	{ operator=(value); }
	explicit univalue::univalue(string_type &&value) noexcept
	{ operator=(value); }
	explicit univalue::univalue(float_type value) noexcept
	{ operator=(value); }
	explicit univalue::univalue(int_type value) noexcept
	{ operator=(value); }
	explicit univalue::univalue(bool value) noexcept
	{ operator=(value); }

	explicit univalue &univalue::operator=(const string_type &value) noexcept {
		m_string = value;
		calculate_float();
		return *this;
	}

	explicit univalue &univalue::operator=(string_type &&value) noexcept {
		m_string = value;
		calculate_float();
		return *this;
	}

	explicit univalue &univalue::operator=(float_type value) noexcept {
		m_float = value;
		calculate_string();
		return *this;
	}

	explicit univalue &univalue::operator=(int_type value) noexcept {
		m_float = static_cast<float_type>(value);
		calculate_string();
		return *this;
	}

	explicit univalue &univalue::operator=(bool value) noexcept {
		m_float = static_cast<float_type>(value);
		calculate_string();
		return *this;
	}

	bool univalue::is_bool() const noexcept
	{
		using char_type = string_type::value_type;

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

	bool univalue::is_float() const noexcept {
		return ((m_float != stl::numeric_limits<float_type>::max()) && (::MFW_STD_NAMESPACE::ceil(m_float) != m_float));
	}

	bool univalue::is_int() const noexcept {
		return ((m_float != stl::numeric_limits<float_type>::max()) && (::MFW_STD_NAMESPACE::ceil(m_float) == m_float));
	}
}