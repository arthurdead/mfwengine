namespace mfw::core
{
	class MFW_VISIBILITY_LOCAL TypeInfo
	{
	public:
		template <typename T>
		TypeInfo &deduce(stl::type_identity<T>) noexcept;

		TypeInfo() noexcept = default;

		template <typename T>
		TypeInfo(stl::type_identity<T>) noexcept
		{ deduce(stl::type_identity<T>{}); }

		MFW_CORE_API TypeInfo & MFW_CORE_CALL clear();

		bool valid() const noexcept
		{ return !m_name.empty() && m_std_info; }

		stl::size_t size() const noexcept
		{ return m_size; }
		stl::size_t align() const noexcept
		{ return m_align; }
		const stl::type_info &stdInfo() const noexcept
		{ return (m_std_info ? *m_std_info : stl::get_typeid<void>()); }

		bool isExact(const TypeInfo &info) const noexcept
		{ return stdInfo() == info.stdInfo(); }
		bool isExact(const stl::type_info &info) const noexcept
		{ return stdInfo() == info; }

		template <typename T>
		bool isRelaxed() const noexcept;

		template <typename T>
		bool isExact() const noexcept;

		bool isInt8() const noexcept;
		bool isInt16() const noexcept;
		bool isInt32() const noexcept;
		bool isInt64() const noexcept;
		bool isInt128() const noexcept;
		bool isFloat16() const noexcept;
		bool isFloat32() const noexcept;
		bool isFloat64() const noexcept;
		bool isFloat80() const noexcept;
		bool isFloat128() const noexcept;

		bool isExact(stl::osstring_view name) const noexcept
		{ return m_name == name; }

		bool isVoid() const noexcept
		{ return isExact<void>(); }
		bool isPrimitive() const noexcept
		{ return !bool_cast(flags_ & flags::class_) || (isVoid() || isAnyInt() || isAnyFloat()); }
		bool isClass() const noexcept
		{ return bool_cast(flags_ & flags::class_) || !isPrimitive(); }
		bool isRvalueRef() const noexcept
		{ return bool_cast(flags_ & flags::rvalue_ref); }
		bool isConst() const noexcept
		{ return bool_cast(flags_ & flags::const_); }
		bool isLvalueRef() const noexcept
		{ return bool_cast(flags_ & flags::lvalue_ref); }
		bool isSigned() const noexcept
		{ return bool_cast(flags_ & flags::signed_); }
		bool isUnsigned() const noexcept
		{ return bool_cast(flags_ & flags::unsigned_); }
		bool isArray() const noexcept
		{ return bool_cast(flags_ & flags::array_) || rank() > 0; }
		bool isPointer() const noexcept
		{ return bool_cast(flags_ & flags::pointer_) || numPointers() > 0; }
		size_t rank() const noexcept
		{ return rank_; }
		size_t extent() const noexcept
		{ return extent_; }
		size_t numPointers() const noexcept
		{ return pointers; }
		bool isAnyInt() const noexcept
		{ return isInt8() || isInt16() || isInt32() || isInt64() || isInt128(); }
		bool isAnyFloat() const noexcept
		{ return isFloat16() || isFloat32() || isFloat64() || isFloat80() || isFloat128(); }
		bool isAnyReference() const noexcept
		{ return isRvalueRef() || isLvalueRef(); }
		bool isPtrLike() const noexcept
		{ return isPointer() || isArray(); }
		const stl::osstring &name() const noexcept
		{ return m_name; }

		MFW_CORE_API const class_info * MFW_CORE_CALL find_class_info() const noexcept;

		MFW_CORE_API void MFW_CORE_CALL make_pointer() noexcept;

	private:
		MFW_CORE_API void MFW_CORE_CALL set_name() noexcept;

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

		struct modify_type
		{
			template <typename T, modify_type_flags flags>
			static constexpr decltype(auto) __modify_type_helper() noexcept;

			template <typename T, modify_type_flags flags>
			using type = remove_reference_t<decltype(__modify_type_helper<T, flags>())>;
		};

		template <typename T, modify_type_flags flags>
		using modify_type_t = typename modify_type::type<T, flags>;

		enum class flags_t : stl::uint16_t
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
		MFW_CLASS_ENUM_FLAGS(flags_t)

		size_t m_rank{0};
		size_t m_extent{0};
		size_t m_pointers{0};

		flags_t m_flags{flags_t::none};
		stl::size_t m_size{0};
		stl::size_t m_align{0};
		const stl::type_info *m_std_info{nullptr};
		stl::osstring m_name{};
	};
}

#include <public/mfw/core/impl/typeinfo.ipp>