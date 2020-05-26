#ifndef __MFW_PUBLIC_CORE_GLOBALS_H
#define __MFW_PUBLIC_CORE_GLOBALS_H

#pragma once

#include <public/mfw/core/core.hpp>
#include <public/mfw/core/application.hpp>
#include <public/mfw/stl/memory.hpp>
#include <public/mfw/stl/defines.hpp>

namespace mfw::core
{
	namespace interfaces
	{
		class global_allocator
		{
		public:
			MFW_CORE_API MFW_CORE_CALL global_allocator();
			virtual MFW_CORE_API MFW_CORE_CALL ~global_allocator();

			virtual void allocate() = 0;
			virtual void deallocate() = 0;
		};

		class global_initializer
		{
		public:
			MFW_CORE_API MFW_CORE_CALL global_initializer();
			virtual MFW_CORE_API MFW_CORE_CALL ~global_initializer();

			virtual exit_status initialize() = 0;
			virtual exit_status update() = 0;
			virtual exit_status shutdown() = 0;
		};
	}

	template <typename T>
	class global_allocator final : interfaces::global_allocator
	{
	public:
		T &instance() {
			allocate();
			return *pointer;
		}
		
		~global_allocator() {
			deallocate();
		}

	private:
		void allocate()
		{
			if(!pointer) {
				pointer = new T{};
			}
		}
		
		void deallocate()
		{
			if(pointer) {
				delete pointer;
			}
			pointer = nullptr;
		}

		T *pointer{nullptr};
	};

	#define MFW_DECLARE_GLOBAL_ALLOCATOR(name, type) \
		static ::mfw::core::global_allocator<type> MFW_MACRO_CONCATENATE(MFW_MACRO_CONCATENATE(__, name), _global_allocator){};

	#define MFW_DECLARE_GLOBAL_CLASS_FUNCTION(globalname, classname, classfuncname) \
		template <typename ...Args> \
		auto globalname(Args &&... args) { \
			return classname::instance().classfuncname(forward<Args>(args)...); \
		}
}

#endif