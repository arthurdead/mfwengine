#include <public/mfw/stl/version.hpp>

#if MFW_OS == MFW_OS_WINDOWS

#ifndef __MFW_PUBLIC_CORE_REGISTRY_HPP
#define __MFW_PUBLIC_CORE_REGISTRY_HPP

#pragma once

#include <public/mfw/core/core.hpp>
#include <public/mfw/stl/scalar.hpp>
#include <public/mfw/stl/string.hpp>

MFW_MESSAGE("get rid of this eventually")
#include <Windows.h>

namespace mfw::core
{
	enum class registry_type : int8_t
	{
		classes_root,
		current_user,
		local_machine,
		users,
		current_config,
	};

	class registry_key
	{
	public:
		registry_key() = default;

		registry_key(registry_key &other) { operator=(other); }
		MFW_CORE_API registry_key & MFW_CORE_CALL operator=(registry_key &other);

		MFW_CORE_API MFW_CORE_CALL registry_key(const pstring &path_);
		MFW_CORE_API MFW_CORE_CALL registry_key(registry_type type, const pstring &path_);
		MFW_CORE_API MFW_CORE_CALL ~registry_key();

		MFW_CORE_API void MFW_CORE_CALL open_key(const pstring &path_, registry_key &subkey);

		MFW_CORE_API void MFW_CORE_CALL get_key_name(uint32_t i, ucstring &name);
		MFW_CORE_API void MFW_CORE_CALL get_value_name(uint32_t i, ucstring &name);

		MFW_CORE_API void MFW_CORE_CALL get_value(const pstring &path_, const ucstring_view &name, uint32_t &value);
		MFW_CORE_API void MFW_CORE_CALL get_value(const pstring &path_, const ucstring_view &name, ucstring &value);

		MFW_CORE_API void MFW_CORE_CALL get_value(const ucstring_view &name, uint32_t &value);
		MFW_CORE_API void MFW_CORE_CALL get_value(const ucstring_view &name, ucstring &value);

		MFW_CORE_API uint32_t MFW_CORE_CALL count() const;

		bool is_valid() const { return key != nullptr; }

		bool operator!() const { return !is_valid(); }
		operator bool() const { return is_valid(); }

	private:
		HKEY key{nullptr};
	};
};

#endif

#endif