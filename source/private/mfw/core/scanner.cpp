#include <private/mfw/core/scanner.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <public/mfw/core/globals.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/core/univalue.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/pch_literals.hpp>

namespace mfw::core
{
	MFW_DECLARE_GLOBAL_ALLOCATOR(scanner, scanner)

	scanner &scanner::instance() {
		return __scanner_global_allocator.instance();
	}

	void scanner::clear()
	{
		flags_ = ignore_flags::default_;
		line_ = 0;
		tokens = nullptr;

		for(size_t i{0}; i < static_cast<size_t>(iterators::count); i++) {
			iterators_[i] = const_iterator{};
		}
	}

	bool scanner::parse(const ucstring_view &str, token_vec_t &tokens_, ignore_flags flags)
	{
		if(str.empty()) {
			return false;
		}

		clear();

		iterators_[iterators::start] = str.cbegin();
		iterators_[iterators::end] = str.cend();
		iterators_[iterators::current] = iterators_[iterators::start];
		iterators_[iterators::line_start] = iterators_[iterators::current];
		tokens = &tokens_;
		flags_ = flags;

		parse_main();

		return !error_;
	}

	bool scanner::finished() const
	{
		return (error_ || iterators_[iterators::current] == iterators_[iterators::end]);
	}

	size_t scanner::line() const
	{
		return line_;
	}

	size_t scanner::offset() const
	{
		return static_cast<size_t>(::MFW_STD_NAMESPACE::distance(iterators_[iterators::line_start], iterators_[iterators::current]));
	}

	size_t scanner::absolute_offset() const
	{
		return static_cast<size_t>(::MFW_STD_NAMESPACE::distance(iterators_[iterators::start], iterators_[iterators::current]));
	}

	ucchar_t scanner::char_() const
	{
		if(finished()) {
			return u'\0';
		}

		return *iterators_[iterators::current];
	}

	void scanner::seek(size_t num)
	{
		iterators_[iterators::current] = iterators_[iterators::start];
		advance(static_cast<ssize_t>(num));
	}

	ucchar_t scanner::peek(ssize_t count_)
	{
		bool negative{count_ < 0};

		if(((iterators_[iterators::current] == iterators_[iterators::end]) && !negative) ||
			((iterators_[iterators::current] == iterators_[iterators::start]) && negative) ||
			(count_ == 0) || error_) {
			return char_();
		}

		advance(count_);
		ucchar_t c{char_()};
		advance(-count_);
		return c;
	}

	void scanner::advance(ssize_t num)
	{
		bool negative{num < 0};
		if(negative) {
			num = -num;
		}

		if(((iterators_[iterators::current] == iterators_[iterators::end]) && !negative) ||
			((iterators_[iterators::current] == iterators_[iterators::start]) && negative) ||
			(num == 0) || error_) {
			return;
		}

		for(ssize_t i{0}; i < num; i++) {
			if(negative) {
				iterators_[iterators::current]--;
				if(iterators_[iterators::current] == iterators_[iterators::start]) {
					break;
				}
			} else {
				iterators_[iterators::current]++;
				if(iterators_[iterators::current] == iterators_[iterators::end]) {
					break;
				}
			}
		}
	}

	void scanner::parse_main()
	{
		while(true)
		{
			if(finished()) {
				break;
			}

			parse_tokens();
		}
	}

	const ucstring_view &scanner::token::get_side_str(type type_, bool left)
	{
		const pair<ucstring_view, ucstring_view> &str{get_str(type_)};
		if(left) {
			return str.first;
		} else {
			return str.second;
		}
	}

	void scanner::token::to_string(ucstring &str) const
	{
		if(!value.empty()) {
			str.clear();
			if(bool_cast(flags_ & flags::string)) {
				str += u'"';
			}
			const ucstring &val_str{value.get_string()};
			str += val_str;
			if(bool_cast(flags_ & flags::string)) {
				str += u'"';
			}
		} else {
			ucstring_view side{};
			if(bool_cast(flags_ & flags::left)) {
				side = get_side_str(type_, true);
			} else if(bool_cast(flags_ & flags::right)) {
				side = get_side_str(type_, false);
			} else {
				side = get_side_str(type_, true);
			}

			str = side;

			if(bool_cast(flags_ & flags::repeat)) {
				str += side;
			}
			if(bool_cast(flags_ & flags::equal)) {
				str += get_side_str(type::assign, true);
			}
		}
	}

	ucstring scanner::token::as_string() const
	{
		ucstring str{};
		to_string(str);
		return str;
	}

	bool scanner::token::can_repeat(type type_)
	{
		if(type_ == type::add || type_ == type::size_compare ||
			type_ == type::subtract || type_ == type::bit_and ||
			type_ == type::bit_or || type_ == type::assign ||
			type_ == type::colon) {
			return true;
		}

		return false;
	}

	bool scanner::token::can_equal(type type_)
	{
		if(type_ == type::add || type_ == type::size_compare ||
			type_ == type::subtract || type_ == type::bit_and ||
			type_ == type::bit_or || type_ == type::multiply ||
			type_ == type::divide || type_ == type::bit_xor ||
			type_ == type::modulo || type_ == type::logic_not) {
			return true;
		}

		return false;
	}

	bool scanner::token::can_equal_after_repeat(type type_)
	{
		if(type_ == type::size_compare) {
			return true;
		}

		return false;
	}

	bool scanner::token::has_side(type type_)
	{
		if(type_ == type::parenthesis || type_ == type::brace ||
			type_ == type::bracket || type_ == type::size_compare ||
			type_ == type::whitespace) {
			return true;
		}

		return false;
	}

	void scanner::token::from_extended_type(extended_type type_, token &token_)
	{
		if(type_ == extended_type::plus_plus) {
			token_.type_ = type::add;
			token_.flags_ |= flags::repeat;
		} else if(type_ == extended_type::minus_minus) {
			token_.type_ = type::subtract;
			token_.flags_ |= flags::repeat;
		} else if(type_ == extended_type::bit_shift_left) {
			token_.type_ = type::size_compare;
			token_.flags_ |= flags::repeat|flags::left;
		} else if(type_ == extended_type::bit_shift_right) {
			token_.type_ = type::size_compare;
			token_.flags_ |= flags::repeat|flags::right;
		} else if(type_ == extended_type::logic_and) {
			token_.type_ = type::bit_and;
			token_.flags_ |= flags::repeat;
		} else if(type_ == extended_type::logic_or) {
			token_.type_ = type::bit_or;
			token_.flags_ |= flags::repeat;
		} else if(type_ == extended_type::double_colon) {
			token_.type_ = type::colon;
			token_.flags_ |= flags::repeat;
		} else if(type_ == extended_type::plus_equal) {
			token_.type_ = type::add;
			token_.flags_ |= flags::equal;
		} else if(type_ == extended_type::minus_equal) {
			token_.type_ = type::subtract;
			token_.flags_ |= flags::equal;
		} else if(type_ == extended_type::multiply_equal) {
			token_.type_ = type::multiply;
			token_.flags_ |= flags::equal;
		} else if(type_ == extended_type::divide_equal) {
			token_.type_ = type::divide;
			token_.flags_ |= flags::equal;
		} else if(type_ == extended_type::not_equal) {
			token_.type_ = type::logic_not;
			token_.flags_ |= flags::equal;
		} else if(type_ == extended_type::equal) {
			token_.type_ = type::assign;
			token_.flags_ |= flags::equal|flags::repeat;
		} else if(type_ == extended_type::lesser_equal) {
			token_.type_ = type::size_compare;
			token_.flags_ |= flags::equal|flags::left;
		} else if(type_ == extended_type::greater_equal) {
			token_.type_ = type::size_compare;
			token_.flags_ |= flags::equal|flags::right;
		} else if(type_ == extended_type::modulo_equal) {
			token_.type_ = type::modulo;
			token_.flags_ |= flags::equal;
		} else if(type_ == extended_type::bit_shift_left_equal) {
			token_.type_ = type::size_compare;
			token_.flags_ |= flags::repeat|flags::left|flags::equal;
		} else if(type_ == extended_type::bit_shift_right_equal) {
			token_.type_ = type::size_compare;
			token_.flags_ |= flags::repeat|flags::right|flags::equal;
		} else if(type_ == extended_type::bit_xor_equal) {
			token_.type_ = type::bit_xor;
			token_.flags_ |= flags::equal;
		} else if(type_ == extended_type::bit_and_equal) {
			token_.type_ = type::bit_and;
			token_.flags_ |= flags::equal;
		} else if(type_ == extended_type::bit_or_equal) {
			token_.type_ = type::bit_or;
			token_.flags_ |= flags::equal;
		} else if(type_ == extended_type::parenthesis_left) {
			token_.type_ = type::parenthesis;
			token_.flags_ |= flags::left;
		} else if(type_ == extended_type::parenthesis_right) {
			token_.type_ = type::parenthesis;
			token_.flags_ |= flags::right;
		} else if(type_ == extended_type::brace_left) {
			token_.type_ = type::brace;
			token_.flags_ |= flags::left;
		} else if(type_ == extended_type::brace_right) {
			token_.type_ = type::brace;
			token_.flags_ |= flags::right;
		} else if(type_ == extended_type::bracket_left) {
			token_.type_ = type::bracket;
			token_.flags_ |= flags::left;
		} else if(type_ == extended_type::bracket_right) {
			token_.type_ = type::bracket;
			token_.flags_ |= flags::right;
		} else if(type_ == extended_type::lesser) {
			token_.type_ = type::size_compare;
			token_.flags_ |= flags::left;
		} else if(type_ == extended_type::greater) {
			token_.type_ = type::size_compare;
			token_.flags_ |= flags::right;
		} else if(type_ == extended_type::unary_logic_not) {
			token_.type_ = type::logic_not;
			token_.flags_ = flags::none;
		} else if(type_ == extended_type::unary_plus) {
			token_.type_ = type::add;
			token_.flags_ = flags::none;
		} else if(type_ == extended_type::unary_minus) {
			token_.type_ = type::subtract;
			token_.flags_ = flags::none;
		} else if(type_ == extended_type::unary_multiply) {
			token_.type_ = type::multiply;
			token_.flags_ = flags::none;
		} else if(type_ == extended_type::unary_bit_and) {
			token_.type_ = type::bit_and;
			token_.flags_ = flags::none;
		} else if(type_ == extended_type::unary_bit_not) {
			token_.type_ = type::bit_not;
			token_.flags_ = flags::none;
		} else if(type_ == extended_type::space) {
			token_.type_ = type::whitespace;
			token_.flags_ |= flags::right;
		} else if(type_ == extended_type::tab) {
			token_.type_ = type::whitespace;
			token_.flags_ |= flags::left;
		} else {
			token_.clear();
		}
	}

	scanner::token::extended_type scanner::token::get_extended_type() const
	{
		if(type_ == type::add) {
			if(bool_cast(flags_ & flags::repeat)) {
				return extended_type::plus_plus;
			} else if(bool_cast(flags_ & flags::equal)) {
				return extended_type::plus_equal;
			} /*else if(flags_ == flags::none) {
				return extended_type::unary_plus;
			}*/
		} else if(type_ == type::subtract) {
			if(bool_cast(flags_ & flags::repeat)) {
				return extended_type::minus_minus;
			} else if(bool_cast(flags_ & flags::equal)) {
				return extended_type::minus_equal;
			} /*else if(flags_ == flags::none) {
				return extended_type::unary_minus;
			}*/
		} else if(type_ == type::bit_and) {
			if(bool_cast(flags_ & flags::repeat)) {
				return extended_type::logic_and;
			} /*else if(flags_ == flags::none) {
				return extended_type::unary_bit_and;
			}*/
		} else if(type_ == type::bit_or) {
			if(bool_cast(flags_ & flags::repeat)) {
				return extended_type::logic_or;
			}
		} /*else if(type_ == type::bit_not) {
			if(flags_ == flags::none) {
				return extended_type::unary_bit_not;
			}
		}*/ else if(type_ == type::multiply) {
			if(bool_cast(flags_ & flags::equal)) {
				return extended_type::multiply_equal;
			}/* else if(flags_ == flags::none) {
				return extended_type::unary_multiply;
			}*/
		} else if(type_ == type::divide) {
			if(bool_cast(flags_ & flags::equal)) {
				return extended_type::divide_equal;
			}
		} else if(type_ == type::modulo) {
			if(bool_cast(flags_ & flags::equal)) {
				return extended_type::modulo_equal;
			}
		} else if(type_ == type::colon) {
			if(bool_cast(flags_ & flags::repeat)) {
				return extended_type::double_colon;
			}
		} else if(type_ == type::size_compare) {
			if(bool_cast(flags_ & flags::left)) {
				if(bool_cast(flags_ & flags::repeat)) {
					if(bool_cast(flags_ & flags::equal)) {
						return extended_type::bit_shift_left_equal;
					} else {
						return extended_type::bit_shift_left;
					}
				} else {
					if(bool_cast(flags_ & flags::equal)) {
						return extended_type::lesser_equal;
					} else {
						return extended_type::lesser;
					}
				}
			} else if(bool_cast(flags_ & flags::right)) {
				if(bool_cast(flags_ & flags::repeat)) {
					if(bool_cast(flags_ & flags::equal)) {
						return extended_type::bit_shift_right_equal;
					} else {
						return extended_type::bit_shift_right;
					}
				} else {
					if(bool_cast(flags_ & flags::equal)) {
						return extended_type::greater_equal;
					} else {
						return extended_type::greater;
					}
				}
			}
		} else if(type_ == type::logic_not) {
			if(bool_cast(flags_ & flags::equal)) {
				return extended_type::not_equal;
			}/* else if(flags_ == flags::none) {
				return extended_type::unary_logic_not;
			}*/
		} else if(type_ == type::assign) {
			if(bool_cast(flags_ & flags::equal) || bool_cast(flags_ & flags::repeat)) {
				return extended_type::equal;
			}
		} else if(type_ == type::bit_xor) {
			if(bool_cast(flags_ & flags::equal)) {
				return extended_type::bit_xor_equal;
			}
		} else if(type_ == type::parenthesis) {
			if(bool_cast(flags_ & flags::left)) {
				return extended_type::parenthesis_left;
			} else if(bool_cast(flags_ & flags::right)) {
				return extended_type::parenthesis_right;
			}
		} else if(type_ == type::brace) {
			if(bool_cast(flags_ & flags::left)) {
				return extended_type::brace_left;
			} else if(bool_cast(flags_ & flags::right)) {
				return extended_type::brace_right;
			}
		} else if(type_ == type::bracket) {
			if(bool_cast(flags_ & flags::left)) {
				return extended_type::bracket_left;
			} else if(bool_cast(flags_ & flags::right)) {
				return extended_type::bracket_right;
			}
		} else if(type_ == type::whitespace) {
			if(bool_cast(flags_ & flags::left)) {
				return extended_type::tab;
			} else if(bool_cast(flags_ & flags::right)) {
				return extended_type::space;
			}
		}

		return extended_type::unknown;
	}

	bool scanner::check_token(const pair<ucstring_view, ucstring_view> &expect_, token::type type_, token &token_)
	{
		if(finished() || type_ == token::type::unknown || expect_.first.empty()) {
			token_.clear();
			return false;
		}

		bool has_side{token::has_side(type_) && (expect_.first != expect_.second)};

		skip_all();

		ucstring found{};
		bool did{false};
		if(!has_side) {
			did = expect(expect_.first, found);
		} else {
			did = (expect(expect_.first, found) || expect(expect_.second, found));
		}

		if(did) {
			token_.type_ = type_;

			if(has_side) {
				if(found == token::get_side_str(token_.type_, true)) {
					token_.flags_ |= token::flags::left;
				} else if(found == token::get_side_str(token_.type_, false)) {
					token_.flags_ |= token::flags::right;
				}
			}

			if(token::can_repeat(token_.type_) || token::can_equal(token_.type_)) {
				skip_all();

				bool repeated{false};
				if(token::can_repeat(token_.type_)) {
					if(expect(found)) {
						repeated = true;
						token_.flags_ |= token::flags::repeat;
					}
				}

				skip_all();

				if(token::can_equal(token_.type_) && (!repeated || (repeated && token::can_equal_after_repeat(token_.type_)))) {
					if(expect(token::get_side_str(token::type::assign, true))) {
						token_.flags_ |= token::flags::equal;
					}
				}
			}

			return true;
		} else {
			return false;
		}
	}

	bool scanner::check_token(const ucstring_view &expect, token::type type_, token &token_)
	{
		return check_token({expect, expect}, type_, token_);
	}

	const scanner::type_str_map_t &scanner::type_str_map() {
		static const type_str_map_t __type_str_map{
			{token::type::assign, {u8"="_sv, {}}},
			{token::type::add, {u8"+"_sv, {}}},
			{token::type::subtract, {u8"-"_sv, {}}},
			{token::type::divide, {u8"/"_sv, {}}},
			{token::type::multiply, {u8"*"_sv, {}}},
			{token::type::modulo, {u8"%"_sv, {}}},
			{token::type::logic_not, {u8"!"_sv, {}}},
			{token::type::bit_or, {u8"|"_sv, {}}},
			{token::type::bit_and, {u8"&"_sv, {}}},
			{token::type::bit_not, {u8"~"_sv, {}}},
			{token::type::bit_xor, {u8"^"_sv, {}}},
			{token::type::size_compare, {u8"<"_sv, u8">"_sv}},
			{token::type::comma, {u8","_sv, {}}},
			{token::type::dot, {u8"."_sv, {}}},
			{token::type::semicolon, {u8";"_sv, {}}},
			{token::type::slash, {u8"\\"_sv, {}}},
			{token::type::ternary, {u8"?"_sv, {}}},
			{token::type::colon, {u8":"_sv, {}}},
			{token::type::at, {u8"@"_sv, {}}},
			{token::type::dollar, {u8"$"_sv, {}}},
			{token::type::hashtag, {u8"#"_sv, {}}},
			{token::type::parenthesis, {u8"("_sv, u8")"_sv}},
			{token::type::brace, {u8"{"_sv, u8"}"_sv}},
			{token::type::bracket, {u8"["_sv, u8"]"_sv}},
			{token::type::newline, {u8"\n"_sv, {}}},
			{token::type::whitespace, {u8"\t"_sv, u8" "_sv}},
		};
		
		return __type_str_map;
	}

	const pair<ucstring_view, ucstring_view> &scanner::token::get_str(type type_)
	{
		const type_str_map_t &map{type_str_map()};
		type_str_map_t::const_iterator it{map.find(type_)};
		if(it == map.cend()) {
			static const pair<ucstring_view, ucstring_view> empty{};
			return empty;
		}
		return it->second;
	}

	size_t scanner::token::get_precedence(bool unary) const
	{
		extended_type extype{get_extended_type()};
		if(extype == extended_type::unknown) {
			if(type_ == type::dot) {
				return 2;
			} else if(unary && (type_ == type::add || type_ == type::subtract)) {
				return 3;
			} else if(!unary && (type_ == type::add || type_ == type::subtract)) {
				return 6;
			} else if(type_ == type::logic_not || type_ == type::bit_not) {
				return 3;
			} else if(unary && (type_ == type::multiply || type_ == type::bit_and)) {
				return 3;
			} else if(!unary && (type_ == type::multiply)) {
				return 5;
			} else if(type_ == type::divide || type_ == type::modulo) {
				return 5;
			} else if(type_ == type::bit_and) {
				return 11;
			} else if(type_ == type::bit_xor) {
				return 12;
			} else if(type_ == type::bit_or) {
				return 13;
			} else if(type_ == type::ternary || type_ == type::colon) {
				return 16;
			} else if(type_ == type::comma) {
				return 17;
			} else if(type_ == type::assign) {
				return 16;
			}
		} else if(extype == extended_type::double_colon) {
			return 1;
		} else if(!unary && (extype == extended_type::plus_plus || extype == extended_type::minus_minus)) {
			return 2;
		} else if(unary && (extype == extended_type::plus_plus || extype == extended_type::minus_minus)) {
			return 3;
		} else if(extype == extended_type::bit_shift_left || extype == extended_type::bit_shift_right) {
			return 7;
		} else if(extype == extended_type::lesser || extype == extended_type::lesser_equal ||
					extype == extended_type::greater || extype == extended_type::greater_equal) {
			return 9;
		} else if(extype == extended_type::equal || extype == extended_type::not_equal) {
			return 10;
		} else if(extype == extended_type::logic_and) {
			return 14;
		} else if(extype == extended_type::logic_or) {
			return 15;
		} else if(extype == extended_type::plus_equal || extype == extended_type::minus_equal ||
					extype == extended_type::multiply_equal || extype == extended_type::divide_equal ||
					extype == extended_type::modulo_equal || extype == extended_type::bit_shift_left_equal ||
					extype == extended_type::bit_shift_right_equal || extype == extended_type::bit_and_equal ||
					extype == extended_type::bit_xor_equal || extype == extended_type::bit_or_equal) {
			return 16;
		} else if(extype == extended_type::parenthesis_left || extype == extended_type::parenthesis_right ||
				  extype == extended_type::bracket_left || extype == extended_type::bracket_right ||
				  extype == extended_type::brace_left || extype == extended_type::brace_right) {
			return 2;
		}

		return 0;
	}

	scanner::token::associative scanner::token::get_associative(bool unary) const
	{
		extended_type extype{get_extended_type()};
		if(extype == extended_type::unknown) {
			if(type_ == type::dot) {
				return associative::left_to_right;
			} else if(unary && (type_ == type::add || type_ == type::subtract)) {
				return associative::right_to_left;
			} else if(!unary && (type_ == type::add || type_ == type::subtract)) {
				return associative::left_to_right;
			} else if(type_ == type::logic_not || type_ == type::bit_not) {
				return associative::right_to_left;
			} else if(unary && (type_ == type::multiply || type_ == type::bit_and)) {
				return associative::right_to_left;
			} else if(!unary && (type_ == type::multiply)) {
				return associative::left_to_right;
			} else if(type_ == type::divide || type_ == type::modulo) {
				return associative::left_to_right;
			} else if(type_ == type::bit_and) {
				return associative::left_to_right;
			} else if(type_ == type::bit_xor) {
				return associative::left_to_right;
			} else if(type_ == type::bit_or) {
				return associative::left_to_right;
			} else if(type_ == type::ternary || type_ == type::colon) {
				return associative::right_to_left;
			} else if(type_ == type::comma) {
				return associative::left_to_right;
			} else if(type_ == type::assign) {
				return associative::right_to_left;
			}
		} else if(extype == extended_type::double_colon) {
			return associative::left_to_right;
		} else if(!unary && (extype == extended_type::plus_plus || extype == extended_type::minus_minus)) {
			return associative::left_to_right;
		} else if(unary && (extype == extended_type::plus_plus || extype == extended_type::minus_minus)) {
			return associative::right_to_left;
		} else if(extype == extended_type::bit_shift_left || extype == extended_type::bit_shift_right) {
			return associative::left_to_right;
		} else if(extype == extended_type::lesser || extype == extended_type::lesser_equal ||
					extype == extended_type::greater || extype == extended_type::greater_equal) {
			return associative::left_to_right;
		} else if(extype == extended_type::equal || extype == extended_type::not_equal) {
			return associative::left_to_right;
		} else if(extype == extended_type::logic_and) {
			return associative::left_to_right;
		} else if(extype == extended_type::logic_or) {
			return associative::left_to_right;
		} else if(extype == extended_type::plus_equal || extype == extended_type::minus_equal ||
					extype == extended_type::multiply_equal || extype == extended_type::divide_equal ||
					extype == extended_type::modulo_equal || extype == extended_type::bit_shift_left_equal ||
					extype == extended_type::bit_shift_right_equal || extype == extended_type::bit_and_equal ||
					extype == extended_type::bit_xor_equal || extype == extended_type::bit_or_equal) {
			return associative::right_to_left;
		} else if(extype == extended_type::parenthesis_left || extype == extended_type::parenthesis_right ||
				  extype == extended_type::bracket_left || extype == extended_type::bracket_right ||
				  extype == extended_type::brace_left || extype == extended_type::brace_right) {
			return associative::left_to_right;
		}

		return associative::unknown;
	}

	bool scanner::token::operator==(const token &other) const
	{
		constexpr flags uselessflags{flags::bool_|flags::string|flags::number|flags::floating|flags::integer};

		if(type_ == other.type_) {
			if((flags_ & ~uselessflags) == (other.flags_ & ~uselessflags)) {
				if(other.value.empty()) {
					return true;
				} else {
					return (value == other.value);
				}
			}
		}

		return false;
	}

	bool scanner::stops_identifier(bool number, bool &floating)
	{
		if(finished()) {
			return true;
		}

		ucchar_t c{char_()};

		if(number) {
			if(c != u8'f' && isalpha(c)) {
				MFW_MESSAGE("hmmmmm")
				//return true;
				return false;
			} else if(c == u8'f') {
				floating = true;
				return false;
			} else if(c == u8'.') {
				floating = true;
				return false;
			}
		}

		if(is_whitespace(c)) {
			return true;
		}

		for(size_t i{0}; i < static_cast<size_t>(token::type::count); i++) {
			const pair<ucstring_view, ucstring_view> &type_str{token::get_str(static_cast<token::type>(i))};
			if(type_str.first.empty()) {
				continue;
			}

			if(static_cast<token::type>(i) == token::type::newline) {
				if(bool_cast(flags_ & ignore_flags::newline)) {
					continue;
				}
			} else if(static_cast<token::type>(i) == token::type::whitespace) {
				if(bool_cast(flags_ & ignore_flags::whitespace)) {
					continue;
				}
			} else if(static_cast<token::type>(i) == token::type::divide) {
				if(bool_cast(flags_ & ignore_flags::divide)) {
					continue;
				}
			} else if(static_cast<token::type>(i) == token::type::slash) {
				if(bool_cast(flags_ & ignore_flags::slash)) {
					continue;
				}
			} else if(static_cast<token::type>(i) == token::type::colon) {
				if(bool_cast(flags_ & ignore_flags::colon)) {
					continue;
				}
			} else if(static_cast<token::type>(i) == token::type::dot) {
				if(bool_cast(flags_ & ignore_flags::dot)) {
					continue;
				}
			}

			size_t size{0};
			if(expect(type_str.first, size) || expect(type_str.second, size)) {
				advance(-static_cast<ssize_t>(size));
				return true;
			}
		}

		return false;
	}

	bool scanner::read_string(token &tok)
	{
		skip_all();

		ucstring str{};
		if(read(u8"\""_sv, str) || read(u8"'"_sv, str)) {
			tok.type_ = token::type::identifier;
			tok.flags_ |= token::flags::string;
			tok.value = str;
			return true;
		}

		return false;
	}

	void scanner::parse_tokens()
	{
		if(finished()) {
			return;
		}

		token token_{};

		if(!read_string(token_)) {
			for(size_t i{0}; i < static_cast<size_t>(token::type::count); i++) {
				const pair<ucstring_view, ucstring_view> &type_str{token::get_str(static_cast<token::type>(i))};
				if(type_str.first.empty()) {
					continue;
				}

				if(static_cast<token::type>(i) == token::type::newline) {
					if(bool_cast(flags_ & ignore_flags::newline)) {
						continue;
					}
				} else if(static_cast<token::type>(i) == token::type::whitespace) {
					if(bool_cast(flags_ & ignore_flags::whitespace)) {
						continue;
					}
				} else if(static_cast<token::type>(i) == token::type::divide) {
					if(bool_cast(flags_ & ignore_flags::divide)) {
						continue;
					}
				} else if(static_cast<token::type>(i) == token::type::slash) {
					if(bool_cast(flags_ & ignore_flags::slash)) {
						continue;
					}
				} else if(static_cast<token::type>(i) == token::type::colon) {
					if(bool_cast(flags_ & ignore_flags::colon)) {
						continue;
					}
				} else if(static_cast<token::type>(i) == token::type::dot) {
					if(bool_cast(flags_ & ignore_flags::dot)) {
						continue;
					}
				}

				if(check_token(type_str, static_cast<token::type>(i), token_)) {
					if(static_cast<token::type>(i) == token::type::newline) {
						iterators_[iterators::line_start] = iterators_[iterators::current];
						line_++;
					}
					break;
				}
			}
		}

		if(token_.type_ == token::type::unknown) {
			ucstring identifier{};
			bool number{false};
			bool floating{false};
			if(iswdigit(char_())) {
				number = true;
			}
			read([this, number, &floating]() -> bool {
				if(stops_identifier(number, floating)) {
					return false;
				}
				return true;
			}, identifier);
			if(identifier.empty()) {
				skip_all();

				if(finished()) {
					return;
				}

				ucstring wtf{};

				while(true)
				{
					if(finished()) {
						break;
					}

					ucchar_t c{read()};
					wtf += c;
				}

				MFW_DEBUGBREAK();

				error(u8"unknown error");
				return;
			} else {
				token_.type_ = token::type::identifier;
				if(number) {
					token_.flags_ |= token::flags::number;
					if(floating) {
						token_.flags_ |= token::flags::floating;
					} else {
						token_.flags_ |= token::flags::integer;
					}
				} else {
					if(identifier == u8"true"_sv || identifier == u8"false"_sv) {
						token_.flags_ |= token::flags::bool_|token::flags::number|token::flags::integer;
					}
				}
				token_.value = identifier;
			}
		}

		if(token_.type_ != token::type::unknown) {
			bool skip{false};
			if(token_.type_ == token::type::newline) {
				if(bool_cast(flags_ & ignore_flags::newline)) {
					skip = true;
				}
			} else if(token_.type_ == token::type::whitespace) {
				if(bool_cast(flags_ & ignore_flags::whitespace)) {
					skip = true;
				}
			} else if(token_.type_ == token::type::divide) {
				if(bool_cast(flags_ & ignore_flags::divide)) {
					skip = true;
				}
			} else if(token_.type_ == token::type::slash) {
				if(bool_cast(flags_ & ignore_flags::slash)) {
					skip = true;
				}
			} else if(token_.type_ == token::type::colon) {
				if(bool_cast(flags_ & ignore_flags::colon)) {
					skip = true;
				}
			} else if(token_.type_ == token::type::dot) {
				if(bool_cast(flags_ & ignore_flags::dot)) {
					skip = true;
				}
			}

			if(!skip) {
				token_.line = line();
				token_.offset = offset();

				tokens->emplace_back(token_);
			}
		}
	}

	void scanner::skip_comments()
	{
		if(finished()) {
			return;
		}

		ucstring tmp{};

		while(true)
		{
			if(finished()) {
				break;
			}

			if(char_() == u8'/' && peek() == u8'/') {
				advance(2);

				while(true) {
					if(finished()) {
						break;
					}

					if(char_() == u8'\n') {
						skip_whitespace();
						break;
					} else {
						advance();
					}
				}
			} else if(char_() == u8'/' && peek() == u8'*') {
				advance(2);

				while(true) {
					if(finished()) {
						error(u8"multiline comment never closed"_sv);
						return;
					}

					if(char_() == u8'*' && peek() == u8'/') {
						advance(2);
						skip_whitespace();
						break;
					} else {
						advance();
					}
				}
			} else {
				break;
			}
		}
	}

	void scanner::skip_whitespace()
	{
		if(finished()) {
			return;
		}

		while(true)
		{
			if(finished()) {
				break;
			}

			if(is_whitespace(char_())) {
				advance();
			} else {
				break;
			}
		}
	}

	bool scanner::is_whitespace(ucchar_t c) const
	{
		if(bool_cast(flags_ & ignore_flags::whitespace)) {
			if(c == u8'\t' || c == u8' ') {
				return true;
			}
		}
		if(bool_cast(flags_ & ignore_flags::newline)) {
			if(c == u8'\n') {
				return true;
			}
		}

		return (c == u8'\0' || c == u8'\r');
	}

	void scanner::error(const ucstring_view &)
	{
		error_ = true;
	}

	bool scanner::expect(ucchar_t expected)
	{
		if(finished() || expected == u8'\0') {
			return false;
		}

		ucchar_t c{read()};
		if(c == expected) {
			return true;
		} else {
			advance(-1);
			return false;
		}
	}

	bool scanner::expect(const ucstring_view &str)
	{
		if(finished() || str.empty()) {
			return false;
		}

		size_t size{0};
		return expect(str, size);
	}

	bool scanner::expect(const ucstring_view &str, size_t &size)
	{
		if(finished() || str.empty()) {
			size = 0;
			return false;
		}

		ucstring tmp{};
		bool did{expect(str, tmp)};
		if(did) {
			size = tmp.length();
		} else {
			size = 0;
		}
		return did;
	}

	bool scanner::expect(const ucstring_view &str, ucstring &found)
	{
		if(finished() || str.empty()) {
			found.clear();
			return false;
		}

		found.clear();
		read(str.length(), found);
		if(str == found) {
			return true;
		} else {
			advance(-static_cast<ssize_t>(str.length()));
			return false;
		}
	}

	ucchar_t scanner::read()
	{
		if(finished()) {
			return u8'\0';
		}

		ucchar_t c{char_()};
		advance();
		return c;
	}

	void scanner::read(function<bool()> util, ucstring &str)
	{
		if(finished()) {
			str.clear();
			return;
		}

		str.clear();

		while(true)
		{
			if(finished()) {
				break;
			}

			if(!util()) {
				break;
			}

			str += read();
		}
	}

	void scanner::read(size_t count_, ucstring &str)
	{
		if(finished() || count_ <= 0) {
			str.clear();
			return;
		}

		size_t i{0};
		read([&i, count_]() -> bool {
			if(i >= count_) {
				return false;
			}
			i++;
			return true;
		}, str);
	}

	void scanner::peek(ssize_t num, size_t count_, ucstring &str)
	{
		if(finished()) {
			str.clear();
			return;
		}

		advance(num);
		read(count_, str);
		advance(-(num + count_));
	}

	bool scanner::read(const ucstring_view &open, ucstring &str)
	{
		if(finished()) {
			str.clear();
			return false;
		}

		bool first_escaped{expect(u8'\\')};

		if(!expect(open)) {
			str.clear();
			return false;
		}

		if(first_escaped) {
			str += open;
		}

		ucstring found{};

		bool valid{false};
		read([&]() -> bool {
			bool escaped{char_() == u8'\\' && peek(-1) != u8'\\'};
			size_t num{escaped ? 1u : 0u};
			if(first_escaped == escaped) {
				peek(num, open.length(), found);
				if(found == open) {
					advance(open.length() + (num));
					valid = true;
					return false;
				}
			} else if(!first_escaped) {
				peek(num, open.length(), found);
				if(found == open) {
					advance(open.length() + (num));
					if(escaped) {
						str += open;
					}
				}
			}
			if(char_() == u8'\\' && peek(-1) == u8'\\') {
				peek(1, open.length(), found);
				if(found == open) {
					str.pop_back();
				}
			}
			return true;
		}, str);

		if(!valid) {
			str.clear();
		}

		return valid;
	}

	bool scanner::read(const ucstring_view &, const ucstring_view &, ucstring &)
	{
		MFW_MESSAGE("todo")
		MFW_DEBUGBREAK();
		return false;
	}

	void to_string(const scanner::token_vec_t &subtokens, ucstring &str)
	{
		scanner::token_vec_t::const_iterator it{subtokens.cbegin()};
		while(it != subtokens.cend()) {
			const scanner::token &tok{*it};
			str += tok.as_string();
			
			bool lastone{it == (subtokens.cend()-1)};

			const scanner::token &next{lastone ? *it : *(it + 1)};

			scanner::token::extended_type extype{tok.get_extended_type()};
			scanner::token::extended_type nextextype{next.get_extended_type()};

			bool dospace{
				nextextype != scanner::token::extended_type::parenthesis_right &&
				nextextype != scanner::token::extended_type::bracket_right &&
				nextextype != scanner::token::extended_type::brace_right &&
				next.type_ != scanner::token::type::dot &&

				nextextype != scanner::token::extended_type::parenthesis_left &&
				nextextype != scanner::token::extended_type::bracket_left &&
				nextextype != scanner::token::extended_type::brace_left &&

				extype != scanner::token::extended_type::parenthesis_left &&
				extype != scanner::token::extended_type::bracket_left &&
				extype != scanner::token::extended_type::brace_left &&

				tok.type_ != scanner::token::type::logic_not &&
				tok.type_ != scanner::token::type::dot &&
				tok.type_ != scanner::token::type::dollar &&

				!lastone
			};

			if(dospace) {
				str += u8' ';
			}

			it++;
		}
	}

	bool scanner::contains_special_chars(const ucstring &str)
	{
		for(size_t i{0}; i < static_cast<size_t>(token::type::count); i++) {
			const pair<ucstring_view, ucstring_view> &type_str{token::get_str(static_cast<token::type>(i))};
			if(type_str.first.empty()) {
				continue;
			}

			if(str.find(type_str.first, 0) != ucstring::npos) {
				return true;
			}

			if(type_str.second.empty()) {
				continue;
			}

			if(str.find(type_str.second, 0) != ucstring::npos) {
				return true;
			}
		}

		return false;
	}
}