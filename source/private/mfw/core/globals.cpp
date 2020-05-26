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