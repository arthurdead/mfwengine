#include <public/mfw/core/serializable.hpp>
#include <public/mfw/core/core.hpp>
#include <private/mfw/core/serializable_parser.hpp>
#include <private/mfw/core/filesystem.hpp>
#include <private/mfw/core/expression_parser.hpp>
#include <public/mfw/stl/memory.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/vector.hpp>

namespace mfw::core
{
	MFW_VISIBILITY_LOCAL_PUSH()

	MFW_CORE_API serializable & MFW_CORE_CALL serializable::operator=(const serializable &other) noexcept
	{
		m_name = other.m_name;
		m_value = other.m_value;
		if(other.m_flags_) {
			create_flags();
			*m_flags = *other.m_flags;
		}
		m_condition = other.m_condition;
		merge(other);
		return *this;
	}

	MFW_CORE_API bool MFW_CORE_CALL serializable::from_string(stl::osstring_view str, const_optional_parser_callbacks callbacks) noexcept
	{
		if(str.empty()) {
			return false;
		}

		if(m_name.empty()) {
			m_name.assign(MFW_T("inline"_sv));
		}

		return serializable_parser::instance().parse(str, *this, callbacks);
	}

	MFW_CORE_API serializable & MFW_CORE_CALL serializable::to_string(stl::osstring &str) const noexcept
	{
		to_string(str, 0);
		return *this;
	}

	void serializable::to_string(stl::osstring &str, stl::size_t ident) const noexcept
	{
		bool is_root{!m_parent.has_value()};

		if(is_root && ident > 0) {
			ident--;
		}

		stl::size_t child_ident{ident};
		if(!is_root) {
			child_ident++;
		}

		stl::osstring spaces{};
		spaces.insert(0, ident * 3, MFW_T(' '));

		if(!is_root)
		{
			if(m_flags && !m_flags->empty())
			{
				str += spaces;
				str += MFW_T("$(\n"_sv);

				m_flags->to_string(str, child_ident+1);

				str += spaces;
				str += MFW_T(")\n"_sv);
			}

			str += spaces;
			str += MFW_T('"');
			stl::osstring name_str{m_name};
			replace_all(name_str, MFW_T('\"'), MFW_T("\\\""_sv));
			str += stl::move(name_str);
			str += MFW_T('"');
			if(!m_value.empty())
			{
				str += MFW_T(' ');
				str += MFW_T('"');
				stl::osstring val_str{m_value.get_string()};
				replace_all(val_str, MFW_T('\"'), MFW_T("\\\""_sv));
				str += stl::move(val_str);
				str += MFW_T('"');
			}
			str += MFW_T('\n');
		}

		if(!m_childs.empty())
		{
			if(!is_root) {
				str += spaces;
				str += MFW_T("{\n"_sv);
			}
			using print_later_t = stl::unordered_map<stl::osstring, stl::vector<stl::reference_wrapper<const serializable>>>;
			print_later_t print_later{};
			child_vec_t::const_iterator end_{m_childs.cend()};
			for(child_vec_t::const_iterator it{m_childs.cbegin()}; it != end_; ++it) {
				const serializable &child{*it};

				const stl::osstring &cond{child.m_condition};
				if(!cond.empty()) {
					bool valid{true};
					if(child.m_parent.has_value()) {
						if(child.m_parent->m_condition == cond) {
							valid = false;
						}
					}
					if(valid) {
						print_later[cond].emplace_back(child);
						continue;
					}
				}

				child.to_string(str, child_ident);
			}
			for(print_later_t::value_type &it : print_later) {
				str += spaces;
				str += MFW_T("$if "_sv);
				str += stl::move(it.first);
				str += MFW_T('\n');

				for(const serializable &child : it.second) {
					child.to_string(str, child_ident);
				}

				str += spaces;
				str += MFW_T("$endif\n"_sv);
			}
			if(!is_root) {
				str += spaces;
				str += MFW_T("}\n"_sv);
			}
		}
	}

	serializable *serializable::allocate_child(stl::ssize_t depth, stl::osstring_view name, const serializable &parent) const noexcept
	{
		if(m_parent.has_value()) {
			if(depth < 0) {
				--depth;
			} else {
				++depth;
			}
			serializable *tmp{m_parent->allocate_child(depth, name, parent)};
			if(tmp) {
				return tmp;
			}
		}

		return new serializable{};
	}

	MFW_CORE_API serializable & MFW_CORE_CALL serializable::create_child_base(stl::osstring_view name, const_iterator it) noexcept
	{
		serializable *ptr{allocate_child(0, name, *this)};
		if(!ptr) {
			ptr = new serializable{};
		}
		stl::unique_ptr<serializable> &tmp{*childs.child_vec_t::super::super::emplace(it)};
		tmp.reset(ptr);
		tmp->m_parent.emplace(*this);
		return *tmp;
	}

	MFW_CORE_API serializable::iterator MFW_CORE_CALL serializable::find(stl::osstring_view name) noexcept
	{
		child_vec_t::iterator it{m_childs.begin()};
		child_vec_t::const_iterator end_{m_childs.cend()};
		while(it != end_) {
			serializable &tmp{*it};
			if(tmp.m_name == str) {
				return it;
			}
			++it;
		}
		return end_;
	}

	MFW_CORE_API serializable::const_iterator MFW_CORE_CALL serializable::find(stl::osstring_view name) const noexcept
	{
		child_vec_t::const_iterator it{m_childs.begin()};
		child_vec_t::const_iterator end_{m_childs.cend()};
		while(it != end_) {
			const serializable &child{*it};
			if(child.m_name == str) {
				return it;
			}
			++it;
		}
		return end_;
	}

	void serializable::merge_child(stl::size_t depth, serializable &child, const serializable &other) const noexcept
	{
		if(m_parent.has_value()) {
			if(depth < 0) {
				--depth;
			} else {
				++depth;
			}
			m_parent->merge_child(depth, child, other);
		}
	}

	MFW_CORE_API void MFW_CORE_CALL serializable::merge(const serializable &other, bool replace) noexcept
	{
		if(other.m_childs.empty()) {
			return;
		}

		child_vec_t::const_iterator it{other.m_childs.cbegin()};
		child_vec_t::const_iterator end_{other.m_childs.cend()};
		while(it != end_) {
			const serializable &otherchild{*it};

			const stl::osstring &othername{otherchild.m_name};
			const univalue &othervalue{otherchild.m_value};
			const stl::osstring &othercond{otherchild.m_condition};

			const flags_t &flags{otherchild.m_flags};

			bool forcecreate{!othercond.empty() && othercond != m_condition};
			/*if(!replace && flags) {
				forcecreate = true;
			}*/
			bool created{false};

			stl::reference_wrapper<serializable> child{};
			if(!forcecreate) {
				const_optional_child tmp{get_child(othername)};
				if(tmp.has_value()) {
					child = *tmp;
				} else {
					child = create_child(othername);
					created = true;
				}
			} else {
				child = create_child(othername);
				created = true;
			}

			if(created) {
				if(flags) {
					child->flags().merge(*flags);
				}

				child->m_condition = othercond;
			}

			if(replace || created) {
				child->m_value = othervalue;
			}

			child->merge(otherchild, replace);

			//child->merge_child(0, *child, otherchild);

			it++;
		}
	}

	MFW_CORE_API void MFW_CORE_CALL serializable::merge(const serializable &other, bool replace, const merge_str_process_t &func) noexcept
	{
		if(other.m_childs.empty()) {
			return;
		}

		child_vec_t::const_iterator it{other.m_childs.cbegin()};
		child_vec_t::const_iterator end_{other.m_childs.cend()};
		while(it != end_) {
			const serializable &otherchild{*it};

			stl::osstring othername{otherchild.m_name};
			univalue othervalue{otherchild.m_value};
			stl::osstring othercond{otherchild.m_condition};

			if(func) {
				func(othername);
				stl::osstring tmp{othervalue.get_string()};
				func(tmp);
				othervalue = stl::move(tmp);
				func(othercond);
			}

			const flags_t &flags{otherchild.m_flags};

			bool forcecreate{!othercond.empty() && othercond != m_condition};
			/*if(!replace && flags) {
				forcecreate = true;
			}*/
			bool created{false};

			stl::reference_wrapper<serializable> child{};
			if(!forcecreate) {
				const_optional_child tmp{get_child(othername)};
				if(tmp.has_value()) {
					child = *tmp;
				} else {
					child = create_child(stl::move(othername));
					created = true;
				}
			} else {
				child = create_child(stl::move(othername));
				created = true;
			}

			if(created) {
				if(flags) {
					child->flags().merge(*flags);
				}

				child->m_condition = stl::move(othercond);
			}

			if(replace || created) {
				child->m_value = stl::move(othervalue);
			}

			child->merge(otherchild, replace);

			//child->merge_child(0, *child, otherchild);

			it++;
		}
	}

	MFW_CORE_API serializable * MFW_CORE_CALL serializable::root() noexcept
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

	MFW_CORE_API serializable & MFW_CORE_CALL serializable::create_flags() noexcept
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

	MFW_CORE_API bool MFW_CORE_CALL serializable::to_file_string(searchpath_view search) const noexcept
	{
		filesystem &filesys{filesystem::instance()};

		pstring filepath{stl::move(search.m_path)};
		filepath.replace_extension(MFW_T(".sr"_p));

		filepath = filesys.resolve({stl::move(filepath), stl::move(search.m_name)}, false);
		if(filepath.empty()) {
			return false;
		}

		if(m_name.empty()) {
			const_cast<serializable *>(this)->m_name.assign(filepath);
		}

		stl::osstring str{};
		to_string(str);

		if(str.empty()) {
			return false;
		}

		return filesys.save_text_file({stl::move(filepath)}, str);
	}

	MFW_CORE_API bool MFW_CORE_CALL serializable::from_file_string(searchpath_view search, const_optional_parser_callbacks callbacks) noexcept
	{
		filesystem &filesys{filesystem::instance()};

		pstring filepath{stl::move(search.m_path)};
		filepath.replace_extension(MFW_T(".sr"_p));

		filepath = filesys.resolve({stl::move(filepath), stl::move(search.m_name)});
		if(filepath.empty()) {
			return false;
		}

		stl::osstring str{};
		if(!filesys.open_text_file({filepath}, str)) {
			return false;
		}

		if(str.empty()) {
			return false;
		}

		if(m_name.empty()) {
			m_name.assign(stl::move(filepath));
		}

		return from_string(str, callbacks);
	}

	MFW_CORE_API bool MFW_CORE_CALL serializable::passes_condition(const_optional_parser_callbacks callbacks) const noexcept
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

	MFW_CORE_API bool MFW_CORE_CALL serializable::erase(stl::osstring_view name) noexcept
	{
		if(m_childs.empty()) {
			return true;
		}

		child_vec_t::iterator it{m_childs.begin()};
		child_vec_t::const_iterator end_{m_childs.end()};
		while(it != end_) {
			if(it->m_name == name) {
				m_childs.erase(it);
				return true;
			}
			++it;
		}

		return false;
	}

	MFW_CORE_API optional_child MFW_CORE_CALL serializable::follow_path(stl::osstring_view) noexcept
	{
		//accessor_result result{};
		//accessor_parser::instance().from_string(xpath, this, result);
		//return result.target;
		MFW_MESSAGE("TODO ")
		MFW_DEBUGBREAK();
		return stl::nullopt;
	}

	MFW_CORE_API const_optional_child MFW_CORE_CALL serializable::follow_path(stl::osstring_view) const noexcept
	{
		//accessor_result result{};
		//accessor_parser::instance().from_string(xpath, this, result);
		//return result.target;
		MFW_MESSAGE("TODO ")
		MFW_DEBUGBREAK();
		return stl::nullopt;
	}

	MFW_CORE_API serializable & MFW_CORE_CALL serializable::follow_path(stl::osstring_view, univalue &) const noexcept
	{
		//accessor_result result{};
		//accessor_parser::instance().from_string(xpath, this, result);
		//value_ = result.value;
		MFW_MESSAGE("TODO ")
		MFW_DEBUGBREAK();
		return *this;
	}

	MFW_CORE_API stl::size_t MFW_CORE_CALL serializable::index() const
	{
		const_optional_child tmp{parent()};
		if(!tmp.has_value()) {
			return 0;
		}

		stl::size_t i{0};
		for(const serializable &it : *tmp) {
			if(&it == this) {
				return i;
			}
			++i;
		}

		return i;
	}

	MFW_CORE_API bool MFW_CORE_CALL serializable::operator==(const serializable &other) noexcept
	{
		if(m_name != other.m_name) {
			return false;
		}
		if(m_value != other.m_value) {
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
		if(!m_childs.empty()) {
			if(m_childs != other.m_childs) {
				return false;
			}
		}
		return true;
	}

	MFW_VISIBILITY_LOCAL_POP()
}