#ifndef __MFW_PUBLIC_CORE_RTTR_INTERFACE_H
#define __MFW_PUBLIC_CORE_RTTR_INTERFACE_H

#pragma once

#include <public/mfw/core/core.hpp>
#include <public/mfw/core/globals.hpp>
#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/tuple.hpp>
#include <public/mfw/stl/type_traits.hpp>
#include <public/mfw/stl/vector.hpp>

namespace mfw::core
{
	class class_info;
	class func_info;
	class type_holder;
	class class_info_finder;

	class type_info
	{
	public:
		template <typename T>
		void deduce(type_identity<T>);

		type_info() = default;

		template <typename T>
		type_info(type_identity<T>) { deduce(type_identity<T>{}); }

		MFW_CORE_API void MFW_CORE_CALL clear();

		bool valid() const { return !name_.empty() && std_info_; }

		size_t size() const { return size_; }
		size_t align() const { return align_; }
		const ::std::type_info &std_info() const { return (std_info_ ? *std_info_ : get_typeid<void>()); }

		bool is_exact(const type_info &info) const { return std_info() == info.std_info(); }
		bool is_exact(const ::std::type_info &info) const { return std_info() == info; }

		template <typename T>
		bool is_relaxed() const;

		template <typename T>
		bool is_exact() const;

	#if MFW_COMPILER == MFW_COMPILER_MSVC
		bool is_int8() const { return is_relaxed<__int8>() || is_relaxed<char>() || is_exact<bool>() || is_exact<char8_t>(); }
		bool is_int16() const { return is_relaxed<__int16>() || is_relaxed<short>() || is_exact<wchar_t>() || is_exact<char16_t>(); }
		bool is_int32() const { return is_relaxed<__int32>() || is_relaxed<int>() || is_relaxed<long>() || is_exact<char32_t>(); }
		bool is_int64() const { return is_relaxed<__int64>() || is_relaxed<long long>(); }
		bool is_int128() const { return is_int64(); }
		bool is_float16() const { return is_int32(); }
		bool is_float32() const { return is_exact<float>(); }
		bool is_float64() const { return is_exact<double>() || is_exact<long double>(); }
		bool is_float80() const { return is_float64(); }
		bool is_float128() const { return is_float80(); }
	#elif MFW_COMPILER & MFW_COMPILER_UNIX_FLAG
		bool is_int8() const { return is_relaxed<char>() || is_exact<bool>() || is_exact<char8_t>(); }
		bool is_int16() const { return is_relaxed<short>() || is_exact<char16_t>(); }
		bool is_int32() const { return is_exact<wchar_t>() || is_relaxed<int>() || is_exact<char32_t>(); }
		bool is_int64() const { return is_relaxed<long>() || is_relaxed<long long>(); }
		bool is_int128() const { return is_int64(); }
		bool is_float16() const { return is_float32(); }
		bool is_float32() const { return is_exact<float>(); }
		bool is_float64() const { return is_exact<double>() || is_exact<long double>(); }
		bool is_float80() const { return is_float64(); }
		bool is_float128() const { return is_float80(); }
	#else
		#error
	#endif

		bool is_exact(const ucstring_view &name) const { return name_ == name; }

		bool is_void() const { return is_exact<void>(); }
		bool is_primitive() const { return !bool_cast(flags_ & flags::class_) || (is_void() || is_any_int() || is_any_float()); }
		bool is_class() const { return bool_cast(flags_ & flags::class_) || !is_primitive(); }
		bool is_rvalue_ref() const { return bool_cast(flags_ & flags::rvalue_ref); }
		bool is_const() const { return bool_cast(flags_ & flags::const_); }
		bool is_lvalue_ref() const { return bool_cast(flags_ & flags::lvalue_ref); }
		bool is_signed() const { return bool_cast(flags_ & flags::signed_); }
		bool is_unsigned() const { return bool_cast(flags_ & flags::unsigned_); }
		bool is_array() const { return bool_cast(flags_ & flags::array_) || rank() > 0; }
		bool is_pointer() const { return bool_cast(flags_ & flags::pointer_) || num_pointers() > 0; }
		size_t rank() const { return rank_; }
		size_t extent() const { return extent_; }
		size_t num_pointers() const { return pointers; }
		bool is_any_int() const { return is_int8() || is_int16() || is_int32() || is_int64() || is_int128(); }
		bool is_any_float() const { return is_float16() || is_float32() || is_float64() || is_float80() || is_float128(); }
		bool is_any_reference() const { return is_rvalue_ref() || is_lvalue_ref(); }
		bool is_ptr_like() const { return is_pointer() || is_array(); }
		const ucstring &name() const { return name_; }

		MFW_CORE_API const class_info * MFW_CORE_CALL find_class_info() const;

		MFW_CORE_API void MFW_CORE_CALL make_pointer();

	private:
		MFW_CORE_API void MFW_CORE_CALL set_name();

		enum class modify_type_flags : int16_t
		{
			none,
			remove_const = MFW_BIT(0),
			remove_reference = MFW_BIT(1),
			remove_pointer = MFW_BIT(2),
			remove_all_extents = MFW_BIT(3),

			add_lvalue_reference = MFW_BIT(4),
			add_rvalue_reference = MFW_BIT(5),
			add_pointer = MFW_BIT(6),
			add_const = MFW_BIT(7),
			make_signed = MFW_BIT(8),
			make_unsigned = MFW_BIT(9),

			remove_all = remove_const|remove_reference|remove_pointer|remove_all_extents,
			remove_all_but_const = remove_all & ~remove_const,
			remove_all_but_reference = remove_all & ~remove_reference,
			remove_all_but_pointer = remove_all & ~remove_pointer,
			remove_all_but_extents = remove_all & ~remove_all_extents,
		};
		MFW_CLASS_ENUM_FLAGS_V1(modify_type_flags)

		struct modify_type
		{
			template <typename T, modify_type_flags flags>
			static constexpr decltype(auto) __modify_type_helper();

			template <typename T, modify_type_flags flags>
			using type = remove_reference_t<decltype(__modify_type_helper<T, flags>())>;
		};

		template <typename T, modify_type_flags flags>
		using modify_type_t = typename modify_type::type<T, flags>;

		enum class flags : int16_t
		{
			none,
			const_ = MFW_BIT(0),
			lvalue_ref = MFW_BIT(1),
			rvalue_ref = MFW_BIT(2),
			signed_ = MFW_BIT(3),
			unsigned_ = MFW_BIT(4),
			pointer_ = MFW_BIT(5),
			array_ = MFW_BIT(6),
			class_ = MFW_BIT(7),
		};
		MFW_CLASS_ENUM_FLAGS(flags)

		size_t rank_{0};
		size_t extent_{0};
		size_t pointers{0};

		flags flags_{flags::none};
		size_t size_{0};
		size_t align_{0};
		const ::std::type_info *std_info_{nullptr};
		ucstring name_{};
	};

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
}

#include <public/mfw/core/rttr_interface.inl>

#endif