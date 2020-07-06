#ifndef __MFW_PUBLIC_CORE_SERIALIZABLE_HPP
#define __MFW_PUBLIC_CORE_SERIALIZABLE_HPP

#pragma once

#include <public/mfw/stl/memory.hpp>
#include <public/mfw/core/core.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/core/univalue.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/core/expression_parser_interface.hpp>
#include <public/mfw/core/searchpath.hpp>

namespace mfw::core
{
	class Serializable;

	class MFW_ABSTRACT_CLASS SerializableParserCallbacks
	{
	protected:
		virtual ~SerializableParserCallbacks() noexcept = default;
		
	public:
		virtual const Serializable *getInherit(stl::osstring_view name) const noexcept = 0;

	protected:
		stl::vector<stl::pstring> m_include_dirs{};
	};

	class MFW_VISIBILITY_PUBLIC Serializable
	{
	public:
		Serializable() noexcept = default;
		virtual ~Serializable() noexcept = default;

		Serializable &operator=(Serializable &&other) noexcept = default;
		Serializable(Serializable &&other) noexcept = default;

		MFW_CORE_API Serializable & MFW_CORE_CALL operator=(const Serializable &other) noexcept;
		Serializable(const Serializable &other) noexcept
		{ operator=(other); }

		MFW_CORE_API bool MFW_CORE_CALL operator==(const Serializable &other) const noexcept;
		bool operator!=(const Serializable &other) const noexcept
		{ return !operator==(other); }

		MFW_CORE_API bool MFW_CORE_CALL fromFileSource(const SearchPath &search, const SerializableParserCallbacks *callbacks = nullptr) noexcept;
		MFW_CORE_API bool MFW_CORE_CALL fromFileBinary(const SearchPath &search) noexcept;
		MFW_CORE_API bool MFW_CORE_CALL fromString(const stl::osstring_view &str, const SerializableParserCallbacks *callbacks = nullptr) noexcept;
		MFW_CORE_API bool MFW_CORE_CALL fromBinary(const stl::vector<stl::byte> &bin) noexcept;

		MFW_CORE_API bool MFW_CORE_CALL toFileSource(const SearchPath &search) const noexcept;
		MFW_CORE_API bool MFW_CORE_CALL toFileBinary(const SearchPath &search) const noexcept;
		MFW_CORE_API Serializable & MFW_CORE_CALL toString(stl::osstring &str) const noexcept;
		MFW_CORE_API Serializable & MFW_CORE_CALL toBinary(stl::vector<stl::byte> &bin) const noexcept;

		MFW_CORE_API Serializable & MFW_CORE_CALL clear() noexcept;

		stl::osstring &name() noexcept
		{ return m_name; }
		const stl::osstring &name() const noexcept
		{ return m_name; }

		UniValue &value() noexcept
		{ return m_value; }
		const UniValue &value() const noexcept
		{ return m_value; }
		bool has_value() const noexcept
		{ return !m_value.empty(); }
		
		bool getValueBool() const noexcept {
			if(m_value.empty()) {
				return true;
			} else {
				return m_value.getBool();
			}
		}

		MFW_CORE_API Serializable * MFW_CORE_CALL root() noexcept;
		MFW_CORE_API const Serializable * MFW_CORE_CALL root() const noexcept;

		Serializable *&parent() noexcept
		{ return m_parent; }
		const Serializable *parent() const noexcept
		{ return m_parent; }
		bool hasParent() const noexcept
		{ return !!m_parent; }

		MFW_CORE_API Serializable & MFW_CORE_CALL flags() noexcept;
		const Serializable &flags() const noexcept
		{ return const_cast<Serializable *>(this)->flags(); }
		MFW_CORE_API Serializable & MFW_CORE_CALL createFlags() noexcept;
		Serializable *getFlags() noexcept
		{ return m_flags.get(); }
		const Serializable *getFlags() const noexcept
		{ return m_flags.get(); }
		bool hasFlags() const noexcept
		{ return static_cast<bool>(m_flags); }
		
		Serializable *getFlag(stl::osstring_view name) noexcept {
			if(!m_flags) {
				return nullptr;
			} else {
				return m_flags->get_child(name);
			}
		}
		const Serializable *getFlag(stl::osstring_view name) const noexcept {
			if(!m_flags) {
				return nullptr;
			} else {
				return m_flags->get_child(name);
			}
		}
		bool getFlagBool(stl::osstring_view name) const noexcept {
			if(!m_flags) {
				return false;
			} else {
				return m_flags->getChildBool(name);
			}
		}

		stl::osstring &condition() noexcept
		{ return m_condition; }
		const stl::osstring &condition() const noexcept
		{ return m_condition; }
		bool hasCondition() const noexcept
		{ return !m_condition.empty(); }
		MFW_CORE_API bool MFW_CORE_CALL passesCondition(const ExpressionParserCallbacks *callbacks = nullptr) const noexcept;

		MFW_CORE_API Serializable & MFW_CORE_CALL getOrCreateChild(stl::osstring_view name) noexcept;
		MFW_CORE_API Serializable & MFW_CORE_CALL createChild(stl::osstring_view name) noexcept
		{ return create_child(str, cend()); }
		MFW_CORE_API Serializable * MFW_CORE_CALL getChild(stl::osstring_view name) noexcept;
		MFW_CORE_API const Serializable * MFW_CORE_CALL getChild(const stl::osstring_view &name) const noexcept;
		const Serializable &geChild(stl::size_t i) const noexcept
		{ return m_childs[i]; }
		const Serializable &operator[](stl::size_t i) const noexcept
		{ return m_childs[i]; }
		Serializable &operator[](stl::osstring_view name) noexcept
		{ return getOrCreateChild(name); }
		bool hasChild(const stl::osstring_view name) const noexcept
		{ return getChild(str) != nullptr; }

		bool getChildBool(stl::osstring_view str) const noexcept {
			const Serializable *child{getChild(str)};
			if(!child) {
				return false;
			}
			return child->getValueBool();
		}

		bool hasFlag(const stl::osstring_view &name) const noexcept
		{ return (hasFlags() && getFlags()->hasChild(name)); }
		Serializable &addFlag(const stl::osstring_view &name) noexcept
		{ flags().getOrCreateChild(name); return *this; }

		Serializable &copy(const Serializable &other) noexcept {
			Serializable &child_{getOrCreateChild(other.name())};
			child_ = other;
			return child_;
		}

		const UniValue *getValue(stl::osstring_view name) const noexcept {
			const Serializable *child{getChild(name)};
			if(!child) {
				return nullptr;
			} else {
				return &child->value();
			}
		}

		MFW_CORE_API stl::size_t MFW_CORE_CALL index() const noexcept;

		Serializable &remove_all() noexcept
		{ m_childs.clear(); return *this; }
		MFW_CORE_API bool MFW_CORE_CALL erase(const stl::osstring_view &str) noexcept;

		using merge_str_process_t = function<void(stl::osstring &str)>;
		MFW_CORE_API Serializable & MFW_CORE_CALL merge(const Serializable &other, bool replace=true, const merge_str_process_t &func = nullptr) noexcept;
		MFW_CORE_API Serializable & MFW_CORE_CALL merge(const Serializable &other, bool replace=true) noexcept;

		MFW_CORE_API Serializable & MFW_CORE_CALL followPath(const stl::osstring_view &path, UniValue &value) const noexcept;
		MFW_CORE_API const Serializable * MFW_CORE_CALL followPath(const stl::osstring_view &path) const noexcept;

		using child_vec_t = stl::ptr_vector<Serializable>;

		using iterator = child_vec_t::iterator;
		using reverse_iterator = child_vec_t::reverse_iterator;
		using const_iterator = child_vec_t::const_iterator;
		using const_reverse_iterator = child_vec_t::const_reverse_iterator;

		MFW_CORE_API iterator MFW_CORE_CALL find(stl::osstring_view name) noexcept;

		void erase(iterator it) noexcept
		{ m_childs.erase(it); }
		void erase(const_iterator it) noexcept
		{ m_childs.erase(it); }

		size_t size() const noexcept
		{ return m_childs.size(); }

		bool empty() const noexcept
		{ return m_childs.empty(); }

		const_iterator cbegin() const noexcept
		{ return m_childs.cbegin(); }
		const_iterator cend() const noexcept
		{ return m_childs.cend(); }

		const_reverse_iterator crbegin() const noexcept
		{ return m_childs.crbegin(); }
		const_reverse_iterator crend() const noexcept
		{ return m_childs.crend(); }

		iterator begin() noexcept
		{ return m_childs.begin(); }
		iterator end() noexcept
		{ return m_childs.end(); }

		reverse_iterator rbegin() noexcept
		{ return m_childs.rbegin(); }
		reverse_iterator rend() noexcept
		{ return m_childs.rend(); }

		const_iterator begin() const noexcept
		{ return m_childs.begin(); }
		const_iterator end() const noexcept
		{ return m_childs.end(); }

		const_reverse_iterator rbegin() const noexcept
		{ return m_childs.rbegin(); }
		const_reverse_iterator rend() const noexcept
		{ return m_childs.rend(); }

		MFW_CORE_API Serializable & MFW_CORE_CALL find_or_emplace(stl::osstring_view name, const_iterator it) noexcept;
		Serializable &find_or_emplace(stl::osstring_view name) noexcept
		{ return find_or_emplace(name, cend()); }

	private:
		virtual Serializable *allocateChild(stl::ssize_t depth, stl::osstring_view name, const Serializable *parent) const noexcept;
		virtual void merge_child(stl::size_t depth, Serializable &child, const Serializable &other) const noexcept;
		Serializable &to_string(stl::osstring &str, stl::size_t ident) const noexcept;
		Serializable &create_child(stl::osstring_view str, const_iterator it) noexcept;

		stl::osstring m_name{};
		UniValue m_value{};
		stl::unique_ptr<Serializable> m_flags{};
		stl::osstring m_condition{};
		Serializable *m_parent{nullptr};
		child_vec_t m_childs{};
	};
}

#endif