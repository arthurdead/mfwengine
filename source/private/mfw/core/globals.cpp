#include <public/mfw/core/globals.hpp>
#include <public/mfw/core/application.hpp>
#include <private/mfw/core/globals_internal.hpp>
#include <public/mfw/stl/memory.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/stl/array.hpp>
#include <public/mfw/stl/unordered_map.hpp>

namespace mfw::core
{
	namespace __private_globals_cpp_internal MFW_VISIBILITY_LOCAL
	{
		enum /*class*/ init_indexes_t : stl::uchar_t
		{
			init,
			update,
			shutdown,
			count,
		};

		using globalallocators_vector_t = stl::vector<GlobalAllocator *>;
		globalallocators_vector_t *globalallocators{nullptr};

		using globalinitializers_vector_t = stl::vector<GlobalInitializer *>;
		stl::array<globalinitializers_vector_t *, init_indexes_t::count> globalinitializers{};

		template <typename T>
		static ExitStatus executeFuncAllGlobals(init_indexes_t num, T func) noexcept
		{
			ExitStatus status{};

			globalinitializers_vector_t *tmp{globalinitializers[num]};
			if(!tmp) {
				return status;
			}

			for(GlobalInitializer *it : *tmp) {
				status += (it->*func)();
				if(!status.succeded()) {
					return status;
				}
			}

			return status;
		}

		void _sortInitializers_impl(init_indexes_t num) noexcept
		{
			MFW_MESSAGE("TODO need to figure this out")

			globalinitializers_vector_t *tmp{globalinitializers[num]};
			if(!tmp) {
				return;
			}

			using list_t = stl::unordered_map<stl::osstring, stl::vector<GlobalInitializer *>>;
			list_t list{};

			list_t::iterator empty_it{list.emplace(list_t::value_type{{}, {}}).first};

			for(GlobalInitializer *init : *tmp) {
				const stl::osstring &name{init->name()};
				const stl::vector<stl::osstring> &depends{init->depends()};
				if(depends.empty()) {
					empty_it->second.emplace_back(init);
				} else {
					for(const stl::osstring &dep : depends) {
						list_t::iterator it{list.find(dep)};
						if(it == list.end()) {
							it = list.emplace(list_t::value_type{dep, {}}).first;
						}
						it->second.emplace_back(init);
					}
				}
			}

			tmp->clear();

			for(GlobalInitializer *init : empty_it->second) {
				tmp->emplace_back(init);
			}

			list.erase(empty_it);

			for(const list_t::value_type &it : list) {
				for(GlobalInitializer *init : it.second) {
					tmp->emplace_back(init);
				}
			}
		}
	}

	MFW_CORE_API MFW_CORE_CALL GlobalAllocator::GlobalAllocator() noexcept
	{
		using __private_globals_cpp_internal::globalallocators_vector_t;
		using __private_globals_cpp_internal::globalallocators;

		globalallocators_vector_t *&tmp{globalallocators};
		if(!tmp) {
			tmp = new globalallocators_vector_t{};
		}
		tmp->emplace_back(this);
	}

	MFW_CORE_API MFW_CORE_CALL GlobalAllocator::~GlobalAllocator() noexcept
	{
		//globalallocators_vector_t &tmp{*__globalallocators};
		//tmp.erase(::std::remove(tmp.begin(), tmp.end(), this), tmp.end());
	}

	MFW_CORE_API MFW_CORE_CALL GlobalInitializer::GlobalInitializer(init_type_t type) noexcept
	{
		using __private_globals_cpp_internal::globalinitializers_vector_t;
		using __private_globals_cpp_internal::globalinitializers;
		using __private_globals_cpp_internal::init_indexes_t;

		if(bool_cast(type & init_type_t::init)) {
			globalinitializers_vector_t *&tmp{globalinitializers[init_indexes_t::init]};
			if(!tmp) {
				tmp = new globalinitializers_vector_t{};
			}
			tmp->emplace_back(this);
		}
		if(bool_cast(type & init_type_t::update)) {
			globalinitializers_vector_t *&tmp{globalinitializers[init_indexes_t::update]};
			if(!tmp) {
				tmp = new globalinitializers_vector_t{};
			}
			tmp->emplace_back(this);
		}
		if(bool_cast(type & init_type_t::shutdown)) {
			globalinitializers_vector_t *&tmp{globalinitializers[init_indexes_t::shutdown]};
			if(!tmp) {
				tmp = new globalinitializers_vector_t{};
			}
			tmp->emplace_back(this);
		}
	}

	MFW_CORE_API MFW_CORE_CALL GlobalInitializer::~GlobalInitializer()
	{
		//globalinitializers_vector_t &tmp{*__globalinitializers};
		//tmp.erase(::std::remove(tmp.begin(), tmp.end(), this), tmp.end());
	}

	MFW_VISIBILITY_LOCAL void allocateAllGlobals() noexcept
	{
		using __private_globals_cpp_internal::globalallocators_vector_t;
		using __private_globals_cpp_internal::globalallocators;

		if(!globalallocators) {
			return;
		}

		globalallocators_vector_t &tmp{*globalallocators};
		for(GlobalAllocator *it : tmp) {
			it->allocate();
		}
	}

	MFW_VISIBILITY_LOCAL void deallocateAllGlobals() noexcept
	{
		using __private_globals_cpp_internal::globalallocators_vector_t;
		using __private_globals_cpp_internal::globalallocators;

		if(!globalallocators) {
			return;
		}

		globalallocators_vector_t &tmp{*globalallocators};
		for(GlobalAllocator *it : tmp) {
			it->deallocate();
		}
		tmp.clear();

		delete globalallocators;
	}

	MFW_VISIBILITY_LOCAL void sortInitializers() noexcept
	{
		using __private_globals_cpp_internal::_sortInitializers_impl;
		using __private_globals_cpp_internal::init_indexes_t;

		_sortInitializers_impl(init_indexes_t::init);
		_sortInitializers_impl(init_indexes_t::update);
		_sortInitializers_impl(init_indexes_t::shutdown);
	}

	MFW_VISIBILITY_LOCAL ExitStatus initializeAllGlobals() noexcept
	{
		using __private_globals_cpp_internal::executeFuncAllGlobals;
		using __private_globals_cpp_internal::init_indexes_t;

		return executeFuncAllGlobals(init_indexes_t::init, &GlobalInitializer::initialize);
	}

	MFW_VISIBILITY_LOCAL ExitStatus updateAllGlobals() noexcept
	{
		using __private_globals_cpp_internal::executeFuncAllGlobals;
		using __private_globals_cpp_internal::init_indexes_t;

		return executeFuncAllGlobals(init_indexes_t::update, &GlobalInitializer::update);
	}

	MFW_VISIBILITY_LOCAL ExitStatus shutdownAllGlobals() noexcept
	{
		using __private_globals_cpp_internal::executeFuncAllGlobals;
		using __private_globals_cpp_internal::init_indexes_t;

		return executeFuncAllGlobals(init_indexes_t::shutdown, &GlobalInitializer::shutdown);
	}
}