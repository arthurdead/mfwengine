#include <public/mfw/core/type_info.hpp>
#include <public/mfw/stl/float.hpp>

namespace mfw::core
{
	bool type_info::is_int8() const noexcept
	{
		return (
		#if MFW_COMPILER_FLAGGED(MSVC)
			is_relaxed<__int8>() ||
		#endif
			is_relaxed<char>() ||
			is_exact<bool>()
		#ifdef MFW_CPP_CHAR8_SUPPORTED
			|| is_exact<char8_t>()
		#endif
		);
	}

	bool type_info::is_int16() const noexcept
	{
		return (
		#if MFW_COMPILER_FLAGGED(MSVC)
			is_relaxed<__int16>() ||
			is_exact<__wchar_t>() ||
		#endif
			is_relaxed<short>() ||
			is_exact<char16_t>()
		#if MFW_WCHAR_SIZE == 16
			|| is_exact<wchar_t>()
		#endif
		);
	}

	bool type_info::is_int32() const noexcept
	{
		return (
		#if MFW_COMPILER_FLAGGED(MSVC)
			is_relaxed<__int32>() ||
		#endif
			is_relaxed<int>() ||
		#if MFW_LONG_SIZE == 32
			is_exact<long>() ||
		#endif
		#if MFW_WCHAR_SIZE == 32
			is_exact<wchar_t>() ||
		#endif
			is_exact<char32_t>()
		);
	}

	bool type_info::is_int64() const noexcept
	{
		return (
		#if MFW_COMPILER_FLAGGED(MSVC)
			is_relaxed<__int64>() ||
		#endif
		#if MFW_LONG_SIZE == 64
			is_exact<long>() ||
		#endif
			is_exact<long long>()
		);
	}

	bool type_info::is_float16() const noexcept
	{
	#ifdef MFW_FLOAT16_SUPPORTED
		return is_exact<_Float16>();
	#else
		return is_float32();
	#endif
	}

	bool type_info::is_float32() const noexcept
	{ return is_exact<float>(); }

	bool type_info::is_float64() const noexcept
	{ return is_exact<double>(); }

	bool type_info::is_float80() const noexcept
	{
		return (
		#if MFW_COMPILER_IS(GCC)
			is_exact<__float80>() ||
		#endif
			is_exact<long double>()
		);
	}

	bool type_info::is_float128() const noexcept
	{
	#ifdef MFW_FLOAT128_SUPPORTED
		return is_exact<_Float128>();
	#else
		return is_float80();
	#endif
	}

	bool type_info::valid() const noexcept
	{ return !m_name.empty() && m_stl_info; }

	stl::size_t type_info::size() const noexcept
	{ return m_size; }
	stl::size_t type_info::align() const noexcept
	{ return m_align; }
	const stl::type_info &type_info::stl_info() const noexcept
	{ return (m_stl_info ? *m_stl_info : stl::get_typeid<void>()); }

	bool type_info::is_exact(const type_info &info) const noexcept
	{ return stl_info() == info.stl_info(); }
	bool type_info::is_exact(const stl::type_info &info) const noexcept
	{ return stl_info() == info; }

	bool type_info::is_exact(string_view_type name) const noexcept
	{ return m_name == name; }

	bool type_info::is_void() const noexcept
	{ return is_exact<void>(); }
	bool type_info::is_primitive() const noexcept
	{ return !bool_cast(flags_ & flags::class_) || (is_void() || is_any_int() || is_any_float()); }
	bool type_info::is_class() const noexcept
	{ return bool_cast(flags_ & flags::class_) || !is_primitive(); }
	bool type_info::is_rvalue_ref() const noexcept
	{ return bool_cast(flags_ & flags::rvalue_ref); }
	bool type_info::is_const() const noexcept
	{ return bool_cast(flags_ & flags::const_); }
	bool type_info::is_lvalue_ref() const noexcept
	{ return bool_cast(flags_ & flags::lvalue_ref); }
	bool type_info::is_signed() const noexcept
	{ return bool_cast(flags_ & flags::signed_); }
	bool type_info::is_unsigned() const noexcept
	{ return bool_cast(flags_ & flags::unsigned_); }
	bool type_info::is_array() const noexcept
	{ return bool_cast(flags_ & flags::array_) || rank() > 0; }
	bool type_info::is_pointer() const noexcept
	{ return bool_cast(flags_ & flags::pointer_) || num_pointers() > 0; }
	stl::size_t type_info::rank() const noexcept
	{ return rank_; }
	stl::size_t type_info::extent() const noexcept
	{ return extent_; }
	stl::size_t type_info::num_pointers() const noexcept
	{ return pointers; }
	bool type_info::is_any_int() const noexcept
	{ return is_int8() || is_int16() || is_int32() || is_int64() || is_int128(); }
	bool type_info::is_any_float() const noexcept
	{ return is_float16() || is_float32() || is_float64() || is_float80() || is_float128(); }
	bool type_info::is_any_reference() const noexcept
	{ return is_rvalue_ref() || is_lvalue_ref(); }
	bool type_info::is_ptr_like() const noexcept
	{ return is_pointer() || is_array(); }
	const type_info::string_type &type_info::name() const noexcept
	{ return m_name; }

	void type_info::set_name() noexcept
	{ m_name = rttr::instance().clean_name(*m_stl_info); }

	void type_info::make_pointer() noexcept
	{
		m_flags |= flags::pointer_;
		m_size = sizeof(void *);
		m_align = alignof(void *);
		m_pointers += 1;
	}

	void type_info::clear() noexcept
	{
		m_flags = flags::none;
		m_size = 0;
		m_align = 0;
		m_stl_info = nullptr;
		m_name.clear();
		m_pointers = 0;
		m_extent = 0;
		m_rank = 0;
	}
}