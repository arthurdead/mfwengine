#ifndef MFW_PUBLIC_CORE_GLOBALS_HPP
#define MFW_PUBLIC_CORE_GLOBALS_HPP

#pragma once

#include <public/mfw/core/core.hpp>
#include <public/mfw/core/application.hpp>
#include <public/mfw/stl/memory.hpp>
#include <public/mfw/stl/defines.hpp>

namespace mfw::core
{
	class MFW_VISIBILITY_PUBLIC global_allocator
	{
		template <typename T>
		friend class global_allocator_impl;

		MFW_CORE_API MFW_CORE_CALL global_allocator() noexcept;
		virtual MFW_CORE_API MFW_CORE_CALL ~global_allocator() noexcept;

	public:
		virtual void allocate() noexcept = 0;
		virtual void deallocate() noexcept = 0;
	};

	class MFW_VISIBILITY_PUBLIC global_initalizer
	{
		using string_view_type = stl::osstring_view;
		using string_type = stl::osstring;

	protected:
		enum class init_type : stl::uchar_t
		{
			init = MFW_BIT(0),
			update = MFW_BIT(1),
			shutdown = MFW_BIT(2),
		};
		MFW_CLASS_ENUM_FLAGS(init_type)

		MFW_CORE_API MFW_CORE_CALL global_initalizer(init_type type) noexcept;
		virtual MFW_CORE_API MFW_CORE_CALL ~global_initalizer() noexcept;

		MFW_VISIBILITY_LOCAL global_initalizer(init_type type, string_view_type name) noexcept
			: global_initalizer{type} { m_name.assign(name); }
		MFW_VISIBILITY_LOCAL global_initalizer(init_type type, string_type &&name) noexcept
			: global_initalizer{type} { m_name.assign(stl::move(name)); }
		MFW_VISIBILITY_LOCAL global_initalizer(init_type type, string_view_type name, stl::initializer_list<string_view_type> depends) noexcept
			: global_initalizer{type, name} { m_depends.assign(depends.begin(), depends.end()); }
		MFW_VISIBILITY_LOCAL global_initalizer(init_type type, string_type &&name, stl::initializer_list<string_view_type> depends) noexcept
			: global_initalizer{type, stl::move(name)} { m_depends.assign(depends.begin(), depends.end()); }

	public:
		MFW_VISIBILITY_LOCAL virtual exit_status initialize() noexcept
		{ return exit_status::success; }
		MFW_VISIBILITY_LOCAL virtual exit_status update() noexcept
		{ return exit_status::success; }
		MFW_VISIBILITY_LOCAL virtual exit_status shutdown() noexcept
		{ return exit_status::success; }

		MFW_VISIBILITY_LOCAL const string_type &name() const noexcept
		{ return m_name; }
		MFW_VISIBILITY_LOCAL const stl::vector<string_type> &depends() const noexcept
		{ return m_depends; }

	private:
		string_type m_name{};
		stl::vector<string_type> m_depends{};
	};

	template <typename T>
	class MFW_VISIBILITY_LOCAL global_allocator_impl final : private global_allocator
	{
	public:
		T &instance() noexcept {
			allocate();
			return *pointer;
		}

		global_allocator_impl() noexcept
			: global_allocator{} {}
		
		~global_allocator_impl() noexcept override
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
		static MFW_VISIBILITY_LOCAL ::mfw::core::global_allocator_impl<type> MFW_MACRO_CONCATENATE(MFW_MACRO_CONCATENATE(__, name), _global_allocator){};

	#define MFW_DECLARE_GLOBAL_CLASS_FUNCTION(globalname, classname, classfuncname) \
		template <typename ...Args> \
		MFW_VISIBILITY_LOCAL auto globalname(Args &&... args) noexcept { \
			return classname::instance().classfuncname(forward<Args>(args)...); \
		}
}

#endif