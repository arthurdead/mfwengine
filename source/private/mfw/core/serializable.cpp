#include <public/mfw/core/serializable.hpp>
#include <public/mfw/core/core.hpp>
#include <private/mfw/core/serializable_parser.hpp>
#include <private/mfw/core/filesystem.hpp>
#include <private/mfw/core/expression_parser.hpp>
#include <public/mfw/stl/memory.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/pch_literals.hpp>

namespace mfw::core
{
	MFW_CORE_API serializable & MFW_CORE_CALL serializable::operator=(const serializable &other)
	{
		name = other.name;
		value = other.value;
		if(other.flags_) {
			create_flags();
			*flags_ = *other.flags_;
		}
		condition = other.condition;
		merge(other);
		return *this;
	}

	MFW_CORE_API void MFW_CORE_CALL serializable::clear()
	{
		name.clear();
		value.clear();
		flags_.reset(nullptr);
		condition.clear();
		childs.clear();
	}

	MFW_CORE_API bool MFW_CORE_CALL serializable::from_string(const ucstring_view &str, const interfaces::serializable_parser_callbacks *callbacks)
	{
		if(str.empty()) {
			return false;
		}

		if(name.empty()) {
			set_name(u8"inline"_sv);
		}

		return serializable_parser::instance().parse(str, *this, callbacks);
	}

	MFW_CORE_API void MFW_CORE_CALL serializable::to_string(ucstring &str) const
	{
		to_string(str, 0);
		
	}

	void serializable::to_string(ucstring &str, int32_t ident) const
	{
		bool is_root{!parent()};

		if(is_root && ident > 0) {
			ident--;
		}

		int32_t child_ident{ident};
		if(!is_root) {
			child_ident++;
		}

		ucstring spaces{};
		spaces.insert(0, ident * 3, u8' ');

		if(!is_root)
		{
			if(flags_ && !flags_->empty())
			{
				const serializable &flags{*flags_};

				str += spaces;
				str += u8"$(\n"_sv;

				flags.to_string(str, child_ident+1);

				str += spaces;
				str += u8")\n"_sv;
			}

			str += spaces;
			str += u8'"';
			ucstring name_str{name};
			replace_all(name_str, u8"\""_sv, u8"\\\""_sv);
			str += name_str;
			str += u8'"';
			if(!value.empty())
			{
				str += u8' ';
				str += u8'"';
				ucstring val_str{value.get_string()};
				replace_all(val_str, u8"\""_sv, u8"\\\""_sv);
				str += val_str;
				str += u8'"';
			}
			str += u8'\n';
		}

		if(!empty())
		{
			if(!is_root) {
				str += spaces;
				str += u8"{\n"_sv;
			}
			unordered_map<ucstring, vector<const serializable *>> print_later{};
			for(const_iterator it{cbegin()}; it != cend(); it++) {
				const serializable &child{*it};

				const ucstring &cond{child.get_condition()};
				if(!cond.empty()) {
					bool valid{true};
					if(child.parent_) {
						if(child.parent_->get_condition() == cond) {
							valid = false;
						}
					}
					if(valid) {
						print_later[cond].emplace_back(&child);
						continue;
					}
				}

				child.to_string(str, child_ident);
			}
			for(const pair<ucstring, vector<const serializable *>> &it : print_later) {
				str += spaces;
				str += u8"$if "_sv;
				str += it.first;
				str += u8'\n';

				for(const serializable *child : it.second) {
					child->to_string(str, child_ident);
				}

				str += spaces;
				str += u8"$endif\n"_sv;
			}
			if(!is_root) {
				str += spaces;
				str += u8"}\n"_sv;
			}
		}
	}

	serializable *serializable::allocate_child(ssize_t depth, const ucstring_view &name_, const core::serializable *parent) const
	{
		bool negative_depth{depth < 0};

		if(parent_) {
			serializable *child{parent_->allocate_child(depth + (negative_depth ? -1 : 1), name_, parent)};
			if(child) {
				return child;
			}
		}

		return new serializable{};
	}

	MFW_CORE_API serializable & MFW_CORE_CALL serializable::create_child(const ucstring_view &str)
	{
		serializable *ptr{allocate_child(0, str, this)};
		if(!ptr) {
			ptr = new serializable{};
		}
		unique_ptr<serializable> &child{childs.child_vec_t::super::super::emplace_back()};
		child.reset(ptr);
		child->parent_ = this;
		child->set_name(str);
		return *child;
	}

	MFW_CORE_API serializable * MFW_CORE_CALL serializable::get_child(const ucstring_view &str)
	{
		iterator it{find(str)};
		if(it != end()) {
			return &(*it);
		}
		return nullptr;
	}
	
	MFW_CORE_API serializable::iterator MFW_CORE_CALL serializable::find(const ucstring_view &str)
	{
		iterator it{begin()};
		while(it != end()) {
			serializable &child{*it};
			if(child.get_name() == str) {
				return it;
			}
			it++;
		}
		return end();
	}

	MFW_CORE_API const serializable * MFW_CORE_CALL serializable::get_child(const ucstring_view &str) const
	{
		const_iterator it{cbegin()};
		while(it != cend()) {
			const serializable &child{*it};
			if(child.get_name() == str) {
				return &child;
			}
			it++;
		}
		return nullptr;
	}

	void serializable::merge_child(size_t depth, serializable &child, const serializable &other) const
	{
		if(parent_) {
			parent_->merge_child(depth+1, child, other);
		}
	}

	MFW_CORE_API void MFW_CORE_CALL serializable::merge(const serializable &other, const merge_str_process_t &func)
	{
		if(other.empty()) {
			return;
		}

		const_iterator it{other.cbegin()};
		while(it != other.cend()) {
			const serializable &otherchild{*it};

			ucstring othername{otherchild.get_name()};
			univalue othervalue{otherchild.get_value()};
			ucstring othercond{otherchild.get_condition()};

			if(func) {
				func(othername);
				ucstring tmp{othervalue.get_string()};
				func(tmp);
				othervalue = move(tmp);
				func(othercond);
			}

			const serializable *flags{otherchild.get_flags()};

			bool forcecreate{(!othercond.empty() && othercond != get_condition()) || !!flags};
			bool created{false};

			serializable *child{nullptr};
			if(!forcecreate) {
				child = get_child(othername);
				if(!child) {
					child = &create_child(othername);
					created = true;
				}
			} else {
				child = &create_child(othername);
				created = true;
			}

			if(created) {
				if(flags) {
					child->flags().merge(*flags);
				}

				child->set_condition(othercond);
			}

			child->set_value(othervalue);

			child->merge(otherchild, func);

			//child->merge_child(0, *child, otherchild);

			it++;
		}
	}

	MFW_CORE_API serializable * MFW_CORE_CALL serializable::root()
	{
		serializable *parent{this};
		while(parent) {
			serializable *tmp{parent->parent()};
			if(!tmp || !tmp->parent()) {
				return parent;
			}

			parent = tmp;
		}

		return parent;
	}

	MFW_CORE_API serializable & MFW_CORE_CALL serializable::child(const ucstring_view &str)
	{
		serializable *child{get_child(str)};
		if(!child) {
			child = &create_child(str);
		}
		return *child;
	}

	MFW_CORE_API serializable & MFW_CORE_CALL serializable::flags()
	{
		if(!flags_) {
			return create_flags();
		}
		return *get_flags();
	}

	MFW_CORE_API serializable & MFW_CORE_CALL serializable::create_flags()
	{
		if(!flags_) {
			serializable *ptr{allocate_child(-1, {}, this)};
			if(!ptr) {
				ptr = new serializable{};
			}
			flags_.reset(ptr);
		}

		return *flags_.get();
	}

	MFW_CORE_API bool MFW_CORE_CALL serializable::to_file(const searchpath &search) const
	{
		interfaces::filesystem &filesys{interfaces::filesystem::instance()};

		pstring filepath{search.dir()};
		filepath.replace_extension(u8".sr"_p);

		filepath = filesys.resolve({filepath, search.name()}, false);
		if(filepath.empty()) {
			return false;
		}

		if(name.empty()) {
			const_cast<serializable *>(this)->set_name(as_string<ucstring>(filepath));
		}

		ucstring str{};
		to_string(str);

		if(str.empty()) {
			return false;
		}

		return filesys.save_text_file({filepath}, str);
	}

	MFW_CORE_API bool MFW_CORE_CALL serializable::from_file(const searchpath &search, const interfaces::serializable_parser_callbacks *callbacks)
	{
		interfaces::filesystem &filesys{interfaces::filesystem::instance()};

		pstring filepath{search.dir()};
		filepath.replace_extension(u8".sr"_p);

		filepath = filesys.resolve({filepath, search.name()});
		if(filepath.empty()) {
			return false;
		}

		ucstring str{};
		if(!filesys.open_text_file({filepath}, str)) {
			return false;
		}

		if(str.empty()) {
			return false;
		}

		if(name.empty()) {
			set_name(as_string<ucstring>(filepath));
		}

		return from_string(str, callbacks);
	}

	MFW_CORE_API bool MFW_CORE_CALL serializable::passes_condition(const interfaces::expression_parser_callbacks *callbacks) const
	{
		if(condition.empty()) {
			return true;
		}

		univalue result{};
		if(!parse_expression(condition, result, callbacks)) {
			return false;
		}
		return result.get_bool();
	}

	MFW_CORE_API bool MFW_CORE_CALL serializable::erase(const ucstring_view &str)
	{
		if(empty()) {
			return true;
		}

		iterator it{begin()};
		while(it != end()) {
			if((it)->get_name() == str) {
				childs.erase(it);
				return true;
			}
			it++;
		}

		return false;
	}

	MFW_CORE_API const serializable * MFW_CORE_CALL serializable::follow_xpath(const ucstring_view &) const
	{
		//accessor_result result{};
		//accessor_parser::instance().from_string(xpath, this, result);
		//return result.target;
		MFW_MESSAGE("TODO ")
		MFW_DEBUGBREAK();
		return nullptr;
	}

	MFW_CORE_API void MFW_CORE_CALL serializable::follow_xpath(const ucstring_view &, univalue &) const
	{
		//accessor_result result{};
		//accessor_parser::instance().from_string(xpath, this, result);
		//value_ = result.value;
		MFW_MESSAGE("TODO ")
		MFW_DEBUGBREAK();
	}

	MFW_CORE_API size_t MFW_CORE_CALL serializable::index() const
	{
		const serializable *tmp{parent()};
		if(!tmp) {
			return 0;
		}

		size_t i{0};
		for(const serializable &it : *tmp) {
			if(&it == this) {
				return i;
			}
			i++;
		}

		return i;
	}

	MFW_CORE_API bool MFW_CORE_CALL serializable::operator==(const serializable &other)
	{
		if(name != other.name) {
			return false;
		}
		if(value != other.value) {
			return false;
		}
		/*if(condition != other.condition) {
			return false;
		}*/
		/*if(has_flags() == other.has_flags()) {
			if(flags() != other.flags()) {
				return false;
			}
		} else {
			return false;
		}*/
		return (childs == other.childs);
	}
}