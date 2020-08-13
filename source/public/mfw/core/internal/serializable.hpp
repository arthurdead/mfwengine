namespace mfw::core
{
	class MFW_VISIBILITY_PUBLIC serializable_parser_callbacks;

	class MFW_VISIBILITY_PUBLIC serializable
	{
	public:
		using string_type = stl::osstring;
		using string_view_type = stl::osstring_view;
		using univalue_type = univalue;
		using univalue_view_type = univalue_view;
		using searchpath_view_type = searchpath_view;

		using parser_callbacks = serializable_parser_callbacks;
		using expression_callbacks = expression_parser_callbacks;

		using optional_child = stl::optional<stl::reference_wrapper<serializable>>;
		using const_optional_child = stl::optional<stl::reference_wrapper<const serializable>>;
		using optional_value = stl::optional<stl::reference_wrapper<univalue_type>>;
		using const_optional_value = stl::optional<stl::reference_wrapper<const univalue_type>>;
		using const_optional_parser_callbacks = stl::optional<stl::reference_wrapper<const parser_callbacks>>;
		using const_optional_expression_callbacks = stl::optional<stl::reference_wrapper<const expression_callbacks>>;

		MFW_VISIBILITY_LOCAL serializable() noexcept = default;
		MFW_VISIBILITY_LOCAL virtual ~serializable() noexcept = default;

		MFW_VISIBILITY_LOCAL serializable &operator=(serializable &&other) noexcept = default;
		MFW_VISIBILITY_LOCAL serializable(serializable &&other) noexcept = default;

		MFW_CORE_API serializable & MFW_CORE_CALL operator=(const serializable &other) noexcept;
		MFW_VISIBILITY_LOCAL serializable(const serializable &other) noexcept;

		MFW_CORE_API bool MFW_CORE_CALL operator==(const serializable &other) const noexcept;
		MFW_VISIBILITY_LOCAL bool operator!=(const serializable &other) const noexcept;

		MFW_CORE_API bool MFW_CORE_CALL from_file_string(searchpath_view_type search, const_optional_parser_callbacks callbacks = {}) noexcept;
		MFW_CORE_API bool MFW_CORE_CALL from_file_binary(searchpath_view_type search) noexcept;
		MFW_CORE_API bool MFW_CORE_CALL from_string(string_view_type str, const_optional_parser_callbacks callbacks = {}) noexcept;
		MFW_CORE_API bool MFW_CORE_CALL from_binary(const stl::vector<stl::byte> &bin) noexcept;

		MFW_CORE_API bool MFW_CORE_CALL to_file_string(searchpath_view_type search) const noexcept;
		MFW_CORE_API bool MFW_CORE_CALL to_file_binary(searchpath_view_type search) const noexcept;
		MFW_CORE_API serializable & MFW_CORE_CALL to_string(string_type &str) const noexcept;
		MFW_CORE_API serializable & MFW_CORE_CALL to_binary(stl::vector<stl::byte> &bin) const noexcept;

		MFW_VISIBILITY_LOCAL serializable &clear() noexcept;

		MFW_VISIBILITY_LOCAL string_type &name() noexcept;
		MFW_VISIBILITY_LOCAL const string_type &name() const noexcept;

		MFW_VISIBILITY_LOCAL univalue_type &value() noexcept;
		MFW_VISIBILITY_LOCAL const univalue_type &value() const noexcept;
		MFW_VISIBILITY_LOCAL bool has_value() const noexcept;
		
		MFW_VISIBILITY_LOCAL bool get_value_bool() const noexcept;

		MFW_CORE_API optional_child MFW_CORE_CALL root() noexcept;
		MFW_CORE_API const_optional_child MFW_CORE_CALL root() const noexcept;

		MFW_VISIBILITY_LOCAL optional_child parent() noexcept;
		MFW_VISIBILITY_LOCAL const_optional_child parent() const noexcept;
		MFW_VISIBILITY_LOCAL bool has_parent() const noexcept;

		MFW_VISIBILITY_LOCAL serializable &flags() noexcept;
		MFW_VISIBILITY_LOCAL const_optional_child flags() const noexcept;
		MFW_CORE_API serializable & MFW_CORE_CALL create_flags() noexcept;
		MFW_VISIBILITY_LOCAL optional_child get_flags() noexcept;
		MFW_VISIBILITY_LOCAL const_optional_child get_flags() const noexcept;
		MFW_VISIBILITY_LOCAL bool has_flags() const noexcept;
		
		MFW_VISIBILITY_LOCAL optional_child get_flag(string_view_type name) noexcept;
		MFW_VISIBILITY_LOCAL const_optional_child get_flag(string_view_type name) const noexcept;
		MFW_VISIBILITY_LOCAL bool get_flag_bool(string_view_type name) const noexcept;

		MFW_VISIBILITY_LOCAL string_type &condition() noexcept;
		MFW_VISIBILITY_LOCAL const string_type &condition() const noexcept;
		MFW_VISIBILITY_LOCAL bool has_condition() const noexcept;
		MFW_CORE_API bool MFW_CORE_CALL passes_condition(const_optional_expression_callbacks callbacks = {}) const noexcept;

		MFW_VISIBILITY_LOCAL const_optional_child child(string_view_type name) const noexcept;
		MFW_VISIBILITY_LOCAL serializable &child(string_view_type name) noexcept;
		MFW_VISIBILITY_LOCAL serializable &child(string_type &&name) noexcept;
		MFW_VISIBILITY_LOCAL serializable &create_child(string_view_type name) noexcept;
		MFW_VISIBILITY_LOCAL serializable &create_child(string_type &&name) noexcept;
		MFW_VISIBILITY_LOCAL optional_child get_child(string_view_type name) noexcept;
		MFW_VISIBILITY_LOCAL const_optional_child get_child(string_view_type name) const noexcept;
		MFW_VISIBILITY_LOCAL const_optional_child get_child(stl::size_t i) const noexcept;
		MFW_VISIBILITY_LOCAL const_optional_child operator[](stl::size_t i) const noexcept;
		MFW_VISIBILITY_LOCAL const_optional_child operator[](string_view_type name) const noexcept;
		MFW_VISIBILITY_LOCAL serializable &operator[](string_view_type name) noexcept;
		MFW_VISIBILITY_LOCAL serializable &operator[](string_type &&name) noexcept;
		MFW_VISIBILITY_LOCAL bool has_child(string_view_type name) const noexcept;

		MFW_VISIBILITY_LOCAL bool get_child_bool(string_view_type str) const noexcept;

		MFW_VISIBILITY_LOCAL bool has_flag(string_view_type name) const noexcept;
		MFW_VISIBILITY_LOCAL serializable &add_flag(string_view_type name) noexcept;
		MFW_VISIBILITY_LOCAL serializable &add_flag(string_type &&name) noexcept;

		MFW_VISIBILITY_LOCAL const_optional_value get_value(string_view_type name) const noexcept;
		MFW_VISIBILITY_LOCAL optional_value get_value(string_view_type name) noexcept;

		MFW_CORE_API stl::size_t MFW_CORE_CALL index() const noexcept;

		MFW_VISIBILITY_LOCAL serializable &remove_all() noexcept;
		MFW_CORE_API bool MFW_CORE_CALL erase(string_view_type str) noexcept;

		using merge_str_process_t = function<void(string_type &)>;
		MFW_CORE_API serializable & MFW_CORE_CALL merge(const serializable &other, bool replace=true, const merge_str_process_t &func = nullptr) noexcept;
		MFW_CORE_API serializable & MFW_CORE_CALL merge(const serializable &other, bool replace=true) noexcept;
		MFW_CORE_API serializable & MFW_CORE_CALL merge(serializable &&other, bool replace=true, const merge_str_process_t &func = nullptr) noexcept;
		MFW_CORE_API serializable & MFW_CORE_CALL merge(serializable &&other, bool replace=true) noexcept;

		MFW_CORE_API serializable & MFW_CORE_CALL follow_path(string_view_type path, univalue_type &value) const noexcept;
		MFW_CORE_API optional_child MFW_CORE_CALL follow_path(string_view_type path) noexcept;
		MFW_CORE_API const_optional_child MFW_CORE_CALL follow_path(string_view_type path) const noexcept;

		using child_vec_t = stl::ptr_vector<serializable>;

		using iterator = child_vec_t::iterator;
		using reverse_iterator = child_vec_t::reverse_iterator;
		using const_iterator = child_vec_t::const_iterator;
		using const_reverse_iterator = child_vec_t::const_reverse_iterator;

		MFW_CORE_API iterator MFW_CORE_CALL find(string_view_type name) noexcept;
		MFW_CORE_API const_iterator MFW_CORE_CALL find(string_view_type name) const noexcept;

		MFW_VISIBILITY_LOCAL serializable &erase(iterator it) noexcept;
		MFW_VISIBILITY_LOCAL serializable &erase(const_iterator it) noexcept;

		MFW_VISIBILITY_LOCAL stl::size_t size() const noexcept;

		MFW_VISIBILITY_LOCAL bool empty() const noexcept;

		MFW_VISIBILITY_LOCAL const_iterator cbegin() const noexcept;
		MFW_VISIBILITY_LOCAL const_iterator cend() const noexcept;

		MFW_VISIBILITY_LOCAL const_reverse_iterator crbegin() const noexcept;
		MFW_VISIBILITY_LOCAL const_reverse_iterator crend() const noexcept;

		MFW_VISIBILITY_LOCAL iterator begin() noexcept;
		MFW_VISIBILITY_LOCAL iterator end() noexcept;

		MFW_VISIBILITY_LOCAL reverse_iterator rbegin() noexcept;
		MFW_VISIBILITY_LOCAL reverse_iterator rend() noexcept;

		MFW_VISIBILITY_LOCAL const_iterator begin() const noexcept;
		MFW_VISIBILITY_LOCAL const_iterator end() const noexcept;

		MFW_VISIBILITY_LOCAL const_reverse_iterator rbegin() const noexcept;
		MFW_VISIBILITY_LOCAL const_reverse_iterator rend() const noexcept;

		MFW_VISIBILITY_LOCAL serializable &find_or_emplace(string_view_type name, const_iterator it) noexcept;
		MFW_VISIBILITY_LOCAL serializable &find_or_emplace(string_view_type name) noexcept;

		MFW_VISIBILITY_LOCAL serializable &find_or_emplace(string_type &&name, const_iterator it) noexcept;
		MFW_VISIBILITY_LOCAL serializable &find_or_emplace(string_type &&name) noexcept;

	private:
		MFW_VISIBILITY_LOCAL virtual serializable *allocate_child(stl::ssize_t depth, string_view_type name, const serializable &parent) const noexcept;
		MFW_VISIBILITY_LOCAL virtual void merge_child(stl::size_t depth, serializable &child, const serializable &other) const noexcept;
		MFW_VISIBILITY_LOCAL void to_string(string_type &str, stl::size_t ident) const noexcept;
		MFW_CORE_API void MFW_CORE_CALL create_child_base(string_view_type name, const_iterator it) noexcept;
		MFW_VISIBILITY_LOCAL void create_child(string_view_type name, const_iterator it) noexcept;
		MFW_VISIBILITY_LOCAL void create_child(string_type &&name, const_iterator it) noexcept;

		string_type m_name{};
		univalue_type m_value{};
		using flags_t = stl::unique_ptr<serializable>;
		flags_t m_flags{};
		string_type m_condition{};
		optional_child m_parent{};
		child_vec_t m_childs{};
	};
}