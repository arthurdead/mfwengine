namespace mfw::core
{
	class log_context final
	{
	public:
		using string_type = stl::osstring;
		using string_view_type = stl::osstring_view;

		log_context() noexcept = default;
		log_context(string_view_type name) noexcept;
		log_context(string_type &&name) noexcept;
		log_context(pstring_view name) noexcept;
		log_context(pstring &&name) noexcept;

		using severity = log_severity;

		string_type &name() noexcept;
		const string_type &name() const noexcept;

		string_type &tag() noexcept;
		const string_type &tag() const noexcept;

		stl::size_t &ident() noexcept;
		stl::size_t ident() const noexcept;

		severity &ident() noexcept;
		severity ident() const noexcept;

		bool paused() const noexcept { return paused_; }
		void pause() noexcept { paused_ = true; }
		MFW_CORE_API void MFW_CORE_CALL resume() noexcept;
		void clear_history() noexcept { history.clear(); }

		MFW_CORE_API void MFW_CORE_CALL print(const ucstring_view &str) noexcept;

		template <typename ...Args>
		bool print(const ucstring_view &fmtstr, Args &&... args) noexcept
		{
			ucstring str{};
			if(!format(str, fmtstr, forward<Args>(args)...)) {
				return false;
			}
			print(str);
			return true;
		}

		void warning(const ucstring_view &str) noexcept {
			set_severity(severity::warning);
			print(str);
		}
		void error(const ucstring_view &str) noexcept {
			set_severity(severity::error);
			print(str);
		}
		void success(const ucstring_view &str) noexcept {
			set_severity(severity::success);
			print(str);
		}
		void info(const ucstring_view &str) noexcept {
			set_severity(severity::info);
			print(str);
		}

		template <typename ...Args>
		bool warning(const ucstring_view &fmtstr, Args &&... args) noexcept {
			set_severity(severity::warning);
			return print(fmtstr, forward<Args>(args)...);
		}
		template <typename ...Args>
		bool error(const ucstring_view &fmtstr, Args &&... args) noexcept {
			set_severity(severity::error);
			return print(fmtstr, forward<Args>(args)...);
		}
		template <typename ...Args>
		bool success(const ucstring_view &fmtstr, Args &&... args) noexcept {
			set_severity(severity::success);
			return print(fmtstr, forward<Args>(args)...);
		}
		template <typename ...Args>
		bool info(const ucstring_view &fmtstr, Args &&... args) noexcept {
			set_severity(severity::info);
			return print(fmtstr, forward<Args>(args)...);
		}

	private:
		struct print_vars_t
		{
			ucstring name_{};
			ucstring tag{};
			size_t ident{0};
			severity severity_{severity::unknown};
		};
		struct history_vars_t final : print_vars_t
		{
			history_vars_t &operator=(const print_vars_t &other) noexcept;

			ucstring print{};
		};
		print_vars_t print_vars_{};
		bool paused_{false};
		vector<history_vars_t> history{};
	};
}