namespace mfw::core
{
	template <typename T>
	func_info::func_info(T func, string_view_type name) noexcept
		: m_name{name} { deduce(func); }

	template <typename T>
	func_info::func_info(T func, string_type &&name) noexcept
		: m_name{stl::move(name)} { deduce(func); }

	func_info::func_info(string_view_type name) noexcept
		: m_name{name} {}

	func_info::func_info(string_type &&name) noexcept
		: m_name{stl::move(name)} {}

	template <typename R, typename... Args>
	void func_info::set_funcptr(R (*func)(Args...)) noexcept {
		m_this_info.clear();
		m_funcptr = stl::force_cast<void *>(func);
	}
	template <typename R, typename T, typename... Args>
	void func_info::set_funcptr(R (T::*func)(Args...)) noexcept {
		m_this_info.deduce(stl::type_identity<T>{});
		m_funcptr = stl::force_cast<void *>(func);
	}

	template <typename T>
	T func_info::get_funcptr() const noexcept
	{ return stl::force_cast<T>(m_funcptr); }

	template <typename T>
	void func_info::deduce(T func) noexcept
	{
		clear();

		set_funcptr(func);
		m_virtualindex = static_cast<stl::size_t>(-1);

		using traits_t = function_traits<T>;
		using ret_t = typename traits_t::ret_t;
		using this_t = typename traits_t::this_t;
		using args_t = typename traits_t::args_t;

		m_return_info.deduce(type_identity<ret_t>{});
		m_this_info.deduce(type_identity<this_t>{});

		constexpr size_t size{traits_t::args_num};
		if constexpr(size > 0) {
			add_arg_helper<0, size, args_t>();
		}
	}

	template <size_t I, size_t S, typename T>
	void func_info::add_arg_helper() noexcept
	{
		if constexpr(I < S) {
			using type_t = stl::tuple_element_t<I, T>;

			add_arg(type_identity<type_t>{});

			if constexpr(I+1 < S) {
				add_arg_helper<I+1, S, T>();
			}
		}
	}

	template <typename T>
	void func_info::add_arg(type_identity<T>) noexcept
	{
		type_info_type &info{arg_infos.emplace_back()};
		info.deduce(type_identity<T>{});
	}
}