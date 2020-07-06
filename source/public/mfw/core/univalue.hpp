#ifndef __MFW_PUBLIC_CORE_UNIVALUE_HPP
#define __MFW_PUBLIC_CORE_UNIVALUE_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/string_view.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/limits.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/float.hpp>
#include <public/mfw/core/core.hpp>

namespace mfw::core
{
	#define __MFW_UNIVALUE_OP(op) \
		univalue operator op (const univalue &) const noexcept; \
		univalue operator op (univalue &&) const noexcept;

	#define __MFW_UNIVALUE_OP_CMP(op) \
		bool operator op (const univalue &) const noexcept;

	#define __MFW_UNIVALUE_OP_EQUAL(op) \
		__MFW_UNIVALUE_OP(op) \
		univalue &operator op =(const univalue &) noexcept; \
		univalue &operator op =(univalue &&) noexcept;

	#define __MFW_UNIVALUE_OP_EQUAL_CMP(op) \
		__MFW_UNIVALUE_OP_CMP(op) \
		bool operator op =(const univalue &) noexcept;

	#define __MFW_UNIVALUE_OP_SCALAR(op, type) \
		type operator op (type value) const noexcept;

	#define __MFW_UNIVALUE_OP_COMPLEX(op, type) \
		type operator op (const type &value) const noexcept; \
		type operator op (type &&value) const noexcept;

	#define __MFW_UNIVALUE_OP_SCALAR_CMP(op, type) \
		bool operator op (type value) const noexcept;

	#define __MFW_UNIVALUE_OP_EQUAL_SCALAR(op, type) \
		__MFW_UNIVALUE_OP_SCALAR(op, type) \
		type operator op =(type value) noexcept;

	#define __MFW_UNIVALUE_OP_EQUAL_COMPLEX(op, type) \
		__MFW_UNIVALUE_OP_COMPLEX(op, type) \
		type operator op =(const type &value) noexcept; \
		type operator op =(type &&value) noexcept;

	#define __MFW_UNIVALUE_OP_EQUAL_SCALAR_CMP(op, type) \
		__MFW_UNIVALUE_OP_SCALAR_CMP(op, type) \
		bool operator op =(type value) const noexcept;

	#define __MFW_UNIVALUE_OP_EQUAL_EQUAL_SCALAR(op, type) \
		explicit univalue(type value) noexcept; \
		univalue &operator=(type value) noexcept; \
		bool operator==(type value) const noexcept; \
		bool operator!=(type value) const noexcept; \
		explicit operator type() const noexcept;

	#define __MFW_UNIVALUE_OP_EQUAL_EQUAL_COMPLEX(op, type) \
		explicit univalue(const type &value) noexcept; \
		explicit univalue(type &&value) noexcept; \
		univalue &operator=(const type &value) noexcept; \
		univalue &operator=(type &&value) noexcept; \
		bool operator==(const type &value) const noexcept; \
		bool operator!=(const type &value) const noexcept; \
		explicit operator const type &() const noexcept;

	class MFW_VISIBILITY_LOCAL univalue final
	{
	public:
		using float_type = stl::float80_t;
		using int_type = stl::uint64_t;
		using string_type = stl::osstring;

		univalue() noexcept = default;
		univalue &operator=(const univalue &other) noexcept = default;
		univalue(const univalue &other) noexcept = default;
		univalue &operator=(univalue &&other) noexcept = default;
		univalue(univalue &&other) noexcept = default;
		~univalue() noexcept = default;

		__MFW_UNIVALUE_OP_EQUAL(+)
		__MFW_UNIVALUE_OP_EQUAL(-)
		__MFW_UNIVALUE_OP_EQUAL(*)
		__MFW_UNIVALUE_OP_EQUAL(&)
		__MFW_UNIVALUE_OP_EQUAL(|)
		__MFW_UNIVALUE_OP_EQUAL(/)
		__MFW_UNIVALUE_OP_EQUAL(<<)
		__MFW_UNIVALUE_OP_EQUAL(>>)
		__MFW_UNIVALUE_OP_EQUAL(%)
		__MFW_UNIVALUE_OP_EQUAL(^)
		__MFW_UNIVALUE_OP_EQUAL_CMP(<)
		__MFW_UNIVALUE_OP_EQUAL_CMP(>)

		__MFW_UNIVALUE_OP_CMP(&&)
		__MFW_UNIVALUE_OP_CMP(||)
		__MFW_UNIVALUE_OP_CMP(~=)

		__MFW_UNIVALUE_OP_EQUAL_EQUAL_SCALAR(float_type)
		__MFW_UNIVALUE_OP_EQUAL_EQUAL_SCALAR(int_type)
		__MFW_UNIVALUE_OP_EQUAL_EQUAL_SCALAR(bool)
		__MFW_UNIVALUE_OP_EQUAL_EQUAL_COMPLEX(string_type)

		__MFW_UNIVALUE_OP_EQUAL_SCALAR(+, float_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(-, float_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(/, float_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(*, float_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR_CMP(<, float_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR_CMP(>, float_type)
		__MFW_UNIVALUE_OP_SCALAR_CMP(&&, float_type)
		__MFW_UNIVALUE_OP_SCALAR_CMP(||, float_type)

		__MFW_UNIVALUE_OP_EQUAL_SCALAR(+, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(-, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(*, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(/, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(<<, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(>>, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(|, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(&, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR_CMP(<, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR_CMP(>, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(^, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(%, int_type)
		__MFW_UNIVALUE_OP_SCALAR_CMP(&&, int_type)
		__MFW_UNIVALUE_OP_SCALAR_CMP(||, int_type)

		__MFW_UNIVALUE_OP_SCALAR_CMP(&&, bool)
		__MFW_UNIVALUE_OP_SCALAR_CMP(||, bool)

		__MFW_UNIVALUE_OP_EQUAL_COMPLEX(+, string_type)

		explicit univalue(const string_type &value) noexcept;
		explicit univalue(string_type &&value) noexcept;

		univalue operator-() const noexcept;
		univalue operator+() const noexcept;
		univalue &operator--(int) noexcept;
		univalue &operator--() noexcept;
		univalue &operator++(int) noexcept;
		univalue &operator++() noexcept;
		int_type operator~() const noexcept;
		bool operator!() const noexcept;
		const string_type *operator->() const noexcept;

		explicit operator const string_type &() const noexcept;

		const string_type &get_string() const noexcept;
		float_type get_float() const noexcept;
		int_type get_int() const noexcept;
		bool get_bool() const noexcept;

		bool is_bool() const noexcept;
		bool is_float() const noexcept;
		bool is_int() const noexcept;
		bool is_string() const noexcept;

		univalue &clear() noexcept;

		bool empty() const noexcept;

	private:
		void calculate_float() noexcept;
		void calculate_string() noexcept;

		string_type m_string{};
		float_type m_float{0.0f};
	};
}

MFW_VISIBILITY_LOCAL ::mfw::core::univalue operator""_uv(::mfw::stl::ullong_t n) noexcept;
MFW_VISIBILITY_LOCAL ::mfw::core::univalue operator""_uv(::mfw::stl::ldouble_t f) noexcept;
MFW_VISIBILITY_LOCAL ::mfw::core::univalue operator""_uv(const ::mfw::core::univalue::string_type::value_type *ptr, ::mfw::stl::size_t len) noexcept;

#endif