#include <public/mfw/core/univalue.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/string.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <cmath>
#else
	#error
#endif

namespace mfw::core
{
	MFW_CORE_API void MFW_CORE_CALL univalue::clear()
	{
		string_.clear();
		float_ = 0.0f;
		var.clear();
	}

	MFW_CORE_API void MFW_CORE_CALL univalue::set_string(const string_type &value)
	{
		string_ = value;
		if(string_ == u8"true"_sv) {
			float_ = 1.0f;
		} else if(string_ == u8"false"_sv) {
			float_ = 0.0f;
		} else {
			to_float(string_, float_);
		}
		var.clear();
	}

	MFW_CORE_API void MFW_CORE_CALL univalue::set_bool(bool value)
	{
		float_ = (value ? 1.0f : 0.0f);
		to_string(value, string_);
		var.clear();
	}

	MFW_CORE_API void MFW_CORE_CALL univalue::set_int(int_type value)
	{
		float_ = static_cast<float_type>(value);
		to_string(value, string_);
		var.clear();
	}

	MFW_CORE_API void MFW_CORE_CALL univalue::set_float(float_type value)
	{
		float_ = value;
		to_string(value, string_);
		var.clear();
	}

	MFW_CORE_API bool MFW_CORE_CALL univalue::is_bool() const
	{
		const ucstring &str{get_string()};
		return ((str == u8"true"_sv) || (str == u8"false"_sv));
	}

	MFW_CORE_API bool MFW_CORE_CALL univalue::is_float() const
	{
		float_type f{get_float()};
		return ((f != numeric_limits<float_type>::max()) && (::MFW_STD_NAMESPACE::ceil(f) != f));
	}

	MFW_CORE_API bool MFW_CORE_CALL univalue::is_int() const
	{
		float_type f{get_float()};
		return ((f != numeric_limits<float_type>::max()) && (::MFW_STD_NAMESPACE::ceil(f) == f));
	}

	MFW_CORE_API const univalue::string_type & MFW_CORE_CALL univalue::get_string() const
	{
		return string_;
	}

	MFW_CORE_API univalue::float_type MFW_CORE_CALL univalue::get_float() const
	{
		return float_;
	}

	MFW_CORE_API univalue::string_view_type MFW_CORE_CALL univalue::get_string_view() const
	{
		return string_view_type{get_string()};
	}

	MFW_CORE_API const univalue::char_type * MFW_CORE_CALL univalue::c_str() const
	{
		return get_string().c_str();
	}

	MFW_CORE_API univalue::int_type MFW_CORE_CALL univalue::get_int() const
	{
		return static_cast<int_type>(get_float());
	}

	MFW_CORE_API bool MFW_CORE_CALL univalue::get_bool() const
	{
		if(get_float() == numeric_limits<float_type>::max()) {
			return false;
		}
		return bool_cast(get_int());
	}

	MFW_CORE_API void MFW_CORE_CALL univalue::set_var(const type_holder &value)
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
	}
}