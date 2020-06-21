#include <public/mfw/core/globals.hpp>
#include <public/mfw/core/application.hpp>
#include <private/mfw/core/globals_internal.hpp>
#include <public/mfw/stl/memory.hpp>
#include <public/mfw/stl/vector.hpp>

namespace mfw::core
{
	namespace __globals_internal
	{
		using globalallocators_vector_t = vector<interfaces::global_allocator *>;
		globalallocators_vector_t *globalallocators{nullptr};

		using globalinitializers_vector_t = vector<interfaces::global_initializer *>;
		globalinitializers_vector_t *globalinitializers{nullptr};

		template <typename T>
		static exit_status execute_func_all_globals(T func)
		{
			exit_status status{};

			if(globalinitializers) {
				globalinitializers_vector_t &tmp{*globalinitializers};
				for(const globalinitializers_vector_t::value_type &it : tmp) {
					status += (it->*func)();
					if(!status.succeded()) {
						return status;
					}
				}
			}

			return status;
		}
	}

	namespace interfaces
	{
		MFW_CORE_API MFW_CORE_CALL global_allocator::global_allocator()
		{
			if(!__globals_internal::globalallocators) {
				__globals_internal::globalallocators = new __globals_internal::globalallocators_vector_t{};
			}

			__globals_internal::globalallocators_vector_t &tmp{*__globals_internal::globalallocators};
			tmp.push_back(this);
		}

		MFW_CORE_API MFW_CORE_CALL global_allocator::~global_allocator()
		{
			//globalallocators_vector_t &tmp{*__globalallocators};
			//tmp.erase(::std::remove(tmp.begin(), tmp.end(), this), tmp.end());
		}

		MFW_CORE_API MFW_CORE_CALL global_initializer::global_initializer()
		{
			if(!__globals_internal::globalinitializers) {
				__globals_internal::globalinitializers = new __globals_internal::globalinitializers_vector_t{};
			}

			__globals_internal::globalinitializers_vector_t &tmp{*__globals_internal::globalinitializers};
			tmp.push_back(this);
		}

		MFW_CORE_API MFW_CORE_CALL global_initializer::~global_initializer()
		{
			//globalinitializers_vector_t &tmp{*__globalinitializers};
			//tmp.erase(::std::remove(tmp.begin(), tmp.end(), this), tmp.end());
		}

		MFW_CORE_API MFW_CORE_CALL global_initializer::global_initializer(ucstring_view _name_)
			: global_initializer{} {
			name_ = _name_;
		}

		MFW_CORE_API MFW_CORE_CALL global_initializer::global_initializer(ucstring_view _name_, const initializer_list<ucstring_view> &_depends_)
			: global_initializer{} {
			name_ = _name_;
			for(const ucstring_view &it : _depends_) {
				depends_.emplace_back(move(it));
			}
		}
	}

	void allocate_all_globals()
	{
		if(!__globals_internal::globalallocators) {
			return;
		}

		__globals_internal::globalallocators_vector_t &tmp{*__globals_internal::globalallocators};
		for(const __globals_internal::globalallocators_vector_t::value_type &it : tmp) {
			it->allocate();
		}
	}

	void deallocate_all_globals()
	{
		if(!__globals_internal::globalallocators) {
			return;
		}

		__globals_internal::globalallocators_vector_t &tmp{*__globals_internal::globalallocators};
		for(const __globals_internal::globalallocators_vector_t::value_type &it : tmp) {
			it->deallocate();
		}
		tmp.clear();

		delete __globals_internal::globalallocators;
	}

	void sort_initializers()
	{
		MFW_MESSAGE("TODO need to figure this out")

		using list_t = unordered_map<ucstring, vector<interfaces::global_initializer *>>;
		list_t list{};

		list_t::iterator empty_it{list.emplace(list_t::value_type{{}, {}}).first};

		__globals_internal::globalinitializers_vector_t &tmp{*__globals_internal::globalinitializers};
		for(interfaces::global_initializer *init : tmp) {
			const ucstring &name{init->name()};
			const vector<ucstring> &depends{init->depends()};
			if(depends.empty()) {
				empty_it->second.emplace_back(init);
			} else {
				for(const ucstring &dep : depends) {
					list_t::iterator it{list.find(dep)};
					if(it == list.end()) {
						it = list.emplace(list_t::value_type{dep, {}}).first;
					}
					it->second.emplace_back(init);
				}
			}
		}

		tmp.clear();

		for(interfaces::global_initializer *init : empty_it->second) {
			tmp.emplace_back(init);
		}

		list.erase(empty_it);

		for(const list_t::value_type &it : list) {
			for(interfaces::global_initializer *init : it.second) {
				tmp.emplace_back(init);
			}
		}
	}

	exit_status initialize_all_globals()
	{
		return __globals_internal::execute_func_all_globals(&interfaces::global_initializer::initialize);
	}

	exit_status update_all_globals()
	{
		return __globals_internal::execute_func_all_globals(&interfaces::global_initializer::update);
	}

	exit_status shutdown_all_globals()
	{
		return __globals_internal::execute_func_all_globals(&interfaces::global_initializer::shutdown);
	}
}