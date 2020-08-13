#include <private/mfw/core/base_parser.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <private/mfw/core/expression_parser.hpp>
#include <private/mfw/core/scanner.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/core/univalue.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/stl/string.hpp>

namespace mfw::core
{
	void base_parser::clear()
	{
		for(size_t i{0}; i < static_cast<size_t>(iterators::count); i++) {
			iterators_[i] = const_iterator{};
		}
	}

	bool base_parser::parse(const ucstring_view &str, ignore_flags flags)
	{
		if(str.empty()) {
			return false;
		}

		token_vec_t tokens{};
		if(!scanner::instance().parse(str, tokens, flags)) {
			return false;
		}
		return parse(tokens);
	}

	bool base_parser::parse(const scanner::token_vec_t &tokens)
	{
		if(tokens.empty()) {
			return false;
		}

		iterators_[iterators::start] = tokens.cbegin();
		iterators_[iterators::end] = tokens.cend();
		iterators_[iterators::current] = iterators_[iterators::start];

		parse_main();

		return !error_;
	}

	bool base_parser::finished() const
	{
		return (error_ || iterators_[iterators::current] == iterators_[iterators::end]);
	}

	size_t base_parser::absolute_offset()
	{
		return distance(iterators_[iterators::start], iterators_[iterators::current]);
	}

	void base_parser::seek(size_t num)
	{
		iterators_[iterators::current] = iterators_[iterators::start];
		advance(static_cast<ssize_t>(num));
	}

	void base_parser::advance(ssize_t num)
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

	const base_parser::token &base_parser::token_() const
	{
		if(finished()) {
			return token::empty();
		}

		return *iterators_[iterators::current];
	}

	const base_parser::token &base_parser::peek(ssize_t count_)
	{
		bool negative{count_ < 0};

		if(((iterators_[iterators::current] == iterators_[iterators::end]) && !negative) ||
			((iterators_[iterators::current] == iterators_[iterators::start]) && negative) ||
			(count_ == 0) || error_) {
			return token_();
		}

		advance(count_);
		const token &tok{token_()};
		advance(-count_);
		return tok;
	}

	void base_parser::skip(skip_flags flags)
	{
		if(finished()) {
			return;
		}

		while(true)
		{
			if(finished()) {
				break;
			}

			const token &tok{token_()};

			bool skipped_any{false};
			if(bool_cast(flags & skip_flags::whitespace)) {
				if(tok.type_ == token::type::whitespace) {
					advance();
					skipped_any = true;
				}
			}
			if(bool_cast(flags & skip_flags::newline)) {
				if(tok.type_ == token::type::newline) {
					advance();
					skipped_any = true;
				}
			}
			if(bool_cast(flags & skip_flags::slash)) {
				if(tok.type_ == token::type::slash) {
					advance();
					skipped_any = true;
				}
			}
			if(bool_cast(flags & skip_flags::divide)) {
				if(tok.type_ == token::type::divide) {
					advance();
					skipped_any = true;
				}
			}
			if(bool_cast(flags & skip_flags::colon)) {
				if(tok.type_ == token::type::colon && tok.flags_ == token::flags::none) {
					advance();
					skipped_any = true;
				}
			}
			if(bool_cast(flags & skip_flags::dot)) {
				if(tok.type_ == token::type::dot) {
					advance();
					skipped_any = true;
				}
			}
			if(!skipped_any) {
				break;
			}
		}
	}

	bool base_parser::expect(const token &expected, token &found)
	{
		if(finished()) {
			found.clear();
			return false;
		}

		const token &tok{read()};
		bool ret{false};
		if(tok == expected) {
			found = tok;
			ret = true;
		} else {
			found.clear();
			advance(-1);
		}
		return ret;
	}

	bool base_parser::expect(const token &expected)
	{
		if(finished() || !expected.valid()) {
			return false;
		}

		token found{};
		return expect(expected, found);
	}

	bool base_parser::expect(const token &expected, univalue &found)
	{
		if(finished() || !expected.valid()) {
			found.clear();
			return false;
		}

		token tok{};
		bool ret{expect(expected, tok)};
		if(ret) {
			found = tok.value;
		} else {
			found.clear();
		}
		return ret;
	}

	const base_parser::token &base_parser::read()
	{
		if(finished()) {
			return token::empty();
		}

		const token &tok{token_()};
		advance();
		return tok;
	}

	void base_parser::read(function<bool()> util, token_vec_t &tokens)
	{
		if(finished()) {
			tokens.clear();
			return;
		}

		while(true) {
			if(finished()) {
				break;
			}

			if(!util()) {
				break;
			}

			tokens.emplace_back(read());
		}
	}

	void base_parser::read(size_t count_, token_vec_t &tokens)
	{
		if(finished()) {
			tokens.clear();
			return;
		}

		size_t i{0};
		read([&i, count_]() -> bool {
			if(i >= count_) {
				return false;
			}
			i++;
			return true;
		}, tokens);
	}

	bool base_parser::read(const token &open, const token &close, token_vec_t &tokens)
	{
		if(finished() || !expect(open)) {
			tokens.clear();
			return false;
		}

		size_t closenum{1};

		bool valid{false};
		read([&]() -> bool {
			const token &tok{token_()};
			if(tok == open) {
				closenum++;
			} else if(tok == close) {
				closenum--;
				if(closenum == 0) {
					advance();
					valid = true;
					return false;
				}
			}
			return true;
		}, tokens);

		if(!valid) {
			tokens.clear();
		}

		return valid;
	}

	void base_parser::error(const ucstring_view &str)
	{
		error_ = true;
	}

	bool base_parser::read_function(vector<univalue> &args, const interfaces::expression_parser_callbacks *callbacks)
	{
		if(finished()) {
			args.clear();
			return false;
		}

		token_vec_t tokens{};
		if(!read({token::extended_type::parenthesis_left}, {token::extended_type::parenthesis_right}, tokens)) {
			args.clear();
			return false;
		}

		token_vec_t argtokens{};

		expression_parser subparser{};

		for(const token &it : tokens) {
			if(it.type_ == token::type::comma) {
				univalue arg{};
				if(!subparser.parse(argtokens, arg, callbacks)) {
					return false;
				}
				args.emplace_back(move(arg));
				argtokens.clear();
				continue;
			}

			argtokens.emplace_back(it);
		}

		if(!argtokens.empty()) {
			univalue arg{};
			if(!subparser.parse(argtokens, arg, callbacks)) {
				return false;
			}
			args.emplace_back(move(arg));
		}

		return true;
	}

	bool base_parser::read_array(univalue &index, const interfaces::expression_parser_callbacks *callbacks)
	{
		if(finished()) {
			index.clear();
			return false;
		}

		token_vec_t tokens{};
		if(!read({token::extended_type::bracket_left}, {token::extended_type::bracket_right}, tokens)) {
			index.clear();
			return false;
		}

		expression_parser subparser{};
		if(!subparser.parse(tokens, index, callbacks)) {
			return false;
		}

		return true;
	}

	bool base_parser::read_line(univalue &val)
	{
		ucstring str{};
		bool did{read_line(str)};
		val = str;
		return did;
	}

	bool base_parser::read_line(token_vec_t &subtokens)
	{
		if(finished()) {
			subtokens.clear();
			return false;
		}

		//size_t currline{token_().line};

		read([&]() -> bool {
			const token &tok{token_()};

			bool newline{
				tok.type_ == token::type::newline
				//|| tok.line != currline
			};

			if(newline) {
				return false;
			}

			return true;
		}, subtokens);

		return true;
	}

	bool base_parser::read_line(ucstring &str)
	{
		token_vec_t subtokens{};
		if(!read_line(subtokens)) {
			str.clear();
			return false;
		}

		to_string(subtokens, str);

		return true;
	}

	/*
	bool base_parser::read_identifier(univalue &id)
	{
		const token &tok{token_()};
		if(!tok.is_identifier()) {
			id.clear();
			return false;
		}

		token_vec_t subtokens{};
		subtokens.emplace_back(tok);

		advance();

		size_t funcnum{0};

		while(true) {
			if(finished()) {
				break;
			}

			const token &subtok{token_()};
			if(expression_parser::stops_accessor(subtok, funcnum)) {
				break;
			}

			subtokens.emplace_back(subtok);
			advance();
		}

		id = as_string(subtokens);
		return true;
	}
	*/

	bool base_parser::read_identifier(univalue &id, const interfaces::expression_parser_callbacks *callbacks)
	{
		const token &tok{token_()};
		if(!tok.is_identifier()) {
			id.clear();
			return false;
		}

		token_vec_t subtokens{};
		subtokens.emplace_back(tok);

		advance();

		size_t funcnum{0};

		expression_parser subparser{};

		while(true) {
			if(finished()) {
				break;
			}

			const token &subtok{token_()};
			if(expression_parser::stops_accessor(subtok, funcnum)) {
				break;
			}

			subtokens.emplace_back(subtok);
			advance();
		}

		bool parsed{subparser.parse(subtokens, id, callbacks)};
		return parsed;
	}
}