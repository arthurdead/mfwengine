namespace mfw::core
{
	class MFW_VISIBILITY_LOCAL func_info final
	{
	public:
		using string_type = stl::osstring;
		using string_view_type = stl::osstring_view;
		using type_info_type = type_info;

		template <typename T>
		void deduce(T func) noexcept;

		func_info() noexcept = default;

		func_info(string_view_type name) noexcept;
		func_info(string_type &&name) noexcept;

		template <typename T>
		func_info(T func, string_view_type name) noexcept;
		template <typename T>
		func_info(T func, string_type &&name) noexcept;

		void clear() noexcept;

		string_type &name() noexcept;
		const string_type &name() const noexcept;

		template <typename R, typename... Args>
		void set_funcptr(R (*func)(Args...)) noexcept;
		template <typename R, typename T, typename... Args>
		void set_funcptr(R (T::*func)(Args...)) noexcept;

		template <typename T = void *>
		T get_funcptr() const noexcept;

		type_info_type &return_info() noexcept;
		const type_info_type &return_info() const noexcept;

		type_info_type &this_info() noexcept;
		const type_info_type &this_info() const noexcept;

		using arg_vec_t = stl::vector<type_info_type>;

		using iterator = arg_vec_t::iterator;
		using reverse_iterator = arg_vec_t::reverse_iterator;
		using const_iterator = arg_vec_t::const_iterator;
		using const_reverse_iterator = arg_vec_t::const_reverse_iterator;

		type_info_type &arg(stl::size_t i) noexcept;
		const type_info_type &arg(stl::size_t i) const noexcept;

		template <typename T>
		void add_arg(type_identity<T>) noexcept;

		type_info_type &operator[](stl::size_t i) noexcept;
		const type_info_type &operator[](stl::size_t i) const noexcept;

		void erase(iterator it) noexcept;
		void erase(const_iterator it) noexcept;

		stl::size_t size() const noexcept;

		bool empty() const noexcept;

		const_iterator cbegin() const noexcept;
		const_iterator cend() const noexcept;

		const_reverse_iterator crbegin() const noexcept;
		const_reverse_iterator crend() const noexcept;

		iterator begin() noexcept;
		iterator end() noexcept;

		reverse_iterator rbegin() noexcept;
		reverse_iterator rend() noexcept;

		const_iterator begin() const noexcept;
		const_iterator end() const noexcept;

		const_reverse_iterator rbegin() const noexcept;
		const_reverse_iterator rend() const noexcept;

	private:
		template <typename T>
		class function_traits;

		#define __MFW_DECLARE_MEMBER_FUNCTRAITS(callconv, cnst, ref, execpt) \
			template <typename R, typename T, typename ...Args> \
			class function_traits<R (callconv T::*)(Args...) cnst ref execpt> final \
			{ \
			public: \
				using func_t = R (callconv T::*)(Args...) cnst; \
				using this_t = T; \
				using ret_t = R; \
				using args_t = tuple<Args...>; \
				static constexpr stl::size_t args_num{sizeof...(Args)}; \
			};

		#define __MFW_DECLARE_STATIC_FUNCTRAITS(callconv, execpt) \
			template <typename R, typename ...Args> \
			class function_traits<R (callconv *)(Args...) execpt> final \
			{ \
			public: \
				using func_t = R (callconv *)(Args...); \
				using this_t = void; \
				using ret_t = R; \
				using args_t = tuple<Args...>; \
				static constexpr stl::size_t args_num{sizeof...(Args)}; \
			};

		__MFW_DECLARE_MEMBER_FUNCTRAITS(MFW_NOTHING, MFW_NOTHING, MFW_NOTHING, MFW_NOTHING)
		__MFW_DECLARE_MEMBER_FUNCTRAITS(MFW_NOTHING, MFW_NOTHING, &, MFW_NOTHING)
		__MFW_DECLARE_MEMBER_FUNCTRAITS(MFW_NOTHING, MFW_NOTHING, &&, MFW_NOTHING)
		__MFW_DECLARE_MEMBER_FUNCTRAITS(MFW_NOTHING, const, MFW_NOTHING, MFW_NOTHING)
		__MFW_DECLARE_MEMBER_FUNCTRAITS(MFW_NOTHING, const, &, MFW_NOTHING)

		__MFW_DECLARE_STATIC_FUNCTRAITS(MFW_NOTHING, MFW_NOTHING)

		template <stl::size_t I, stl::size_t S, typename T>
		void add_arg_helper() noexcept;

		type_info_type m_this_info{};
		arg_vec_t m_args_info{};
		type_info_type m_return_info{};
		void *m_funcptr{nullptr};
		stl::size_t m_virtualindex{static_cast<stl::size_t>(-1)};
	};
}