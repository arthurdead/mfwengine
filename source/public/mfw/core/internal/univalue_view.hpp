namespace mfw::core
{
	#define __MFW_UNIVALUE_VIEW_OP(op) \
		univalue_view operator op (univalue_view) const noexcept;

	#define __MFW_UNIVALUE_VIEW_OP_CMP(op) \
		bool operator op (univalue_view) const noexcept;

	#define __MFW_UNIVALUE_VIEW_OP_EQUAL_CMP(op) \
		__MFW_UNIVALUE_VIEW_OP_CMP(op) \
		bool operator op =(const univalue_view &) noexcept;

	#define __MFW_UNIVALUE_VIEW_OP_SCALAR(op, type) \
		type operator op (type value) const noexcept;

	#define __MFW_UNIVALUE_VIEW_OP_SCALAR_CMP(op, type) \
		bool operator op (type value) const noexcept;

	#define __MFW_UNIVALUE_VIEW_OP_EQUAL_SCALAR_CMP(op, type) \
		__MFW_UNIVALUE_VIEW_OP_SCALAR_CMP(op, type) \
		bool operator op =(type value) const noexcept;

	#define __MFW_UNIVALUE_VIEW_OP_EQUAL_EQUAL_SCALAR(op, type) \
		explicit univalue_view(type value) noexcept; \
		univalue_view &operator=(type value) noexcept; \
		bool operator==(type value) const noexcept; \
		bool operator!=(type value) const noexcept; \
		explicit operator type() const noexcept;

	class MFW_VISIBILITY_LOCAL univalue_view final
	{
	public:
		using float_type = stl::float80_t;
		using int_type = stl::uint64_t;
		using string_type = stl::osstring;
		using string_view_type = stl::osstring_view;

		univalue_view() noexcept = default;
		univalue_view &operator=(const univalue_view &other) noexcept = default;
		univalue_view(const univalue_view &other) noexcept = default;
		univalue_view &operator=(univalue_view &&other) noexcept = default;
		univalue_view(univalue_view &&other) noexcept = default;
		~univalue_view() noexcept = default;

		univalue_view(const univalue &) noexcept;
		univalue_view &operator=(const univalue &) noexcept;

		__MFW_UNIVALUE_VIEW_OP(+)
		__MFW_UNIVALUE_VIEW_OP(-)
		__MFW_UNIVALUE_VIEW_OP(*)
		__MFW_UNIVALUE_VIEW_OP(&)
		__MFW_UNIVALUE_VIEW_OP(|)
		__MFW_UNIVALUE_VIEW_OP(/)
		__MFW_UNIVALUE_VIEW_OP(<<)
		__MFW_UNIVALUE_VIEW_OP(>>)
		__MFW_UNIVALUE_VIEW_OP(%)
		__MFW_UNIVALUE_VIEW_OP(^)
		__MFW_UNIVALUE_VIEW_OP_EQUAL_CMP(<)
		__MFW_UNIVALUE_VIEW_OP_EQUAL_CMP(>)

		__MFW_UNIVALUE_VIEW_OP_CMP(&&)
		__MFW_UNIVALUE_VIEW_OP_CMP(||)
		__MFW_UNIVALUE_VIEW_OP_CMP(~=)

		__MFW_UNIVALUE_VIEW_OP_EQUAL_EQUAL_SCALAR(float_type)
		__MFW_UNIVALUE_VIEW_OP_EQUAL_EQUAL_SCALAR(int_type)
		__MFW_UNIVALUE_VIEW_OP_EQUAL_EQUAL_SCALAR(bool)

		__MFW_UNIVALUE_VIEW_OP_SCALAR(+, float_type)
		__MFW_UNIVALUE_VIEW_OP_SCALAR(-, float_type)
		__MFW_UNIVALUE_VIEW_OP_SCALAR(/, float_type)
		__MFW_UNIVALUE_VIEW_OP_SCALAR(*, float_type)
		__MFW_UNIVALUE_VIEW_OP_EQUAL_SCALAR_CMP(<, float_type)
		__MFW_UNIVALUE_VIEW_OP_EQUAL_SCALAR_CMP(>, float_type)
		__MFW_UNIVALUE_VIEW_OP_SCALAR_CMP(&&, float_type)
		__MFW_UNIVALUE_VIEW_OP_SCALAR_CMP(||, float_type)

		__MFW_UNIVALUE_VIEW_OP_SCALAR(+, int_type)
		__MFW_UNIVALUE_VIEW_OP_SCALAR(-, int_type)
		__MFW_UNIVALUE_VIEW_OP_SCALAR(*, int_type)
		__MFW_UNIVALUE_VIEW_OP_SCALAR(/, int_type)
		__MFW_UNIVALUE_VIEW_OP_SCALAR(<<, int_type)
		__MFW_UNIVALUE_VIEW_OP_SCALAR(>>, int_type)
		__MFW_UNIVALUE_VIEW_OP_SCALAR(|, int_type)
		__MFW_UNIVALUE_VIEW_OP_SCALAR(&, int_type)
		__MFW_UNIVALUE_VIEW_OP_EQUAL_SCALAR_CMP(<, int_type)
		__MFW_UNIVALUE_VIEW_OP_EQUAL_SCALAR_CMP(>, int_type)
		__MFW_UNIVALUE_VIEW_OP_SCALAR(^, int_type)
		__MFW_UNIVALUE_VIEW_OP_SCALAR(%, int_type)
		__MFW_UNIVALUE_VIEW_OP_SCALAR_CMP(&&, int_type)
		__MFW_UNIVALUE_VIEW_OP_SCALAR_CMP(||, int_type)

		__MFW_UNIVALUE_VIEW_OP_SCALAR_CMP(&&, bool)
		__MFW_UNIVALUE_VIEW_OP_SCALAR_CMP(||, bool)

		string_type operator+(string_view_type value) const noexcept;
		string_type operator+(string_type &&value) const noexcept;

		bool operator==(string_view_type value) const noexcept;
		bool operator!=(string_view_type value) const noexcept;

		univalue_view &operator=(string_view_type value) noexcept;
		explicit univalue_view(string_view_type value) noexcept;

		univalue_view operator-() const noexcept;
		univalue_view operator+() const noexcept;
		int_type operator~() const noexcept;
		bool operator!() const noexcept;
		const string_view_type *operator->() const noexcept;

		explicit operator const string_view_type &() const noexcept;

		const string_view_type &get_string() const noexcept;
		float_type get_float() const noexcept;
		int_type get_int() const noexcept;
		bool get_bool() const noexcept;

		bool is_bool() const noexcept;
		bool is_float() const noexcept;
		bool is_int() const noexcept;
		bool is_string() const noexcept;

		bool empty() const noexcept;

	private:
		string_view_type m_string{};
		float_type m_float{0.0f};
	};
}