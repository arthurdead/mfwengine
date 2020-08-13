namespace mfw::core
{
	template <typename T>
	class_info &class_info::deduce(type_identity<T>) noexcept
	{
		clear();
		super::deduce(type_identity<T>{});
		counstructor(type_identity<T>{}, type_identity_multiple<>{});
		counstructor(type_identity<T>{}, type_identity_multiple<const T &>{});
		counstructor(type_identity<T>{}, type_identity_multiple<T &&>{});
		function(overload_cast_member(T &, MFW_NOTHING, T, operator=, MFW_NOTHING, const T &), MFW_T("operator="_sv));
		function(overload_cast_member(T &, MFW_NOTHING, T, operator=, MFW_NOTHING, T &&), MFW_T("operator="_sv));
		destructor(type_identity<T>{});
		m_name.assign(interfaces::rttr::instance().clean_name(get_typeid<T>()));
		return *this;
	}

	template <typename T, typename V>
	void class_info::member_variable_info::set_helper(T *ptr, V T::*var, const V &value) noexcept
	{ ptr->*var = value; }

	template <typename T, typename V>
	void class_info::member_variable_info::get_helper(T *ptr, V T::*var, V &value) noexcept
	{ value = ptr->*var; }

	template <typename T, typename V>
	class_info::member_variable_info &class_info::member_variable_info::deduce(V T:: *var) noexcept
	{
		clear();
		super::deduce(type_identity<V>{});
		m_offset = stl::var_offset(var);
		m_var = stl::force_cast<void *>(var);
		m_set_func = stl::force_cast<void *>(set_helper<T, V>);
		m_get_func = stl::force_cast<void *>(get_helper<T, V>);
		return *this;
	}

	template <typename T>
	void class_info::dtor_ctor_helper<T>::dtor() noexcept
	{ reinterpret_cast<T *>(this)->~T(); }

	#pragma push_macro("new")
	#undef new

	template <typename T>
	template <typename ...Args>
	T *class_info::dtor_ctor_helper<T>::ctor(Args &&... args) noexcept
	{ return new(reinterpret_cast<T *>(this)) T{forward<Args>(args)...}; }

	#pragma pop_macro("new")

	template <typename T, typename ...Args>
	bool class_info::counstructor(type_identity<T>, type_identity_multiple<Args...>) noexcept
	{
		func_info_type &info{m_functions.emplace_back()};
		info.deduce(overload_cast_member(T *, MFW_NOTHING, dtor_ctor_helper<T>, ctor, MFW_NOTHING, Args...));
		info.name().assign(MFW_T("counstructor"_sv));
		info.this_info().deduce(type_identity<T>{});
		return false;
	}

	template <typename T>
	bool class_info::destructor(type_identity<T>) noexcept
	{
		func_info_type &info{m_functions.emplace_back()};
		info.deduce(overload_cast_member(void, MFW_NOTHING, dtor_ctor_helper<T>, dtor, MFW_NOTHING, void));
		info.name().assign(MFW_T(u8"destructor"_sv));
		info.this_info().deduce(type_identity<T>{});
		return true;
	}

	template <typename V, typename T>
	class_info::member_variable_info &class_info::variable_base(V T::*var) noexcept
	{
		member_variable_info &info{m_variables.emplace_back()};
		info.deduce(var);
		return info;
	}

	template <typename V, typename T>
	bool class_info::variable(V T::*var, string_view_type name) noexcept
	{
		member_variable_info &info{variable_base(var)};
		info.name().assign(name);
		return true;
	}

	template <typename V, typename T>
	bool class_info::variable(V T::*var, string_type &&name) noexcept
	{
		member_variable_info &info{variable_base(var)};
		info.name().assign(stl::move(name));
		return true;
	}

	template <typename R, typename T, typename ...Args>
	class_info::func_info_type &class_info::function_base(R (T::*func)(Args...)) noexcept
	{
		func_info_type &info{m_functions.emplace_back()};
		info.deduce(func);
		return info;
	}

	template <typename R, typename T, typename ...Args>
	bool class_info::function(R (T::*func)(Args...), string_view_type name) noexcept
	{
		func_info_type &info{function_base(func)};
		info.name().assign(name);
		return false;
	}

	template <typename R, typename T, typename ...Args>
	bool class_info::function(R (T::*func)(Args...), string_type &&name) noexcept
	{
		func_info_type &info{function_base(func)};
		info.name().assign(stl::move(name));
		return false;
	}

	template <typename T>
	class_infoT<T> &class_infoT<T>::deduce() noexcept
	{ super::deduce(type_identity<T>{}); return *this; }

	template <typename T>
	class_infoT<T>::class_infoT() noexcept
	{ deduce(); }

	template <typename T>
	template <typename ...Args>
	bool class_infoT<T>::counstructor() noexcept
	{ return super::counstructor(type_identity<T>{}, type_identity_multiple<Args...>{}); }

	template <typename T>
	bool class_infoT<T>::destructor() noexcept
	{ return super::destructor(type_identity<T>{}); }

	template <typename T>
	template <typename V>
	bool class_infoT<T>::variable(V T::*var, string_view_type name) noexcept
	{ return super::variable(var, name); }
	template <typename T>
	template <typename V>
	bool class_infoT<T>::variable(V T::*var, string_type &&name) noexcept
	{ return super::variable(var, stl::move(name)); }

	template <typename T>
	template <typename R, typename ...Args>
	bool class_infoT<T>::function(R (T::*func)(Args...), string_view_type name) noexcept
	{ return super::function(func, name); }
	template <typename T>
	template <typename R, typename ...Args>
	bool class_infoT<T>::function(R (T::*func)(Args...), string_type &&name) noexcept
	{ return super::function(func, stl::move(name)); }
}