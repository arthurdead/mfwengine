#ifndef __MFW_PUBLIC_CORE_SERIALIZABLE_H
#define __MFW_PUBLIC_CORE_SERIALIZABLE_H

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
	class serializable;

	namespace interfaces
	{
		class serializable_parser_callbacks
		{
		protected:
			virtual ~serializable_parser_callbacks() = default;
			
		public:
			virtual const serializable *get_inherit(const ucstring_view &name) const = 0;
		};
	}

	class serializable //: public use_allocator<serializable>
	{
	public:
		serializable() = default;
		virtual ~serializable() = default;

		serializable &operator=(serializable &&other) = default;
		serializable(serializable &&other) = default;

		MFW_CORE_API serializable & MFW_CORE_CALL operator=(const serializable &other);
		serializable(const serializable &other) { operator=(other); }

		MFW_CORE_API bool MFW_CORE_CALL operator==(const serializable &other);
		bool operator!=(const serializable &other) { return !operator==(other); }

		MFW_CORE_API bool MFW_CORE_CALL from_file(const searchpath &search, const interfaces::serializable_parser_callbacks *callbacks = nullptr);
		MFW_CORE_API bool MFW_CORE_CALL from_string(const ucstring_view &str, const interfaces::serializable_parser_callbacks *callbacks = nullptr);

		MFW_CORE_API bool MFW_CORE_CALL to_file(const searchpath &search) const;
		MFW_CORE_API void MFW_CORE_CALL to_string(ucstring &str) const;

		MFW_CORE_API void MFW_CORE_CALL clear();

		void set_name(const ucstring_view &str) { name = str; }
		const ucstring &get_name() const { return name; }

		template <typename T>
		void set_value(const T &val) { value.set(val); }
		const univalue &get_value() const { return value; }
		bool has_value() const { return !value.empty(); }
		
		bool get_value_bool() const {
			if(value.empty()) {
				return true;
			}
			return value.get_bool();
		}

		MFW_CORE_API serializable * MFW_CORE_CALL root();
		const serializable *root() const { return const_cast<serializable *>(this)->root(); }

		void set_parent(serializable *parent) { parent_ = parent; }
		serializable *get_parent() { return parent_; }
		const serializable *get_parent() const { return parent_; }
		bool has_parent() const { return parent_ != nullptr; }

		serializable *parent() { return get_parent(); }
		const serializable *parent() const { return get_parent(); }

		MFW_CORE_API serializable & MFW_CORE_CALL flags();
		const serializable &flags() const { return const_cast<serializable *>(this)->flags(); }
		MFW_CORE_API serializable & MFW_CORE_CALL create_flags();
		serializable *get_flags() { return flags_.get(); }
		const serializable *get_flags() const { return flags_.get(); }
		bool has_flags() const { return static_cast<bool>(flags_); }
		
		serializable *get_flag(const ucstring_view &str) {
			serializable *flags_ptr{get_flags()};
			if(!flags_ptr) {
				return nullptr;
			}
			return flags_ptr->get_child(str);
		}
		const serializable *get_flag(const ucstring_view &str) const  {
			const serializable *flags_ptr{get_flags()};
			if(!flags_ptr) {
				return nullptr;
			}
			return flags_ptr->get_child(str);
		}
		bool get_flag_bool(const ucstring_view &name_) const {
			const serializable *flags_ptr{get_flags()};
			if(!flags_ptr) {
				return false;
			}
			return flags_ptr->get_child_bool(name_);
		}

		bool has_condition() const { return !condition.empty(); }
		void set_condition(const ucstring &val) { condition = val; }
		const ucstring &get_condition() const { return condition; }
		MFW_CORE_API bool MFW_CORE_CALL passes_condition(const interfaces::expression_parser_callbacks *callbacks = nullptr) const;

		MFW_CORE_API serializable & MFW_CORE_CALL child(const ucstring_view &str);
		MFW_CORE_API serializable & MFW_CORE_CALL create_child(const ucstring_view &str);
		MFW_CORE_API serializable * MFW_CORE_CALL get_child(const ucstring_view &str);
		MFW_CORE_API const serializable * MFW_CORE_CALL get_child(const ucstring_view &str) const;
		const serializable &get_child(size_t i) const { return childs[i]; }
		const serializable &operator[](size_t i) const { return get_child(i); }
		serializable &operator[](const ucstring_view &str) { return child(str); }
		bool has_child(const ucstring_view &str) const { return get_child(str) != nullptr; }

		bool get_child_bool(const ucstring_view &str) const {
			const serializable *child{get_child(str)};
			if(!child) {
				return false;
			}

			return child->get_value_bool();
		}

		bool has_flag(const ucstring_view &name_) const { return (has_flags() && get_flags()->has_child(name_)); }
		void add_flag(const ucstring_view &name_) { flags().child(name_); }

		serializable &copy(const serializable &other) {
			serializable &child_{child(other.get_name())};
			child_ = other;
			return child_;
		}

		const univalue *get_value(const ucstring_view &str) const {
			const serializable *child{get_child(str)};
			if(!child) {
				return nullptr;
			}
			return &child->get_value();
		}

		MFW_CORE_API size_t MFW_CORE_CALL index() const;

		void remove_all() { childs.clear(); }
		MFW_CORE_API bool MFW_CORE_CALL erase(const ucstring_view &str);

		using merge_str_process_t = function<void(ucstring &str)>;
		MFW_CORE_API void MFW_CORE_CALL merge(const serializable &other, const merge_str_process_t &func = nullptr);

		MFW_CORE_API void MFW_CORE_CALL follow_xpath(const ucstring_view &xpath, univalue &value_) const;
		MFW_CORE_API const serializable * MFW_CORE_CALL follow_xpath(const ucstring_view &xpath) const;

		using child_vec_t = ptr_vector<serializable>;

		using iterator = child_vec_t::iterator;
		using reverse_iterator = child_vec_t::reverse_iterator;
		using const_iterator = child_vec_t::const_iterator;
		using const_reverse_iterator = child_vec_t::const_reverse_iterator;

		MFW_CORE_API iterator MFW_CORE_CALL find(const ucstring_view &str);

		void erase(iterator it) { childs.erase(it); }
		void erase(const_iterator it) { childs.erase(it); }

		size_t size() const { return childs.size(); }

		bool empty() const { return childs.empty(); }

		const_iterator cbegin() const { return childs.cbegin(); }
		const_iterator cend() const { return childs.cend(); }

		const_reverse_iterator crbegin() const { return childs.crbegin(); }
		const_reverse_iterator crend() const { return childs.crend(); }

		iterator begin() { return childs.begin(); }
		iterator end() { return childs.end(); }

		reverse_iterator rbegin() { return childs.rbegin(); }
		reverse_iterator rend() { return childs.rend(); }

		const_iterator begin() const { return cbegin(); }
		const_iterator end() const { return cend(); }

		const_reverse_iterator rbegin() const { return crbegin(); }
		const_reverse_iterator rend() const { return crend(); }

	private:
		virtual serializable *allocate_child(ssize_t depth, const ucstring_view &name, const core::serializable *parent) const;
		virtual void merge_child(size_t depth, serializable &child, const serializable &other) const;
		void to_string(ucstring &str, int32_t ident) const;

		ucstring name{};
		univalue value{};
		unique_ptr<serializable> flags_{};
		ucstring condition{};
		serializable *parent_{nullptr};
		child_vec_t childs{};
	};
}

#endif