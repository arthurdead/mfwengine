#ifndef __MFW_PUBLIC_CORE_UNIVALUE_H
#define __MFW_PUBLIC_CORE_UNIVALUE_H

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/core/core.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/limits.hpp>
#include <public/mfw/core/rttr_interface.hpp>

namespace mfw
{
	#define __MFW_UNIVALUE_OPERATORS_UNIVALUE() \
		univalue &operator+=(const univalue &value) { \
			if(value.is_string()) { \
				set(get_string() + value.get_string()); \
			} else { \
				set(get_float() + value.get_float()); \
			} \
			return *this; \
		} \
		univalue operator+(const univalue &value) const { \
			if(value.is_string()) { \
				return univalue{get_string() + value.get_string()}; \
			} else { \
				return univalue{get_float() + value.get_float()}; \
			} \
		} \
		univalue &operator/=(const univalue &value) { set(get_float() / value.get_float()); return *this; } \
		univalue operator/(const univalue &value) const { return univalue{get_float() / value.get_float()}; } \
		univalue &operator-=(const univalue &value) { set(get_float() - value.get_float()); return *this; } \
		univalue operator-(const univalue &value) const { return univalue{get_float() - value.get_float()}; } \
		univalue &operator*=(const univalue &value) { set(get_float() * value.get_float()); return *this; } \
		univalue operator*(const univalue &value) const { return univalue{get_float() * value.get_float()}; } \
		bool operator&&(const univalue &value) const { return (get_bool() && value.get_bool()); } \
		bool operator||(const univalue &value) const { return (get_bool() || value.get_bool()); } \
		bool operator>(const univalue &value) const { return (get_float() > value.get_float()); } \
		bool operator>=(const univalue &value) const { return (get_float() >= value.get_float()); } \
		bool operator<(const univalue &value) const { return (get_float() < value.get_float()); } \
		bool operator<=(const univalue &value) const { return (get_float() <= value.get_float()); } \
		bool operator==(const univalue &value) const { \
			if(value.is_string()) { \
				return (get_string() == value.get_string()); \
			} else { \
				return (get_float() == value.get_float()); \
			} \
		} \
		bool operator!=(const univalue &value) const { return !operator==(value); } \
		univalue operator^(const univalue &value) const { return univalue{get_int() ^ value.get_int()}; } \
		univalue operator|(const univalue &value) const { return univalue{get_int() | value.get_int()}; } \
		univalue operator&(const univalue &value) const { return univalue{get_int() & value.get_int()}; } \
		univalue operator<<(const univalue &value) const { return univalue{get_int() << value.get_int()}; } \
		univalue operator>>(const univalue &value) const { return univalue{get_int() >> value.get_int()}; } \
		univalue operator%(const univalue &value) const { return univalue{get_int() % value.get_int()}; }

	#define __MFW_UNIVALUE_OPERATORS_STRING(realtype, faketype) \
		explicit univalue(const faketype &value) { set(value); } \
		void set(const faketype &value) { set_string(as_string<realtype>(value)); } \
		univalue &operator=(const faketype &value) { set(value); return *this; } \
		bool operator==(const faketype &value) const { return (get_string() == as_string<realtype>(value)); } \
		bool operator!=(const faketype &value) const { return !operator==(value); } \
		faketype operator+(const faketype &value) const { return (as_string<faketype>(get_string()) + value); } \
		univalue &operator+=(const faketype &value) { set(get_string() + as_string<realtype>(value)); return *this; }

	#define __MFW_UNIVALUE_OPERATORS_STRINGVIEW(realtype, faketype, fakestringtype) \
		explicit univalue(const faketype &value) { set(value); } \
		void set(const faketype &value) { set_string(as_string<realtype>(value)); } \
		univalue &operator=(const faketype &value) { set(value); return *this; } \
		bool operator==(const faketype &value) const { return (get_string() == as_string<realtype>(value)); } \
		bool operator!=(const faketype &value) const { return !operator==(value); } \
		fakestringtype operator+(const faketype &value) const { return (as_string<fakestringtype>(get_string()) + value.data()); } \
		univalue &operator+=(const faketype &value) { set(get_string() + as_string<realtype>(value)); return *this; }

	#define __MFW_UNIVALUE_OPERATORS_CSTRING(realtype, faketype, fakestringtype) \
		explicit univalue(const faketype *value) { set(value); } \
		void set(const faketype *value) { set_string(as_string<realtype>(fakestringtype##_view{value})); } \
		univalue &operator=(const faketype *value) { set(value); return *this; } \
		bool operator==(const faketype *value) const { return (get_string() == as_string<realtype>(fakestringtype##_view{value})); } \
		bool operator!=(const faketype *value) const { return !operator==(value); } \
		fakestringtype operator+(const faketype *value) const { return (as_string<fakestringtype>(get_string()) + value); } \
		univalue &operator+=(const faketype *value) { set(get_string() + as_string<realtype>(fakestringtype##_view{value})); return *this; }

	#define __MFW_UNIVALUE_OPERATORS_ARRSTRING(realtype, faketype, fakestringtype) \
		template <size_t size_> \
		explicit univalue(const faketype (&value)[size_]) { set(value); } \
		template <size_t size_> \
		void set(const faketype (&value)[size_]) { set_string(as_string<realtype>(fakestringtype##_view{value})); } \
		template <size_t size_> \
		univalue &operator=(const faketype (&value)[size_]) { set(value); return *this; } \
		template <size_t size_> \
		bool operator==(const faketype (&value)[size_]) const { return (get_string() == as_string<realtype>(fakestringtype##_view{value})); } \
		template <size_t size_> \
		bool operator!=(const faketype (&value)[size_]) const { return !operator==(value); } \
		template <size_t size_> \
		fakestringtype operator+(const faketype (&value)[size_]) const { return (as_string<fakestringtype>(get_string()) + value); } \
		template <size_t size_> \
		univalue &operator+=(const faketype (&value)[size_]) { set(get_string() + as_string<realtype>(fakestringtype##_view{value})); return *this; }

	#define __MFW_UNIVALUE_OPERATORS_CHAR(realtype, faketype, fakestringtype) \
		explicit univalue(faketype value) { set(static_cast<realtype>(value)); } \
		void set(faketype value) { set_char(static_cast<realtype>(value)); } \
		univalue &operator=(faketype value) { set(static_cast<realtype>(value)); return *this; } \
		fakestringtype operator+(faketype value) const { return (as_string<fakestringtype>(get_string()) + value); } \
		univalue &operator+=(faketype value) { set(get_string() + static_cast<realtype>(value)); return *this; }

	#define __MFW_UNIVALUE_OPERATORS_FLOAT(realtype, faketype) \
		explicit univalue(faketype value) { set(static_cast<realtype>(value)); } \
		void set(faketype value) { set_float(static_cast<realtype>(value)); } \
		univalue &operator=(faketype value) { set(static_cast<realtype>(value)); return *this; } \
		bool operator==(faketype value) const { return (static_cast<faketype>(get_float()) == value); } \
		bool operator&&(faketype value) const { return (static_cast<faketype>(get_float()) && value); } \
		bool operator||(faketype value) const { return (static_cast<faketype>(get_float()) || value); } \
		bool operator!=(faketype value) const { return (static_cast<faketype>(get_float()) != value); } \
		bool operator>(faketype value) const { return (static_cast<faketype>(get_float()) > value); } \
		bool operator>=(faketype value) const { return (static_cast<faketype>(get_float()) >= value); } \
		bool operator<(faketype value) const { return (static_cast<faketype>(get_float()) < value); } \
		bool operator<=(faketype value) const { return (static_cast<faketype>(get_float()) <= value); } \
		faketype operator+(faketype value) const { return (static_cast<faketype>(get_float()) + value); } \
		univalue &operator+=(faketype value) { set(get_float() + static_cast<realtype>(value)); return *this; } \
		faketype operator-(faketype value) const { return (static_cast<faketype>(get_float()) - value); } \
		univalue &operator-=(faketype value) { set(get_float() - static_cast<realtype>(value)); return *this; } \
		faketype operator*(faketype value) const { return (static_cast<faketype>(get_float()) * value); } \
		univalue &operator*=(faketype value) { set(get_float() * static_cast<realtype>(value)); return *this; } \
		faketype operator/(faketype value) const { return (static_cast<faketype>(get_float()) / value); } \
		univalue &operator/=(faketype value) { set(get_float() / static_cast<realtype>(value)); return *this; } \
		explicit operator faketype() const { return static_cast<faketype>(get_float()); }

	#define __MFW_UNIVALUE_OPERATORS_INT(realtype, faketype) \
		explicit univalue(faketype value) { set(static_cast<realtype>(value)); } \
		void set(faketype value) { set_int(static_cast<realtype>(value)); } \
		univalue &operator=(faketype value) { set(static_cast<realtype>(value)); return *this; } \
		bool operator==(faketype value) const { return (static_cast<faketype>(get_int()) == value); } \
		bool operator&&(faketype value) const { return (static_cast<faketype>(get_int()) && value); } \
		bool operator||(faketype value) const { return (static_cast<faketype>(get_int()) || value); } \
		bool operator!=(faketype value) const { return (static_cast<faketype>(get_int()) != value); } \
		bool operator>(faketype value) const { return (static_cast<faketype>(get_int()) > value); } \
		bool operator>=(faketype value) const { return (static_cast<faketype>(get_int()) >= value); } \
		bool operator<(faketype value) const { return (static_cast<faketype>(get_int()) < value); } \
		bool operator<=(faketype value) const { return (static_cast<faketype>(get_int()) <= value); } \
		faketype operator+(faketype value) const { return (static_cast<faketype>(get_int()) + value); } \
		univalue &operator+=(faketype value) { set(get_int() + static_cast<realtype>(value)); return *this; } \
		faketype operator-(faketype value) const { return (static_cast<faketype>(get_int()) - value); } \
		univalue &operator-=(faketype value) { set(get_int() - static_cast<realtype>(value)); return *this; } \
		faketype operator*(faketype value) const { return (static_cast<faketype>(get_int()) * value); } \
		univalue &operator*=(faketype value) { set(get_int() * static_cast<realtype>(value)); return *this; } \
		faketype operator/(faketype value) const { return (static_cast<faketype>(get_int()) / value); } \
		univalue &operator/=(faketype value) { set(get_int() / static_cast<realtype>(value)); return *this; } \
		faketype operator|(faketype value) const { return (static_cast<faketype>(get_int()) | value); } \
		univalue &operator|=(faketype value) { set(get_int() | static_cast<realtype>(value)); return *this; } \
		faketype operator^(faketype value) const { return (static_cast<faketype>(get_int()) ^ value); } \
		univalue &operator^=(faketype value) { set(get_int() ^ static_cast<realtype>(value)); return *this; } \
		faketype operator%(faketype value) const { return (static_cast<faketype>(get_int()) % value); } \
		univalue &operator%=(faketype value) { set(get_int() % static_cast<realtype>(value)); return *this; } \
		faketype operator<<(faketype value) const { return (static_cast<faketype>(get_int()) << value); } \
		univalue &operator<<=(faketype value) { set(get_int() << static_cast<realtype>(value)); return *this; } \
		faketype operator>>(faketype value) const { return (static_cast<faketype>(get_int()) >> value); } \
		univalue &operator>>=(faketype value) { set(get_int() >> static_cast<realtype>(value)); return *this; } \
		faketype operator&(faketype value) const { return (static_cast<faketype>(get_int()) & value); } \
		univalue &operator&=(faketype value) { set(get_int() & static_cast<realtype>(value)); return *this; } \
		explicit operator faketype() const { return static_cast<faketype>(get_int()); }

	#define __MFW_UNIVALUE_OPERATORS_BOOL() \
		explicit univalue(bool value) { set(value); } \
		void set(bool value) { set_bool(value); } \
		univalue &operator=(bool value) { set(value); return *this; } \
		bool operator==(bool value) const { return (get_bool() == value); } \
		bool operator&&(bool value) const { return (get_bool() && value); } \
		bool operator||(bool value) const { return (get_bool() || value); } \
		bool operator!=(bool value) const { return (get_bool() != value); } \
		explicit operator bool() const { return get_bool(); }

	namespace core
	{
		class univalue
		{
		public:
			using float_type = float80_t;
			using int_type = uint64_t;
			using char_type = ucchar_t;
			using string_type = ucstring;
			using string_view_type = ucstring_view;

			univalue &operator=(const univalue &other) = default;
			univalue(const univalue &other) = default;
			univalue &operator=(univalue &&other) = default;
			univalue(univalue &&other) = default;

			void set(const univalue &other) { *this = other; }

			template <typename T, size_t s>
			univalue(T (&ptr)[s]) = delete;
			template <typename T, size_t s>
			void set(T (&ptr)[s]) = delete;
			template <typename T>
			univalue(T *ptr) = delete;
			template <typename T>
			void set(T *ptr) = delete;

			univalue() = default;
			~univalue() = default;

			MFW_CORE_API const string_type & MFW_CORE_CALL get_string() const;
			MFW_CORE_API string_view_type MFW_CORE_CALL get_string_view() const;
			MFW_CORE_API const char_type * MFW_CORE_CALL c_str() const;
			MFW_CORE_API float_type MFW_CORE_CALL get_float() const;
			MFW_CORE_API int_type MFW_CORE_CALL get_int() const;
			MFW_CORE_API bool MFW_CORE_CALL get_bool() const;
			const type_holder &get_var() const { return var; }

			MFW_CORE_API bool MFW_CORE_CALL is_bool() const;
			MFW_CORE_API bool MFW_CORE_CALL is_float() const;
			MFW_CORE_API bool MFW_CORE_CALL is_int() const;
			bool is_string() const { return (get_float() == numeric_limits<float_type>::max()); }
			bool is_var() const { return (!is_string() && !is_bool()); }

			explicit operator const string_type &() const { return get_string(); }
			explicit operator string_view_type() const { return get_string_view(); }
			//explicit operator const char_type *() const { return c_str(); }

			const string_type *operator->() const { return &get_string(); }

			__MFW_UNIVALUE_OPERATORS_STRING(string_type, ucstring)
			__MFW_UNIVALUE_OPERATORS_STRINGVIEW(string_type, ucstring_view, ucstring)
			__MFW_UNIVALUE_OPERATORS_CSTRING(string_type, ucchar_t, ucstring)
			__MFW_UNIVALUE_OPERATORS_ARRSTRING(string_type, ucchar_t, ucstring)
			__MFW_UNIVALUE_OPERATORS_CHAR(char_type, ucchar_t, ucstring)

			__MFW_UNIVALUE_OPERATORS_FLOAT(float_type, float32_t)
			__MFW_UNIVALUE_OPERATORS_FLOAT(float_type, float64_t)
			__MFW_UNIVALUE_OPERATORS_FLOAT(float_type, float80_t)

		#if MFW_COMPILER == MFW_COMPILER_MSVC
			MFW_WARNING_PUSH()
			MFW_WARNING_DISABLE(4365)
		#endif
			//__MFW_UNIVALUE_OPERATORS_INT(int_type, int8_t)
			//__MFW_UNIVALUE_OPERATORS_INT(int_type, uint8_t)
			__MFW_UNIVALUE_OPERATORS_INT(int_type, int16_t)
			__MFW_UNIVALUE_OPERATORS_INT(int_type, uint16_t)
			__MFW_UNIVALUE_OPERATORS_INT(int_type, int32_t)
			__MFW_UNIVALUE_OPERATORS_INT(int_type, uint32_t)
			__MFW_UNIVALUE_OPERATORS_INT(int_type, int64_t)
			__MFW_UNIVALUE_OPERATORS_INT(int_type, uint64_t)
		#if MFW_COMPILER == MFW_COMPILER_MSVC
			MFW_WARNING_POP()
		#endif
			int_type operator~() const { return ~get_int(); }

			__MFW_UNIVALUE_OPERATORS_BOOL()
			bool operator!() const { return !get_bool(); }

			__MFW_UNIVALUE_OPERATORS_UNIVALUE()
			univalue operator-() const { return univalue{-get_float()}; }
			univalue operator+() const { return univalue{+get_float()}; }
			univalue &operator--(int) { set(get_float() - 1.0f); return *this; }
			univalue &operator--() { set(get_float() - 1.0f); return *this; }
			univalue &operator++(int) { set(get_float() + 1.0f); return *this; }
			univalue &operator++() { set(get_float() + 1.0f); return *this; }

			template <typename T>
			void set(const T &value) { set_var(value); }

			void set(const type_holder &value) { set_var(value); }

			MFW_CORE_API void MFW_CORE_CALL clear();

			bool empty() const { return string_.empty(); }

			void set_strview(const string_view_type &value) { set(string_type{value}); }
			template <size_t size_>
			void set_arrstr(const char_type (&value)[size_]) { set(string_type{value}); }
			void set_char(char_type value) { set(string_type{value}); }
			void set_cstr(const char_type *value) { set(string_type{value}); }

			MFW_CORE_API void MFW_CORE_CALL set_string(const string_type &value);
			MFW_CORE_API void MFW_CORE_CALL set_float(float_type value);
			MFW_CORE_API void MFW_CORE_CALL set_int(int_type value);
			MFW_CORE_API void MFW_CORE_CALL set_bool(bool value);
			MFW_CORE_API void MFW_CORE_CALL set_var(const type_holder &value);

			template <typename T>
			void set_var(const T &value);

		private:
			string_type string_{};
			float_type float_{0.0f};
			type_holder var{};
		};

		template <typename T>
		void univalue::set_var(const T &value)
		{
			type_holder tmp{};
			tmp.deduce(value);
			set_var(tmp);
		}

		namespace literals
		{
			inline univalue operator""_uv(ullong_t n) { return univalue{static_cast<univalue::int_type>(n)}; }
			inline univalue operator""_uv(ldouble_t f) { return univalue{static_cast<univalue::float_type>(f)}; }

			inline univalue operator""_uv(const univalue::char_type *ptr, size_t len)
			{
				univalue::string_view_type str{ptr, len};
				return univalue{move(str)};
			}
		}
	}

	namespace stl
	{
		#define __MFW_STL_C_STR \
			overload_cast_static(const char *, , stl::c_str, const ucstring &)

		/*inline const char *c_str(const univalue &src)
		{
			const ucstring &str{src.get_string()};
			return __MFW_STL_C_STR(str);
		}*/

		inline void to_string(const core::univalue &src, ucstring &dst) { dst = src.get_string(); }
		inline void to_string(const core::univalue &src, pstring &dst)
		{
			const ucstring &str{src.get_string()};
			const upnchar_t *begin{__MFW_STL_C_STR(str)};
			const upnchar_t *end{begin+src->length()};
			dst.assign(begin, end);
		}
	}
}


#endif