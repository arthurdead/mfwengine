#ifndef __MFW_PUBLIC_CORE_DEBUGGING_HPP
#define __MFW_PUBLIC_CORE_DEBUGGING_HPP

#pragma once

#include <public/mfw/core/core.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/stl/forward_list.hpp>

namespace mfw::core
{
	enum class undecorate_flags : int32_t
	{
		complete = MFW_BIT(0),
		no_leading_under_scores = MFW_BIT(1),
		no_ms_keywords = MFW_BIT(2),
		no_func_return = MFW_BIT(3),
		no_allocation_model = MFW_BIT(4),
		no_allocation_languague = MFW_BIT(5),
		no_ms_this_type = MFW_BIT(6),
		no_cv_this_type = MFW_BIT(7),
		no_this_type = MFW_BIT(8),
		no_access_specifiers = MFW_BIT(9),
		no_throw_signatures = MFW_BIT(10),
		no_memberType = MFW_BIT(11),
		no_udt_return_model = MFW_BIT(12),
		undecorate_32bit = MFW_BIT(13),
		name_only = MFW_BIT(14),
		no_arguments = MFW_BIT(15),
		no_special_syms = MFW_BIT(16),
	};
	MFW_ENUM_FLAGS(undecorate_flags)

	struct symbol
	{
		ucstring name{};
		int32_t line{0};
		ucstring filename{};
		ucstring library{};
	};

	MFW_CORE_API bool MFW_CORE_CALL undecorate(const ucstring_view &decorated, ucstring &undecorated, undecorate_flags flags = undecorate_flags::complete);

	MFW_CORE_API bool MFW_CORE_CALL symbols_from_address(const void *ptr, symbol &sym);
	MFW_CORE_API bool MFW_CORE_CALL walk_stack(vector<symbol> &symbols, uint32_t count=0);

	MFW_CORE_API void MFW_CORE_CALL print_stack(uint32_t count = 0);

	class rtti
	{
	public:
		rtti() = default;

		template <typename T>
		rtti(const T *ptr) { initialize(ptr); }

		MFW_CORE_API void MFW_CORE_CALL clear();

		template <typename T>
		bool initialize(const T *ptr)
		{
			if(!initialize(reinterpret_cast<const void *>(ptr))) {
				typeinfo = &get_typeid<T>();
				name_.assign(reinterpret_cast<const char8_t *>(typeinfo->name()));
			}
			return true;
		}

		const ucstring &name() const { return name_; }

		MFW_CORE_API bool MFW_CORE_CALL is_derived_of(const ucstring_view &name) const;
		bool is_derived_of(const rtti &info) const { return is_derived_of(info.name()); }
		template <typename T>
		bool is_derived_of(const T *ptr) const { return is_derived_of(rtti{ptr}.name()); }

		void reverse() { baselist.reverse(); }

		using rtti_list_t = forward_list<rtti>;
		using const_iterator = rtti_list_t::const_iterator;
		using iterator = rtti_list_t::iterator;

		const_iterator cbefore_begin() const { return baselist.cbefore_begin(); }
		const_iterator cbegin() const { return baselist.cbegin(); }
		const_iterator cend() const { return baselist.cend(); }

		iterator before_begin() { return baselist.before_begin(); }
		iterator begin() { return baselist.begin(); }
		iterator end() { return baselist.end(); }

		const_iterator before_begin() const { return cbefore_begin(); }
		const_iterator begin() const { return cbegin(); }
		const_iterator end() const { return cend(); }

	private:
		MFW_CORE_API bool MFW_CORE_CALL initialize(const void *ptr);
		void initialize_internal(const void *ptr);

		const ::std::type_info *typeinfo{nullptr};
		rtti_list_t baselist{};
		ucstring name_{};
	};
}

#endif