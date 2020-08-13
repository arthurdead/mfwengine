#ifndef MFW_PRIVATE_CORE_SCANNER_HPP
#define MFW_PRIVATE_CORE_SCANNER_HPP

#pragma once

#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/core/univalue.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/stl/unordered_map.hpp>
#include <public/mfw/stl/functional.hpp>

namespace mfw::core
{
	class scanner
	{
	public:
		static scanner &instance();

		scanner() = default;
		~scanner() = default;

		struct token
		{
			enum class flags : int16_t
			{
				none = 0,

				equal = MFW_BIT(1),

				left = MFW_BIT(2),
				right = MFW_BIT(3),

				repeat = MFW_BIT(4),

				bool_ = MFW_BIT(5),
				string  = MFW_BIT(6),
				number = MFW_BIT(7),
				floating = MFW_BIT(8),
				integer = MFW_BIT(9),
			};
			MFW_CLASS_ENUM_FLAGS(flags)

			enum class type : uchar_t
			{
				unknown,

				assign,
				add,
				subtract,
				divide,
				multiply,
				modulo,

				logic_not,

				bit_not,
				bit_xor,
				bit_or,
				bit_and,

				size_compare,

				comma,
				dot,
				semicolon,
				slash,
				ternary,
				colon,
				at,
				dollar,
				hashtag,

				parenthesis,
				brace,
				bracket,

				identifier,

				newline,
				whitespace,

				count,
			};

			enum class extended_type : uchar_t
			{
				unknown,

				plus_plus,
				minus_minus,
				bit_shift_left,
				bit_shift_right,
				double_colon,
				logic_or,
				logic_and,

				plus_equal,
				minus_equal,
				multiply_equal,
				divide_equal,
				not_equal,
				equal,
				lesser_equal,
				greater_equal,
				modulo_equal,
				bit_shift_left_equal,
				bit_shift_right_equal,
				bit_xor_equal,
				bit_and_equal,
				bit_or_equal,

				lesser,
				greater,
				parenthesis_left,
				parenthesis_right,
				brace_left,
				brace_right,
				bracket_left,
				bracket_right,

				unary_logic_not,
				unary_plus,
				unary_minus,
				unary_multiply,
				unary_bit_and,
				unary_bit_not,

				space,
				tab,
			};

			static const pair<ucstring_view, ucstring_view> &get_str(type type_);
			static const ucstring_view &get_side_str(type type_, bool left);

			void to_string(ucstring &str) const;
			ucstring as_string() const;

			static bool can_repeat(type type_);
			static bool can_equal(type type_);
			static bool can_equal_after_repeat(type type_);
			static bool has_side(type type_);

			bool is_string() const {
				return ((type_ == type::identifier) && bool_cast(flags_ & flags::string));
			}
			bool is_bool() const {
				return ((type_ == type::identifier) && bool_cast(flags_ & flags::bool_));
			}
			bool is_float() const {
				return ((type_ == type::identifier) && bool_cast(flags_ & flags::floating));
			}
			bool is_int() const {
				return ((type_ == type::identifier) && bool_cast(flags_ & flags::integer));
			}
			bool is_number() const {
				return ((type_ == type::identifier) && bool_cast(flags_ & flags::number));
			}
			bool is_identifier() const {
				return ((type_ == type::identifier) && (
					!bool_cast(flags_ & flags::string) &&
					!bool_cast(flags_ & flags::number)
				));
			}

			token() = default;
			token(type typ) : type_{typ} {}
			token(type typ, flags flag) : type_{typ}, flags_{flag} {}
			token(type typ, const univalue &val) : type_{typ}, value{val} {}
			token(extended_type typ) { from_extended_type(typ, *this); }

			token(const token &other) = default;
			token(token &&other) = default;
			token &operator=(const token &other) = default;
			token &operator=(token &&other) = default;

			static void from_extended_type(extended_type type_, token &token_);
			void from_extended_type(extended_type _type_) { from_extended_type(_type_, *this); }
			extended_type get_extended_type() const;

			bool is_extended_type(extended_type extype) const { return get_extended_type() == extype; }

			size_t get_precedence(bool unary) const;

			enum class associative : uchar_t
			{
				unknown,
				left_to_right,
				right_to_left,
			};

			associative get_associative(bool unary) const;

			static const token &empty() { 
				static const token empttok{};
				return empttok;
			}

			bool operator==(const token &other) const;
			bool operator!=(const token &other) const { return !operator==(other); }

			bool valid() const { return (type_ != type::unknown) || !value.empty(); }
			void clear() {
				type_ = type::unknown;
				flags_ = flags::none;
				value.clear();
				line = 0;
				offset = 0;
			}

			type type_{type::unknown};
			flags flags_{flags::none};
			univalue value{};
			size_t line{0};
			size_t offset{0};
		};

		enum class ignore_flags : uchar_t
		{
			none = 0,
			newline = MFW_BIT(0),
			whitespace = MFW_BIT(1),
			divide = MFW_BIT(2),
			slash = MFW_BIT(3),
			colon = MFW_BIT(4),
			dot = MFW_BIT(5),
			default_ = newline|whitespace,
			all = newline|whitespace|divide|slash|colon|dot,
		};
		MFW_CLASS_ENUM_FLAGS(ignore_flags)

		using token_vec_t = vector<token>;
		bool parse(const ucstring_view &str, token_vec_t &tokens, ignore_flags flags = ignore_flags::default_);

		bool contains_special_chars(const ucstring &str);
		bool contains_special_chars(const univalue &val)
		{ return contains_special_chars(val.get_string()); }

	private:
		void clear();

		void error(const ucstring_view &str);

		bool is_whitespace(ucchar_t c) const;
		void skip_comments();
		void skip_whitespace();
		void skip_all() {
			skip_whitespace();
			skip_comments();
		}

		void advance(ssize_t num = 1);
		bool finished() const;
		ucchar_t char_() const;
		ucchar_t peek(ssize_t count = 1);
		void seek(size_t num);
		size_t absolute_offset() const;
		size_t offset() const;
		size_t line() const;

		bool expect(const ucstring_view &str);
		bool expect(const ucstring_view &str, size_t &found);
		bool expect(const ucstring_view &str, ucstring &found);
		bool expect(ucchar_t expected);
		void read(size_t count, ucstring &str);
		void peek(ssize_t num, size_t count, ucstring &str);
		void peek(size_t count_, ucstring &str) { peek(0, count_, str); }
		void read(ucchar_t count, ucstring &str) = delete;
		bool read(const ucstring_view &open, const ucstring_view &close, ucstring &str);
		bool read(const ucstring_view &open, ucstring &str);
		void read(function<bool()> util, ucstring &str);
		ucchar_t read();

		bool check_token(const pair<ucstring_view, ucstring_view> &expect, token::type type_, token &token_);
		bool check_token(const ucstring_view &expect, token::type type_, token &token_);

		bool stops_identifier(bool number, bool &floating);

		bool read_string(token &tok);

		void parse_main();
		void parse_tokens();

		enum /*class*/ iterators : uchar_t
		{
			start,
			end,
			current,
			line_start,
			count,
		};
		MFW_CLASS_ENUM(iterators)

		bool error_{false};
		using const_iterator = ucstring_view::const_iterator;
		const_iterator iterators_[static_cast<size_t>(iterators::count)]{};

		size_t line_{0};

		ignore_flags flags_{ignore_flags::default_};

		token_vec_t *tokens{nullptr};

		using type_str_map_t = unordered_map<token::type, pair<ucstring_view, ucstring_view>>;
		static const type_str_map_t &type_str_map();
	};

	void to_string(const scanner::token_vec_t &subtokens, ucstring &str);
}

#endif