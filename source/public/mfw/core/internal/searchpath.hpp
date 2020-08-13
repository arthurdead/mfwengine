namespace mfw::core
{
	MFW_VISIBILITY_LOCAL_PUSH()
	
	struct searchpath final
	{
		using string_type = stl::osstring;
		using string_view_type = stl::osstring_view;
		using char_type = stl::oschar_t;

		searchpath() noexcept = default;
		searchpath(const searchpath &) noexcept = default;
		searchpath &operator=(const searchpath &) noexcept = default;
		searchpath(searchpath &&) noexcept = default;
		searchpath &operator=(searchpath &&) noexcept = default;
		~searchpath() noexcept = default;

		searchpath(const searchpath_view &) noexcept;
		searchpath &operator=(const searchpath_view &) noexcept;

		searchpath(stl::pchar_t *ptr, stl::size_t len) noexcept;
		searchpath(char_type *ptr, stl::size_t len) noexcept;

		searchpath(stl::pstring_view path) noexcept;
		searchpath(stl::pstring &&path) noexcept;
		searchpath(string_view_type name) noexcept;
		searchpath(string_type &&name) noexcept;

		searchpath(stl::pstring_view path, string_view_type name) noexcept;
		searchpath(stl::pstring_view path, string_type &&name) noexcept;
		searchpath(stl::pstring &&path, string_view_type name) noexcept;
		searchpath(stl::pstring &&path, string_type &&name) noexcept;

		operator searchpath_view() const noexcept;

		bool empty() const noexcept;
		searchpath &clear() noexcept;

		stl::pstring m_path{};
		string_type m_name{};
	};

	MFW_VISIBILITY_LOCAL_POP()
}