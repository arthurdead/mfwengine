namespace mfw::core
{
	MFW_VISIBILITY_LOCAL_PUSH()

	class class_info : public type_info
	{
	public:
		using super = type_info;
		using string_type = stl::osstring;
		using string_view_type = stl::osstring_view;
		using func_info_type = func_info;
		using type_info_type = type_info;

		using optional_function = stl::optinal<stl::reference_wrapper<func_info_type>>;
		using const_optional_function = stl::optinal<stl::reference_wrapper<const func_info_type>>;

		class_info() noexcept = default;
		class_info(string_view_type name) noexcept;
		class_info(string_type &&name) noexcept;

		class_info &clear() noexcept;

		template <typename T>
		class_info &deduce(type_identity<T>) noexcept;

		string_type &name() noexcept;
		const string_type &name() const noexcept;

		optional_function function(string_view_type name) noexcept;
		const_optional_function function(string_view_type name) const noexcept;

		stl::vector<func_info_type> &functions() noexcept;
		const stl::vector<func_info_type> &functions() const noexcept;

		class member_variable_info final : public type_info_type
		{
		public:
			member_variable_info &clear() noexcept;

			template <typename T, typename V>
			member_variable_info &deduce(V T::*var_) noexcept;

			string_type &name() noexcept;
			const string_type &name() const noexcept;

			MFW_CORE_API member_variable_info & MFW_CORE_CALL set(type_holder &obj, const type_holder &val) const noexcept;
			MFW_CORE_API member_variable_info & MFW_CORE_CALL get(const type_holder &obj, type_holder &val) const noexcept;
			MFW_CORE_API member_variable_info & MFW_CORE_CALL get_ptr(const type_holder &obj, type_holder &val) const noexcept;

		private:
			template <typename T, typename V>
			static void set_helper(T *ptr, V T::*var, const V &value) noexcept

			template <typename T, typename V>
			static void get_helper(T *ptr, V T::*var, V &value) noexcept;

			string_type m_name{};
			stl::size_t m_offset{0};
			void *m_var{nullptr};
			void *m_set_func{nullptr};
			void *m_get_func{nullptr};
		};

		using optional_variable = stl::optinal<stl::reference_wrapper<member_variable_info>>;
		using const_optional_variable = stl::optinal<stl::reference_wrapper<const member_variable_info>>;

		optional_variable variable(string_view_type name) noexcept;
		const_optional_variable variable(string_view_type name) const noexcept;

		stl::vector<member_variable_info> &variables() noexcept;
		const stl::vector<member_variable_info> &variables() const noexcept;

	protected:
		template <typename T>
		class dtor_ctor_helper final
		{
		public:
			void dtor() noexcept;

			template <typename ...Args>
			T *ctor(Args &&... args) noexcept;
		};

		template <typename T, typename ...Args>
		bool counstructor(type_identity<T>, type_identity_multiple<Args...>) noexcept;

		template <typename T>
		bool destructor(type_identity<T>) noexcept;

		template <typename V, typename T>
		member_variable_info &variable_base(V T::*var) noexcept;
		template <typename V, typename T>
		bool variable(V T::*var, string_view_type name) noexcept;
		template <typename V, typename T>
		bool variable(V T::*var, string_type &&name) noexcept;

		template <typename R, typename T, typename ...Args>
		func_info_type &function_base(R (T::*func)(Args...)) noexcept;
		template <typename R, typename T, typename ...Args>
		bool function(R (T::*func)(Args...), string_view_type name) noexcept;
		template <typename R, typename T, typename ...Args>
		bool function(R (T::*func)(Args...), string_type &&name) noexcept;

	private:
		string_type m_name{};
		stl::vector<member_variable_info> m_variables{};
		stl::vector<func_info_type> m_functions{};
	};

	template <typename T>
	class class_infoT final : public class_info
	{
	public:
		using super = class_info;

		class_infoT &deduce() noexcept;

		class_infoT() noexcept;

		template <typename ...Args>
		bool counstructor() noexcept;
		bool destructor() noexcept;

		template <typename V>
		bool variable(V T::*var, string_view_type name) noexcept;
		template <typename V>
		bool variable(V T::*var, string_type &&name) noexcept;

		template <typename R, typename ...Args>
		bool function(R (T::*func)(Args...), string_view_type name) noexcept;
		template <typename R, typename ...Args>
		bool function(R (T::*func)(Args...), string_type &&name) noexcept;
	};

	MFW_VISIBILITY_LOCAL_POP()
}