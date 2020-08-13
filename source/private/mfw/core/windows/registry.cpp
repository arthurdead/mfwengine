#include <public/mfw/stl/version.hpp>

#if MFW_OS == MFW_OS_WINDOWS

#include <public/mfw/core/core.hpp>
#include <public/mfw/core/registry.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/scalar.hpp>

namespace mfw::core
{
	MFW_CORE_API registry_key & MFW_CORE_CALL registry_key::operator=(registry_key &other)
	{
		key = other.key;
		other.key = nullptr;
		return *this;
	}

	MFW_CORE_API MFW_CORE_CALL registry_key::registry_key(const pstring &path_)
	{
		LSTATUS status{RegLoadAppKeyW(path_.c_str(), &key, KEY_READ, REG_NONE, 0)};
		if(status != ERROR_SUCCESS) {
			key = nullptr;
		}
	}

	MFW_CORE_API MFW_CORE_CALL registry_key::registry_key(registry_type type, const pstring &path_)
	{
		HKEY parent{};
		switch(type)
		{
			case registry_type::classes_root: { parent = HKEY_CLASSES_ROOT; break; }
			case registry_type::current_user: { parent = HKEY_CURRENT_USER; break; }
			case registry_type::local_machine: { parent = HKEY_LOCAL_MACHINE; break; }
			case registry_type::users: { parent = HKEY_USERS; break; }
			case registry_type::current_config: { parent = HKEY_CURRENT_CONFIG; break; }
		}

	#if MFW_COMPILER == MFW_COMPILER_MSVC
		#pragma warning(suppress: 6387)
	#endif
		LSTATUS status{RegOpenKeyExW(parent, path_.c_str(), REG_NONE, KEY_READ, &key)};
		if(status != ERROR_SUCCESS) {
			key = nullptr;
		}
	}

	MFW_CORE_API MFW_CORE_CALL registry_key::~registry_key()
	{
		if(key) {
			RegCloseKey(key);
		}
	}

	MFW_CORE_API uint32_t MFW_CORE_CALL registry_key::count() const
	{
		uint32_t numkeys{};
		LSTATUS status{RegQueryInfoKeyW(key, nullptr, nullptr, nullptr, reinterpret_cast<unsigned long *>(&numkeys), nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr)};
		if(status != ERROR_SUCCESS) {
			return 0;
		}
		return numkeys;
	}

	MFW_CORE_API void MFW_CORE_CALL registry_key::get_key_name(uint32_t i, u16string &name)
	{
		uint32_t maxname{};
		LSTATUS status{RegQueryInfoKeyW(key, nullptr, nullptr, nullptr, nullptr, reinterpret_cast<unsigned long *>(&maxname), nullptr, nullptr, nullptr, nullptr, nullptr, nullptr)};
		if(status != ERROR_SUCCESS) {
			name.clear();
			return;
		}
		maxname++;
		name.resize(maxname);
		status = RegEnumKeyExW(key, i, reinterpret_cast<wchar_t *>(name.data()), reinterpret_cast<unsigned long *>(&maxname), nullptr, nullptr, nullptr, nullptr);
		if(status != ERROR_SUCCESS) {
			name.clear();
			return;
		}
		name.resize(maxname);
	}

	MFW_CORE_API void MFW_CORE_CALL registry_key::get_value_name(uint32_t i, u16string &name)
	{
		uint32_t maxname{};
		LSTATUS status{RegQueryInfoKeyW(key, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, reinterpret_cast<unsigned long *>(&maxname), nullptr, nullptr, nullptr)};
		if(status != ERROR_SUCCESS) {
			name.clear();
			return;
		}
		maxname++;
		name.resize(maxname);
		status = RegEnumValueW(key, i, reinterpret_cast<wchar_t *>(name.data()), reinterpret_cast<unsigned long *>(&maxname), nullptr, nullptr, nullptr, nullptr);
		if(status != ERROR_SUCCESS) {
			name.clear();
			return;
		}
		name.resize(maxname);
	}

	MFW_CORE_API void MFW_CORE_CALL registry_key::open_key(const pstring &path_, registry_key &subkey)
	{
		LSTATUS status{RegOpenKeyExW(key, path_.c_str(), REG_NONE, KEY_READ, &subkey.key)};
		if(status != ERROR_SUCCESS) {
			subkey.key = nullptr;
		}
	}

	MFW_CORE_API void MFW_CORE_CALL registry_key::get_value(const pstring &path_, const u16string_view &name, u16string &value)
	{
		uint32_t maxvalue{};
		LSTATUS status{RegGetValueW(key, path_.c_str(), reinterpret_cast<const wchar_t *>(name.data()), RRF_RT_REG_SZ, nullptr, nullptr, reinterpret_cast<unsigned long *>(&maxvalue))};
		if(status != ERROR_SUCCESS && status != ERROR_MORE_DATA) {
			value.clear();
			return;
		}
		value.resize(maxvalue / sizeof(wchar_t));
		status = RegGetValueW(key, path_.c_str(), reinterpret_cast<const wchar_t *>(name.data()), RRF_RT_REG_SZ, nullptr, value.data(), reinterpret_cast<unsigned long *>(&maxvalue));
		if(status != ERROR_SUCCESS) {
			value.clear();
			return;
		}
		value.erase(value.cend() - 1);
	}

	MFW_CORE_API void MFW_CORE_CALL registry_key::get_value(const pstring &path_, const u16string_view &name, uint32_t &value)
	{
		uint32_t maxvalue{};
		LSTATUS status{RegGetValueW(key, path_.c_str(), reinterpret_cast<const wchar_t *>(name.data()), RRF_RT_REG_DWORD, nullptr, nullptr, reinterpret_cast<unsigned long *>(&maxvalue))};
		if(status != ERROR_SUCCESS && status != ERROR_MORE_DATA) {
			value = UINT32_MAX;
			return;
		}
		value = (maxvalue / sizeof(uint32_t));
		status = RegGetValueW(key, path_.c_str(), reinterpret_cast<const wchar_t *>(name.data()), RRF_RT_REG_DWORD, nullptr, &value, reinterpret_cast<unsigned long *>(&maxvalue));
		if(status != ERROR_SUCCESS) {
			value = UINT32_MAX;
			return;
		}
	}

	MFW_CORE_API void MFW_CORE_CALL registry_key::get_value(const u16string_view &name, uint32_t &value)
	{
		uint32_t maxvalue{};
		LSTATUS status{RegQueryValueExW(key, reinterpret_cast<const wchar_t *>(name.data()), nullptr, nullptr, reinterpret_cast<::byte *>(&value), reinterpret_cast<unsigned long *>(&maxvalue))};
		if(status != ERROR_SUCCESS && status != ERROR_MORE_DATA) {
			value = UINT32_MAX;
			return;
		}
		value = (maxvalue / sizeof(uint32_t));
		status = RegQueryValueExW(key, reinterpret_cast<const wchar_t *>(name.data()), nullptr, nullptr, reinterpret_cast<::byte *>(&value), reinterpret_cast<unsigned long *>(&maxvalue));
		if(status != ERROR_SUCCESS) {
			value = UINT32_MAX;
			return;
		}
	}

	MFW_CORE_API void MFW_CORE_CALL registry_key::get_value(const u16string_view &name, u16string &value)
	{
		uint32_t maxvalue{};
		LSTATUS status{RegQueryValueExW(key, reinterpret_cast<const wchar_t *>(name.data()), nullptr, nullptr, reinterpret_cast<::byte *>(value.data()), reinterpret_cast<unsigned long *>(&maxvalue))};
		if(status != ERROR_SUCCESS && status != ERROR_MORE_DATA) {
			value.clear();
			return;
		}
		value.resize(maxvalue / sizeof(wchar_t));
		status = RegQueryValueExW(key, reinterpret_cast<const wchar_t *>(name.data()), nullptr, nullptr, reinterpret_cast<::byte *>(value.data()), reinterpret_cast<unsigned long *>(&maxvalue));
		if(status != ERROR_SUCCESS) {
			value.clear();
			return;
		}
		value.erase(value.cend() - 1);
	}
};

#endif