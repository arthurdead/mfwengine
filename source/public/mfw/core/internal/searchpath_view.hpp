namespace mfw::core
{
	MFW_VISIBILITY_LOCAL_PUSH()

	struct searchpath;

	struct searchpath_view final
	{
		using string_view_type = stl::osstring_view;
		using char_type = stl::oschar_t;

		searchpath_view() noexcept = default;
		searchpath_view(const searchpath_view &) noexcept = default;
		searchpath_view &operator=(const searchpath_view &) noexcept = default;
		searchpath_view(searchpath_view &&) noexcept = default;
		searchpath_view &operator=(searchpath_view &&) noexcept = default;
		~searchpath_view() noexcept = default;

		searchpath_view(const searchpath &) noexcept;
		searchpath_view &operator=(const searchpath &) noexcept;

		searchpath_view(stl::pchar_t *ptr, stl::size_t len) noexcept;
		searchpath_view(char_type *ptr, stl::size_t len) noexcept;

		searchpath_view(stl::pstring_view path) noexcept;
		searchpath_view(string_view_type name) noexcept;

		searchpath_view(stl::pstring_view path, string_view_type name) noexcept;

		bool empty() const noexcept;

		stl::pstring_view m_path{};
		string_view_type m_name{};
	};

	MFW_VISIBILITY_LOCAL_POP()
}