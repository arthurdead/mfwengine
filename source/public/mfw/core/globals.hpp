#ifndef __MFW_PUBLIC_CORE_GLOBALS_HPP
#define __MFW_PUBLIC_CORE_GLOBALS_HPP

#pragma once

#include <public/mfw/core/core.hpp>
#include <public/mfw/core/application.hpp>
#include <public/mfw/stl/memory.hpp>
#include <public/mfw/stl/defines.hpp>

namespace mfw::core
{
	class MFW_VISIBILITY_PUBLIC GlobalAllocator
	{
		template <typename T>
		friend class GlobalAllocatorImpl;

		MFW_CORE_API MFW_CORE_CALL GlobalAllocator() noexcept;
		virtual MFW_CORE_API MFW_CORE_CALL ~GlobalAllocator() noexcept;

	public:
		virtual void allocate() noexcept = 0;
		virtual void deallocate() noexcept = 0;
	};

	class MFW_VISIBILITY_PUBLIC GlobalInitializer
	{
	protected:
		enum class init_type_t : stl::uchar_t
		{
			init = MFW_BIT(0),
			update = MFW_BIT(1),
			shutdown = MFW_BIT(2),
		};
		MFW_CLASS_ENUM_FLAGS(init_type_t)

		MFW_CORE_API MFW_CORE_CALL GlobalInitializer(init_type_t type) noexcept;
		virtual MFW_CORE_API MFW_CORE_CALL ~GlobalInitializer() noexcept;

		MFW_VISIBILITY_LOCAL GlobalInitializer(init_type_t type, stl::osstring_view name) noexcept
			: GlobalInitializer{type} { m_name.assign(name); }
		MFW_VISIBILITY_LOCAL GlobalInitializer(init_type_t type, stl::osstring_view name, stl::initializer_list<stl::osstring_view> depends) noexcept
			: GlobalInitializer{type, name} { m_depends.assign(depends.begin(), depends.end()); }

	public:
		virtual ExitStatus initialize() noexcept
		{ return ExitStatus::success; }
		virtual ExitStatus update() noexcept
		{ return ExitStatus::success; }
		virtual ExitStatus shutdown() noexcept
		{ return ExitStatus::success; }

		MFW_VISIBILITY_LOCAL const stl::osstring &name() const noexcept
		{ return m_name; }
		MFW_VISIBILITY_LOCAL const stl::vector<stl::osstring> &depends() const noexcept
		{ return m_depends; }

	private:
		stl::osstring m_name{};
		stl::vector<stl::osstring> m_depends{};
	};

	template <typename T>
	class MFW_VISIBILITY_LOCAL GlobalAllocatorImpl final : private GlobalAllocator
	{
	public:
		T &instance() noexcept {
			allocate();
			return *pointer;
		}

		GlobalAllocatorImpl() noexcept
			: GlobalAllocator{} {}
		
		~GlobalAllocatorImpl() noexcept override
		{ deallocate(); }

	private:
		void allocate() noexcept override {
			if(!pointer) {
				pointer = new T{};
			}
		}
		
		void deallocate() noexcept override {
			if(pointer) {
				delete pointer;
			}
			pointer = nullptr;
		}

		T *pointer{nullptr};
	};

	#define MFW_DECLARE_GLOBAL_ALLOCATOR(name, type) \
		static MFW_VISIBILITY_LOCAL ::mfw::core::GlobalAllocatorImpl<type> MFW_MACRO_CONCATENATE(MFW_MACRO_CONCATENATE(__, name), _global_allocator){};

	#define MFW_DECLARE_GLOBAL_CLASS_FUNCTION(globalname, classname, classfuncname) \
		template <typename ...Args> \
		MFW_VISIBILITY_LOCAL auto globalname(Args &&... args) noexcept { \
			return classname::instance().classfuncname(forward<Args>(args)...); \
		}
}

#endif