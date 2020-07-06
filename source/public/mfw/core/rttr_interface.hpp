#ifndef __MFW_PUBLIC_CORE_RTTR_INTERFACE_HPP
#define __MFW_PUBLIC_CORE_RTTR_INTERFACE_HPP

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

	class func_info
	{
	public:
		template <typename T>
		void deduce(T func);

		func_info() = default;

		func_info(const ucstring_view &name_) : name{name_} {}

		template <typename T>
		func_info(T func, const ucstring_view &name_) { deduce(func); set_name(name_); }

		MFW_CORE_API void MFW_CORE_CALL clear();

		void set_name(const ucstring_view &name_) { name = name_; }
		const ucstring &get_name() const { return name; }

		template <typename R, typename ...Args>
		void set_funcptr(R (*func)(Args...)) { funcptr = force_cast<void *>(func); }
		template <typename R, typename T, typename ...Args>
		void set_funcptr(R (T::*func)(Args...)) {
			set_this_info(type_identity<T>{});
			funcptr = force_cast<void *>(func);
		}

		template <typename T = void *>
		T get_funcptr() const { return force_cast<T>(funcptr); }

		template <typename T>
		void set_return_info(type_identity<T>) { return_info.deduce(type_identity<T>{}); }
		const type_info &get_return_info() const { return return_info; }

		template <typename T>
		void set_this_info(type_identity<T>) { this_info.deduce(type_identity<T>{}); }
		const type_info &get_this_info() const { return this_info; }

		size_t arg_num() const { return arg_infos.size(); }
		const vector<type_info> &get_args() const { return arg_infos; }
		const type_info &get_arg(size_t i) const { return arg_infos[i]; }

		template <typename T>
		void add_arg_info(type_identity<T>);

		template <typename T>
		const T &get_extrainfo() const { return *reinterpret_cast<const T *>(extrainfo); }
		template <typename T>
		void set_extrainfo(const T &extra) { extrainfo = reinterpret_cast<void *>(const_cast<T *>(&extra)); }

	protected:
		template <typename T>
		T &get_extrainfo() { return *reinterpret_cast<T *>(extrainfo); }

	private:
		template <typename T>
		class function_traits;

		#define __MFW_DECLARE_MEMBER_FUNCTRAITS(callconv, cnst, ref, execpt) \
			template <typename R, typename T, typename ...Args> \
			class function_traits<R (callconv T::*)(Args...) cnst ref execpt> \
			{ \
			public: \
				using func_t = R (callconv T::*)(Args...) cnst; \
				using this_t = T; \
				using ret_t = R; \
				using args_t = tuple<Args...>; \
				static inline constexpr size_t args_num{sizeof...(Args)}; \
			};

		#define __MFW_DECLARE_STATIC_FUNCTRAITS(callconv, execpt) \
			template <typename R, typename ...Args> \
			class function_traits<R (callconv *)(Args...) execpt> \
			{ \
			public: \
				using func_t = R (callconv *)(Args...); \
				using this_t = void; \
				using ret_t = R; \
				using args_t = tuple<Args...>; \
				static inline constexpr size_t args_num{sizeof...(Args)}; \
			};

		__MFW_DECLARE_MEMBER_FUNCTRAITS(MFW_NOTHING, MFW_NOTHING, MFW_NOTHING, MFW_NOTHING)
		__MFW_DECLARE_MEMBER_FUNCTRAITS(MFW_NOTHING, MFW_NOTHING, &, MFW_NOTHING)
		__MFW_DECLARE_MEMBER_FUNCTRAITS(MFW_NOTHING, MFW_NOTHING, &&, MFW_NOTHING)
		__MFW_DECLARE_MEMBER_FUNCTRAITS(MFW_NOTHING, const, MFW_NOTHING, MFW_NOTHING)
		__MFW_DECLARE_MEMBER_FUNCTRAITS(MFW_NOTHING, const, &, MFW_NOTHING)

		__MFW_DECLARE_STATIC_FUNCTRAITS(MFW_NOTHING, MFW_NOTHING)

		template <size_t i, size_t s, typename T>
		void add_arg_info_helper();

		ucstring name{};
		type_info this_info{};
		vector<type_info> arg_infos{};
		type_info return_info{};
		void *funcptr{nullptr};
		size_t virtualindex{static_cast<size_t>(-1)};
		void *extrainfo{nullptr};
	};

	class class_info : public type_info
	{
	public:
		using super = type_info;

		class_info() = default;
		class_info(const ucstring_view &_name_) : name{_name_} {}

		MFW_CORE_API void MFW_CORE_CALL clear();

		template <typename T>
		void deduce(type_identity<T>);

		void set_name(const ucstring_view &_name_) { name = _name_; }
		const ucstring &get_name() const { return name; }

		MFW_CORE_API const func_info * MFW_CORE_CALL get_function(const ucstring_view &name_) const;
		const vector<func_info> &get_functions() const { return functions; }

		class member_variable_info : public type_info
		{
		public:
			MFW_CORE_API void MFW_CORE_CALL clear();

			template <typename T, typename V>
			void deduce(V T::*var_);

			void set_name(const ucstring_view &_name_) { name = _name_; }
			const ucstring &get_name() const { return name; }

			MFW_CORE_API void MFW_CORE_CALL set(type_holder &obj, const type_holder &val) const;
			MFW_CORE_API void MFW_CORE_CALL get(const type_holder &obj, type_holder &val) const;
			MFW_CORE_API void MFW_CORE_CALL get_ptr(const type_holder &obj, type_holder &val) const;

		private:
			template <typename T, typename V>
			static void __set_helper(T *ptr, V T::*var, const V &value) { ptr->*var = value; }

			template <typename T, typename V>
			static void __get_helper(T *ptr, V T::*var, V &value) { value = ptr->*var; }

			ucstring name{};
			size_t offset{0};
			void *var{nullptr};
			void *set_func{nullptr};
			void *get_func{nullptr};
		};

		MFW_CORE_API const member_variable_info * MFW_CORE_CALL get_variable(const ucstring_view &name_) const;
		const vector<member_variable_info> &get_variables() const { return variables; }

	protected:
		vector<func_info> &get_functions() { return functions; }

		template <typename T>
		class __internal_funcs_helper
		{
		public:
			void destructor() { reinterpret_cast<T *>(this)->~T(); }

			#pragma push_macro("new")
			#undef new

			template <typename ...Args>
			T *counstructor(Args &&... args) { return new(reinterpret_cast<T *>(this)) T{forward<Args>(args)...}; }

			#pragma pop_macro("new")
		};

		template <typename T, typename ...Args>
		bool counstructor(type_identity<T>, type_identity_multiple<Args...>);

		template <typename T>
		bool destructor(type_identity<T>);

		template <typename V, typename T>
		bool variable(V T::*var, const ucstring_view &name);

		template <typename R, typename T, typename ...Args>
		bool function(R (T::*func)(Args...), const ucstring_view &name);

		ucstring name{};
		vector<member_variable_info> variables{};
		vector<func_info> functions{};
	};

	template <typename T>
	class class_infoT : public class_info
	{
	public:
		using super = class_info;

		void deduce() { super::deduce(type_identity<T>{}); }

		class_infoT() { deduce(); }

		template <typename ...Args>
		bool counstructor()
		{ return super::counstructor(type_identity<T>{}, type_identity_multiple<Args...>{}); }

		bool destructor()
		{ return super::destructor(type_identity<T>{}); }

		template <typename V>
		bool variable(V T::*var, const ucstring_view &_name_)
		{ return super::variable(var, _name_); }

		template <typename R, typename ...Args>
		bool function(R (T::*func)(Args...), const ucstring_view &_name_)
		{ return super::function(func, _name_); }
	};

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