namespace mfw::core
{
	#define __MFW_UNIVALUE_OP(op) \
		univalue operator op (const univalue &) const noexcept; \
		univalue operator op (univalue &&) const noexcept;

	#define __MFW_UNIVALUE_OP_CMP(op) \
		bool operator op (const univalue &) const noexcept;

	#define __MFW_UNIVALUE_OP_EQUAL(op) \
		__MFW_UNIVALUE_OP(op) \
		univalue &operator op =(const univalue &) noexcept; \
		univalue &operator op =(univalue &&) noexcept;

	#define __MFW_UNIVALUE_OP_EQUAL_CMP(op) \
		__MFW_UNIVALUE_OP_CMP(op) \
		bool operator op =(const univalue &) noexcept;

	#define __MFW_UNIVALUE_OP_SCALAR(op, type) \
		type operator op (type value) const noexcept;

	#define __MFW_UNIVALUE_OP_SCALAR_CMP(op, type) \
		bool operator op (type value) const noexcept;

	#define __MFW_UNIVALUE_OP_EQUAL_SCALAR(op, type) \
		__MFW_UNIVALUE_OP_SCALAR(op, type) \
		type operator op =(type value) noexcept;

	#define __MFW_UNIVALUE_OP_EQUAL_SCALAR_CMP(op, type) \
		__MFW_UNIVALUE_OP_SCALAR_CMP(op, type) \
		bool operator op =(type value) const noexcept;

	#define __MFW_UNIVALUE_OP_EQUAL_EQUAL_SCALAR(op, type) \
		explicit univalue(type value) noexcept; \
		univalue &operator=(type value) noexcept; \
		bool operator==(type value) const noexcept; \
		bool operator!=(type value) const noexcept; \
		explicit operator type() const noexcept;

	class MFW_VISIBILITY_LOCAL univalue final
	{
	public:
		using float_type = stl::float80_t;
		using int_type = stl::uint64_t;
		using string_type = stl::osstring;
		using string_view_type = stl::osstring_view;

		univalue() noexcept = default;
		univalue &operator=(const univalue &other) noexcept = default;
		univalue(const univalue &other) noexcept = default;
		univalue &operator=(univalue &&other) noexcept = default;
		univalue(univalue &&other) noexcept = default;
		~univalue() noexcept = default;

		__MFW_UNIVALUE_OP_EQUAL(+)
		__MFW_UNIVALUE_OP_EQUAL(-)
		__MFW_UNIVALUE_OP_EQUAL(*)
		__MFW_UNIVALUE_OP_EQUAL(&)
		__MFW_UNIVALUE_OP_EQUAL(|)
		__MFW_UNIVALUE_OP_EQUAL(/)
		__MFW_UNIVALUE_OP_EQUAL(<<)
		__MFW_UNIVALUE_OP_EQUAL(>>)
		__MFW_UNIVALUE_OP_EQUAL(%)
		__MFW_UNIVALUE_OP_EQUAL(^)
		__MFW_UNIVALUE_OP_EQUAL_CMP(<)
		__MFW_UNIVALUE_OP_EQUAL_CMP(>)

		__MFW_UNIVALUE_OP_CMP(&&)
		__MFW_UNIVALUE_OP_CMP(||)
		__MFW_UNIVALUE_OP_CMP(~=)

		__MFW_UNIVALUE_OP_EQUAL_EQUAL_SCALAR(float_type)
		__MFW_UNIVALUE_OP_EQUAL_EQUAL_SCALAR(int_type)
		__MFW_UNIVALUE_OP_EQUAL_EQUAL_SCALAR(bool)

		__MFW_UNIVALUE_OP_EQUAL_SCALAR(+, float_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(-, float_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(/, float_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(*, float_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR_CMP(<, float_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR_CMP(>, float_type)
		__MFW_UNIVALUE_OP_SCALAR_CMP(&&, float_type)
		__MFW_UNIVALUE_OP_SCALAR_CMP(||, float_type)

		__MFW_UNIVALUE_OP_EQUAL_SCALAR(+, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(-, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(*, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(/, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(<<, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(>>, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(|, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(&, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR_CMP(<, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR_CMP(>, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(^, int_type)
		__MFW_UNIVALUE_OP_EQUAL_SCALAR(%, int_type)
		__MFW_UNIVALUE_OP_SCALAR_CMP(&&, int_type)
		__MFW_UNIVALUE_OP_SCALAR_CMP(||, int_type)

		__MFW_UNIVALUE_OP_SCALAR_CMP(&&, bool)
		__MFW_UNIVALUE_OP_SCALAR_CMP(||, bool)

		string_type operator+(string_view_type value) const noexcept;
		string_type operator+(string_type &&value) const noexcept;

		string_type operator+=(string_view_type value) const noexcept;
		string_type operator+=(string_type &&value) const noexcept;

		bool operator==(string_view_type value) const noexcept;
		bool operator!=(string_view_type value) const noexcept;

		univalue &operator=(string_view_type value) noexcept;
		univalue &operator=(string_type &&value) noexcept;
		explicit univalue(string_view_type value) noexcept;
		explicit univalue(string_type &&value) noexcept;

		univalue operator-() const noexcept;
		univalue operator+() const noexcept;
		univalue &operator--(int) noexcept;
		univalue &operator--() noexcept;
		univalue &operator++(int) noexcept;
		univalue &operator++() noexcept;
		int_type operator~() const noexcept;
		bool operator!() const noexcept;
		const string_type *operator->() const noexcept;

		explicit operator const string_type &() const noexcept;

		operator univalue_view() const noexcept;

		const string_type &get_string() const noexcept;
		float_type get_float() const noexcept;
		int_type get_int() const noexcept;
		bool get_bool() const noexcept;

		bool is_bool() const noexcept;
		bool is_float() const noexcept;
		bool is_int() const noexcept;
		bool is_string() const noexcept;

		void clear() noexcept;

		bool empty() const noexcept;

	private:
		string_type m_string{};
		float_type m_float{0.0f};
	};
}