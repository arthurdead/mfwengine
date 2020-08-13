#ifndef MFW_PUBLIC_CORE_ENVIRONMENT_HPP
#define MFW_PUBLIC_CORE_ENVIRONMENT_HPP

#pragma once

#include <public/mfw/core/core.hpp>

namespace mfw::core
{
	class environment_var
	{
	public:
	#if MFW_OS == MFW_OS_LINUX
		static inline constexpr ucchar_t default_sep{u8':'};
	#elif MFW_OS == MFW_OS_WINDOWS
		static inline constexpr ucchar_t default_sep{u8';'};
	#else
		#error
	#endif
	
		environment_var() = default;
		environment_var(ucstring_view name_, ucchar_t sep_ = default_sep) { reset(name_, sep_); }
		~environment_var() = default;

		MFW_CORE_API void MFW_CORE_CALL reset(ucstring_view name_, ucchar_t sep_ = default_sep);
		void reset() { reset(name, sep); }

		void set(ucstring_view value = {}) { values_.resize(1); values_[0] = value; }
		void unset() { values_.clear(); }

		bool is_set() const { return !values_.empty(); }

		void append(ucstring_view value) { values_.emplace_back(value); }
		MFW_CORE_API void MFW_CORE_CALL remove(ucstring_view value);
		void clear() { values_.resize(1); values_[0].clear(); }

		using values_vec_t = vector<ucstring>;

		size_t size() const { return values_.size(); }
		bool empty() const { return values_.empty(); }

		values_vec_t &values() { return values_; }
		ucstring &value() { return values_[0]; }

		ucstring &operator[](size_t i) { return values_[i]; }
		const ucstring &operator[](size_t i) const { return values_[i]; }

		environment_var &operator+=(ucstring_view value) { append(value); return *this; }
		environment_var &operator-=(ucstring_view value) { remove(value); return *this; }

		using iterator = values_vec_t::iterator;
		using const_iterator = values_vec_t::const_iterator;
		using const_reverse_iterator = values_vec_t::const_reverse_iterator;
		using reverse_iterator = values_vec_t::reverse_iterator;

		const_iterator cbegin() const { return values_.cbegin(); }
		const_iterator cend() const { return values_.cend(); }

		const_reverse_iterator crbegin() const { return values_.crbegin(); }
		const_reverse_iterator crend() const { return values_.crend(); }

		iterator begin() { return values_.begin(); }
		iterator end() { return values_.end(); }

		reverse_iterator rbegin() { return values_.rbegin(); }
		reverse_iterator rend() { return values_.rend(); }

		const_iterator begin() const { return cbegin(); }
		const_iterator end() const { return cend(); }

		const_reverse_iterator rbegin() const { return crbegin(); }
		const_reverse_iterator rend() const { return crend(); }

		MFW_CORE_API void MFW_CORE_CALL commit();

	private:
		ucstring name{};
		ucchar_t sep{u8'\0'};
		values_vec_t values_{};
	};
}

#endif