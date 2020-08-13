#ifndef MFW_PUBLIC_CORE_RTTR_INTERFACE_HPP
#define MFW_PUBLIC_CORE_RTTR_INTERFACE_HPP

#pragma once

#include <public/mfw/core/core.hpp>
#include <public/mfw/core/globals.hpp>
#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/tuple.hpp>
#include <public/mfw/stl/type_traits.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/core/univalue.hpp>

#include <public/mfw/core/internal/typeinfo.hpp>

namespace mfw::core
{
	class ClassInfo;
	class FuncInfo;
	class TypeHolder;
	class ClassInfoFinder;

	

	namespace interfaces
	{
		class rttr
		{
		protected:
			virtual ~rttr() = default;
			
		public:
			static MFW_CORE_API rttr & MFW_CORE_CALL instance();

			virtual bool register_class_info(const class_info &info) = 0;
			virtual bool register_func_info(const func_info &info) = 0;

			virtual const class_info *find_class_info(const ucstring_view &name) = 0;
			virtual const func_info *find_function_info(const ucstring_view &name) const = 0;

			virtual const ucstring &clean_name(const ::std::type_info &info) const = 0;

			virtual bool call_function(const func_info &info, const vector<type_holder> &args, type_holder &result) const = 0;

			enum class internal_func : uchar_t
			{
				default_ctor,
				copy_ctor,
				move_ctor,
				copy_assign,
				move_assign,
				dtor,
			};

			virtual bool call_internal_function(internal_func which, void *dst, const void *src, const class_info &info) const = 0;

			virtual void *allocate(const class_info &info) const = 0;
			virtual bool deallocate(void *ptr, const class_info &info) const = 0;
		};
	}

	

	class type_holder
	{
	public:
		MFW_CORE_API void MFW_CORE_CALL clear();

		template <typename T>
		void deduce(type_identity<T>);
		void deduce(const class_info &_info) { deduce(static_cast<const type_info &>(_info)); }
		void deduce(const class_info::member_variable_info &_info) { deduce(static_cast<const type_info &>(_info)); }
		void deduce(const type_info &_info);
		template <typename T>
		void deduce(const T &var);
		template <typename T>
		void deduce(const class_info &_info, const T &var);
		template <typename T>
		void deduce(const type_info &_info, const T &var);

		type_holder() = default;
		type_holder(const type_holder &other) { operator=(other); }
		type_holder(type_holder &&) = default;
		MFW_CORE_API type_holder & MFW_CORE_CALL operator=(const type_holder &other);
		type_holder &operator=(type_holder &&other) = default;
		~type_holder() { call_dtor(); }

		template <typename ...Args>
		type_holder(Args &&... args) { deduce(forward<Args>(args)...); }

		template <typename T>
		T &get_var() { return *reinterpret_cast<T *>(memory_.get()); }
		template <typename T>
		const T &get_var() const { return *reinterpret_cast<const T *>(memory_.get()); }

		template <typename T>
		T *get_var_ptr() { return info_.is_pointer() ? get_var<T *>() : &get_var<T>(); }
		template <typename T>
		const T *get_var_ptr() const { return info_.is_pointer() ? get_var<T *>() : &get_var<T>(); }

		template <typename T>
		T convert() const;

		template <typename T>
		bool set_var(const T &var);
		MFW_CORE_API bool MFW_CORE_CALL set_var(const void *var);
		MFW_CORE_API bool MFW_CORE_CALL set_var_raw(const void *var);
		MFW_CORE_API void MFW_CORE_CALL unset_var();

		void *memory() { return memory_.get(); }
		const void *memory() const { return memory_.get(); }
		const type_info &info() const { return info_; }

		bool valid() const { return info_.valid(); }

	private:
		void allocate() { memory_.reset(new byte[info_.size()]{}); }

		bool call_default_ctor();
		bool call_copy_ctor(const void *var);
		MFW_CORE_API void MFW_CORE_CALL call_dtor();

		type_info info_{};
		unique_ptr<byte[]> memory_{};
		bool ctor_called{false};
	};

	#define MFW_RTTR_BEGIN_CLASS(name) \
		class __##name##_rttr_register { \
		public: \
			__##name##_rttr_register() { \
				using T = name; \
				::mfw::core::class_infoT<T> info{}; \

	#define MFW_RTTR_END_CLASS(name) \
				::mfw::core::rttr::instance().register_class_info(info); \
			} \
		}; \
		MFW_DECLARE_GLOBAL_ALLOCATOR(name, __##name##_rttr_register)

	#define MFW_RTTR_MEMBER_VARIABLE(name) \
		info.variable(&T::name, u## #name ##_sv);

	#define MFW_RTTR_MEMBER_FUNCTION(name) \
		info.function(&T::name, u## #name ##_sv);

	#define MFW_RTTR_FUNCTION(name) \
		class __##name##_rttr_register { \
		public: \
			__##name##_rttr_register() { \
				::mfw::core::func_info info{name, u## #name ##_sv}; \
				::mfw::core::rttr::instance().register_func_info(info); \
			} \
		}; \
		MFW_DECLARE_GLOBAL_ALLOCATOR(name, __##name##_rttr_register)

	class univalue_var : public univalue
	{
	public:
		bool is_var() const noexcept
		{ return (!is_string() && !is_bool()); }

		const type_holder &get_var() const noexcept
		{ return var; }

		template <typename T>
		void set(const T &value) noexcept
		{ set_var(value); }

		void set(const type_holder &value) noexcept
		{ set_var(value); }

		void set_var(const type_holder &value) noexcept
		{ var = value; }

		template <typename T>
		void set_var(const T &value) noexcept
		{ var.deduce(value); }

	private:
		type_holder var{};
	};
}

#include <public/mfw/core/rttr_interface.inl>

#endif