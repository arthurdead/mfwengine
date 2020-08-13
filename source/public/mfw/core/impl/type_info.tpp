namespace mfw::core
{
	template <typename T, type_info::modify_type_flags F>
	constexpr decltype(auto) type_info::modify_type::modify_type_helper() noexcept
	{
		if constexpr(bool_cast(F & modify_type_flags::remove_const)) {
			using type_t = stl::remove_const_t<T>;
			return modify_type_helper<type_t, F & ~modify_type_flags::remove_const>();
		} else if constexpr(bool_cast(F & modify_type_flags::remove_reference)) {
			using type_t = stl::remove_reference_t<T>;
			return modify_type_helper<type_t, F & ~modify_type_flags::remove_reference>();
		} else if constexpr(bool_cast(F & modify_type_flags::remove_pointer)) {
			using type_t = stl::remove_pointer_t<T>;
			return modify_type_helper<type_t, F & ~modify_type_flags::remove_pointer>();
		} else if constexpr(bool_cast(F & modify_type_flags::remove_all_extents)) {
			using type_t = stl::remove_all_extents_t<T>;
			return modify_type_helper<type_t, F & ~modify_type_flags::remove_all_extents>();
		} else if constexpr(bool_cast(F & modify_type_flags::add_lvalue_reference)) {
			using type_t = stl::add_lvalue_reference_t<T>;
			return modify_type_helper<type_t, F & ~modify_type_flags::add_lvalue_reference>();
		} else if constexpr(bool_cast(F & modify_type_flags::add_rvalue_reference)) {
			using type_t = stl::add_rvalue_reference_t<T>;
			return modify_type_helper<type_t, F & ~modify_type_flags::add_rvalue_reference>();
		} else if constexpr(bool_cast(F & modify_type_flags::add_pointer)) {
			using type_t = stl::add_pointer_t<T>;
			return modify_type_helper<type_t, F & ~modify_type_flags::add_pointer>();
		} else if constexpr(bool_cast(F & modify_type_flags::add_const)) {
			using type_t = stl::add_const_t<T>;
			return modify_type_helper<type_t, F & ~modify_type_flags::add_const>();
		} else if constexpr(bool_cast(F & modify_type_flags::make_signed)) {
			using type_t = stl::make_signed_t<T>;
			return modify_type_helper<type_t, F & ~modify_type_flags::make_signed>();
		} else if constexpr(bool_cast(F & modify_type_flags::make_unsigned)) {
			using type_t = stl::make_unsigned_t<T>;
			return modify_type_helper<type_t, F & ~modify_type_flags::make_unsigned>();
		} else {
			if constexpr(!is_void_v<T>) {
				return T{};
			} else {
				return;
			}
		}
	}

	template <typename T>
	type_info::type_info(stl::type_identity<T>) noexcept
	{ deduce(stl::type_identity<T>{}); }

	template <typename T>
	void type_info::deduce(type_identity<T>) noexcept
	{
		using base_type_t = modify_type_t<T, modify_type_flags::remove_all>;
		using only_const_t = modify_type_t<T, modify_type_flags::remove_all_but_const>;
		using only_reference_t = modify_type_t<T, modify_type_flags::remove_all_but_reference>;
		using only_pointer_t = modify_type_t<T, modify_type_flags::remove_all_but_pointer>;
		using only_extents_t = modify_type_t<T, modify_type_flags::remove_all_but_extents>;

		clear();
		if constexpr(!stl::is_void_v<only_pointer_t>) {
			m_size = sizeof(only_pointer_t);
			m_align = alignof(only_pointer_t);
		} else {
			m_size = 0;
			m_align = 0;
		}
		m_stl_info = &stl::get_typeid<base_type_t>();
		set_name();

		m_rank = stl::rank_v<only_extents_t>;
		m_extent = stl::extent_v<only_extents_t>;

		if constexpr(stl::is_class_v<base_type_t>) {
			m_flags |= flags::class_;
		}
		if constexpr(stl::is_pointer_v<only_pointer_t>) {
			m_flags |= flags::pointer_;
			m_pointers += 1;
		}
		if constexpr(stl::is_array_v<only_extents_t>) {
			m_flags |= flags::array_;
		}
		if constexpr(stl::is_signed_v<base_type_t>) {
			m_flags |= flags::signed_;
		} else if constexpr(stl::is_unsigned_v<base_type_t>) {
			m_flags |= flags::unsigned_;
		}
		if constexpr(stl::is_const_v<only_const_t>) {
			m_flags |= flags::const_;
		}
		if constexpr(stl::is_lvalue_reference_v<only_reference_t> || stl::is_lvalue_reference_v<T>) {
			m_flags |= flags::lvalue_ref;
		} else if constexpr(stl::is_rvalue_reference_v<only_reference_t> || stl::is_rvalue_reference_v<T>) {
			m_flags |= flags::rvalue_ref;
		}
	}

	template <typename T>
	bool type_info::is_exact() const noexcept
	{
		using only_pointer_t = modify_type_t<T, modify_type_flags::remove_all_but_pointer>;

		return is_exact(stl::get_typeid<only_pointer_t>());
	}

	template <typename T>
	bool type_info::is_relaxed() const noexcept
	{
		using only_pointer_t = modify_type_t<T, modify_type_flags::remove_all_but_pointer>;

		constexpr bool is_int_v{
			!stl::is_same_v<only_pointer_t, bool> &&
			!stl::is_floating_point_v<only_pointer_t> &&
			!stl::is_void_v<only_pointer_t> &&
			stl::is_arithmetic_v<only_pointer_t> &&
			stl::is_fundamental_v<only_pointer_t>
		};

		if constexpr(is_int_v) {
			if(is_signed()) {
				using signed_t = modify_type_t<only_pointer_t, modify_type_flags::make_signed>;
				return is_exact(stl::get_typeid<signed_t>());
			} else if(is_unsigned()) {
				using unsigned_t = modify_type_t<only_pointer_t, modify_type_flags::make_unsigned>;
				return is_exact(stl::get_typeid<unsigned_t>());
			}
		}

		return is_exact(stl::get_typeid<only_pointer_t>());
	}
}