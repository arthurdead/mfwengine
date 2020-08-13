#ifndef MFW_PRIVATE_CORE_BASE_PARSER_HPP
#define MFW_PRIVATE_CORE_BASE_PARSER_HPP

#pragma once

#include <private/mfw/core/scanner.hpp>
#include <public/mfw/core/expression_parser_interface.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/core/univalue.hpp>
#include <public/mfw/stl/format.hpp>

namespace mfw::core
{
	class base_parser
	{
	public:
		base_parser() = default;
		virtual ~base_parser() = default;

		using token_vec_t = scanner::token_vec_t;
		using token = scanner::token;
		using ignore_flags = scanner::ignore_flags;

	protected:
		bool parse(const ucstring_view &str, ignore_flags flags = ignore_flags::default_);
		bool parse(const token_vec_t &tokens);

		virtual void clear();
		virtual void parse_main() = 0;

		virtual void error(const ucstring_view &str) = 0;

		void error(const ucstring_view &str) const { const_cast<base_parser *>(this)->error(str); }

		template <typename ...Args>
		void error(const ucstring_view &fmt, Args &&... args);

		template <typename ...Args>
		void error(const ucstring_view &fmt, Args &&... args) const { const_cast<base_parser *>(this)->error(fmt, forward<Args>(args)...); }

		using skip_flags = ignore_flags;
		void skip(skip_flags flags = skip_flags::default_);
		void skip_newlines() { skip(skip_flags::newline); }
		void skip_whitespace() { skip(skip_flags::whitespace); }
		void skip_all() { skip(skip_flags::all); }
		void advance(ssize_t num = 1);
		void seek(size_t num);
		size_t absolute_offset();
		bool finished() const;
		const token &token_() const;
		const token &peek(ssize_t count = 1);
		void read(size_t count, token_vec_t &tokens);
		bool read(const token &open, const token &close, token_vec_t &tokens);
		void read(function<bool()> util, token_vec_t &tokens);
		bool expect(const token &expected);
		bool expect(const token &expected, token &found);
		bool expect(const token &expected, univalue &found);
		const token &read();

		bool read_function(vector<univalue> &args, const interfaces::expression_parser_callbacks *callbacks);
		bool read_array(univalue &index, const interfaces::expression_parser_callbacks *callbacks);
		bool read_identifier(univalue &id, const interfaces::expression_parser_callbacks *callbacks);
		bool read_function(vector<univalue> &args, const interfaces::accessor_parser_callbacks *callbacks)
		{ return read_function(args, static_cast<const interfaces::expression_parser_callbacks *>(callbacks)); }
		bool read_array(univalue &index, const interfaces::accessor_parser_callbacks *callbacks)
		{ return read_array(index, static_cast<const interfaces::expression_parser_callbacks *>(callbacks)); }
		bool read_identifier(univalue &id, const interfaces::accessor_parser_callbacks *callbacks)
		{ return read_array(id, static_cast<const interfaces::expression_parser_callbacks *>(callbacks)); }
		bool read_line(ucstring &str);
		bool read_line(univalue &str);
		bool read_line(token_vec_t &subtokens);
		//bool read_identifier(univalue &id);

		enum /*class*/ iterators : uchar_t
		{
			start,
			end,
			current,
			count,
		};
		MFW_CLASS_ENUM(iterators)

		bool error_{false};
		using const_iterator = token_vec_t::const_iterator;
		const_iterator iterators_[static_cast<size_t>(iterators::count)]{};
	};

	template <typename ...Args>
	void base_parser::error(const ucstring_view &fmt, Args &&... args)
	{
		ucstring str{};
		format(str, fmt, forward<Args>(args)...);
		error(str);
	}
}

#endif