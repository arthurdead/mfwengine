namespace mfw::core
{
	class MFW_VISIBILITY_LOCAL type_info final
	{
	public:
		using string_type = stl::osstring;
		using string_view_type = stl::osstring_view;

		template <typename T>
		void deduce(stl::type_identity<T>) noexcept;

		type_info() noexcept = default;

		template <typename T>
		type_info(stl::type_identity<T>) noexcept;

		void clear() noexcept;

		bool valid() const noexcept;

		stl::size_t size() const noexcept;
		stl::size_t align() const noexcept;
		const stl::type_info &stl_info() const noexcept;

		bool is_exact(const type_info &info) const noexcept;
		bool is_exact(const stl::type_info &info) const noexcept;

		template <typename T>
		bool is_relaxed() const noexcept;

		template <typename T>
		bool is_exact() const noexcept;

		bool is_int8() const noexcept;
		bool is_int16() const noexcept;
		bool is_int32() const noexcept;
		bool is_int64() const noexcept;
		bool is_int128() const noexcept;
		bool is_float16() const noexcept;
		bool is_float32() const noexcept;
		bool is_float64() const noexcept;
		bool is_float80() const noexcept;
		bool is_float128() const noexcept;

		bool is_exact(string_view_type name) const noexcept;

		bool is_void() const noexcept;
		bool is_primitive() const noexcept;
		bool is_class() const noexcept;
		bool is_rvalue_ref() const noexcept;
		bool is_const() const noexcept;
		bool is_lvalue_ref() const noexcept;
		bool is_signed() const noexcept;
		bool is_unsigned() const noexcept;
		bool is_array() const noexcept;
		bool is_pointer() const noexcept;
		stl::size_t rank() const noexcept;
		stl::size_t extent() const noexcept;
		stl::size_t num_pointers() const noexcept;
		bool is_any_int() const noexcept;
		bool is_any_float() const noexcept;
		bool is_any_reference() const noexcept;
		bool is_ptr_like() const noexcept;
		const string_type &name() const noexcept;

		MFW_CORE_API const class_info * MFW_CORE_CALL find_class_info() const noexcept;

		void make_pointer() noexcept;

	private:
		void set_name() noexcept;

		enum class modify_type_flags : stl::int16_t
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
			remove_all = (remove_const|remove_reference|remove_pointer|remove_all_extents),
			remove_all_but_const = (remove_all & ~remove_const),
			remove_all_but_reference = (remove_all & ~remove_reference),
			remove_all_but_pointer = (remove_all & ~remove_pointer),
			remove_all_but_extents = (remove_all & ~remove_all_extents),
		};
		MFW_CLASS_ENUM_FLAGS_V1(modify_type_flags)

		struct modify_type final
		{
		private:
			template <typename T, modify_type_flags flags>
			static constexpr decltype(auto) modify_type_helper() noexcept;

		public:
			template <typename T, modify_type_flags flags>
			using type = decltype(modify_type_helper<T, flags>());
		};

		template <typename T, modify_type_flags flags>
		using modify_type_t = typename modify_type::type<T, flags>;

		enum class flags : stl::uint16_t
		{
			none,
			const_ = MFW_BIT(0),
			lvalue_ref = MFW_BIT(1),
			rvalue_ref = MFW_BIT(2),
			signed_ = MFW_BIT(3),
			unsigned_ = MFW_BIT(4),
			pointer = MFW_BIT(5),
			array = MFW_BIT(6),
			class_ = MFW_BIT(7),
		};
		MFW_CLASS_ENUM_FLAGS(flags)

		stl::size_t m_rank{0};
		stl::size_t m_extent{0};
		stl::size_t m_pointers{0};

		flags m_flags{flags::none};
		stl::size_t m_size{0};
		stl::size_t m_align{0};
		const stl::type_info *m_stl_info{nullptr};
		string_type m_name{};
	};
}

#include <public/mfw/core/impl/type_info.tpp>