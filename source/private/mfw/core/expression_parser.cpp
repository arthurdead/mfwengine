#include <private/mfw/core/expression_parser.hpp>
#include <public/mfw/core/expression_parser_interface.hpp>
#include <public/mfw/core/globals.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <public/mfw/core/serializable.hpp>
#include <public/mfw/core/univalue.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/pch_literals.hpp>

namespace mfw::core
{
	MFW_DECLARE_LOG_CONTEXT(log_expression_parser, u8"core/expression_parser"_p)

	MFW_DECLARE_GLOBAL_ALLOCATOR(expressionparser, expression_parser)

	expression_parser &expression_parser::instance() {
		return __expressionparser_global_allocator.instance();
	}

	void expression_parser::clear()
	{
		super::clear();

		currvalue.clear();
		callbacks_ = nullptr;
	}

	void expression_parser::parse_main()
	{
		while(true)
		{
			if(finished()) {
				break;
			}

			token value1{};
			if(!read_var(value1)) {
				value1 = currvalue;
			}

			if(finished()) {
				currvalue = value1;
				return;
			}

			token op{};
			token value2{};
			if(!read_op(op, value2)) {
				return;
			}

			token result{};
			if(!do_operator(value1, value2, op, result)) {
				return;
			}

			if(needs_assign(op)) {
				currvalue = result;
			} else {
				currvalue.value += result.value;
			}
		}
	}

	bool expression_parser::needs_assign(const token &tok)
	{
		return needs_subparser(tok);
	}

	namespace __expression_parser_internal
	{
		static bool to_univalue(const type_holder &var, univalue &value)
		{
			value.set(var);
			return true;
		}
	}

	bool expression_parser::stops_accessor(const token &tok, size_t &funcnum)
	{
		token::extended_type subextype{tok.get_extended_type()};

		if(tok.type_ == token::type::comma) {
			if(funcnum > 0) {
				return false;
			}
		} else if(tok.type_ == token::type::dot) {
			return false;
		} else if(subextype == token::extended_type::parenthesis_left || subextype == token::extended_type::parenthesis_right) {
			if(subextype == token::extended_type::parenthesis_left) {
				funcnum++;
			} else if(subextype == token::extended_type::parenthesis_right) {
				funcnum--;
			}
			return false;
		} else if(subextype == token::extended_type::bracket_left || subextype == token::extended_type::bracket_right) {
			return false;
		} else if(tok.type_ == token::type::identifier) {
			return false;
		} else if(tok.type_ == token::type::slash || tok.type_ == token::type::divide) {
			return false;
		}

		return true;
	}

	bool expression_parser::is_unary(const token &tok)
	{
		token::extended_type preextype{tok.get_extended_type()};

		if(tok.type_ == token::type::add || tok.type_ == token::type::subtract) {
			const token &prev{peek(-1)};
			return (prev.type_ != token::type::identifier);
		} else if(tok.type_ == token::type::logic_not || tok.type_ == token::type::bit_not) {
			return true;
		} else if(preextype == token::extended_type::plus_plus || preextype == token::extended_type::minus_minus) {
			return true;
		}

		return false;
	}

	bool expression_parser::read_var(token &value)
	{
		if(finished()) {
			return false;
		}

		token pretok{token_()};
		if(!is_unary(pretok)) {
			pretok.clear();
		} else {
			advance();
		}

		token_vec_t subtokens{};
		if(read({token::extended_type::parenthesis_left}, {token::extended_type::parenthesis_right}, subtokens)) {
			expression_parser subparser{};
			if(!subparser.parse(subtokens, value, callbacks_)) {
				return false;
			}
		} else {
			const token &tok{read()};
			if(tok.type_ != token::type::identifier) {
				advance(pretok.valid() ? -2 : -1);
				return false;
			}

			if(tok.is_identifier()) {
				subtokens.clear();
				subtokens.emplace_back(tok);

				size_t funcnum{0};

				while(true) {
					if(finished()) {
						break;
					}

					const token &subtok{token_()};
					if(stops_accessor(subtok, funcnum)) {
						break;
					}

					subtokens.emplace_back(subtok);
					advance();
				}

				type_holder result{};
				if(!read_accessor(subtokens, result)) {
					return false;
				}

				value = tok;
				if(!__expression_parser_internal::to_univalue(result, value.value)) {
					error(u8"cannot convert result: {}"_sv, tok.as_string());
					return false;
				}
			} else {
				value = tok;
			}
		}

		if(pretok.valid()) {
			token result{};
			if(!do_operator(value, {}, pretok, result)) {
				return false;
			}
			value = result;
		}

		if(!value.valid()) {
			MFW_DEBUGBREAK();
		}

		return true;
	}

	bool expression_parser::needs_subparser(const token &tok)
	{
		token::associative assoc{tok.get_associative(false)};

		return (assoc == token::associative::left_to_right);
	}

	bool expression_parser::stops_subparser(const token &start, const token &current)
	{
		size_t prec1{start.get_precedence(false)};
		size_t prec2{current.get_precedence(false)};

		return (prec1 < prec2);
	}

	bool expression_parser::read_op(token &op, token &value)
	{
		if(finished()) {
			return false;
		}

		const token &tok{read()};
		if(tok.type_ == token::type::identifier) {
			error(u8"expected operator but found: {}"_sv, tok.as_string());
			return false;
		}

		op = tok;

		if(needs_subparser(tok)) {
			token_vec_t subtokens{};

			token::extended_type extype{token_().get_extended_type()};

			size_t closenum{0};

			bool enclosed{false};
			if(extype == token::extended_type::parenthesis_left) {
				enclosed = true;
				closenum++;
			}

			while(true) {
				if(finished()) {
					break;
				}

				const token &sub_tok{token_()};
				if(!enclosed && stops_subparser(tok, sub_tok)) {
					break;
				} else if(enclosed) {
					extype = sub_tok.get_extended_type();
					if(extype == token::extended_type::parenthesis_left) {
						closenum++;
					} else if(extype == token::extended_type::parenthesis_right) {
						closenum--;
						if(closenum == 0) {
							break;
						}
					}
				}

				subtokens.emplace_back(sub_tok);
				advance();
			}

			expression_parser subparser{};
			if(!subparser.parse(subtokens, value, callbacks_)) {
				return false;
			}
		} else {
			if(!read_var(value)) {
				return false;
			}
		}

		return true;
	}

	bool expression_parser::do_operator(const token &val1, const token &val2, const token &tok, token &result)
	{
		result.clear();
		result.type_ = token::type::identifier;

		bool assign{false};

		token::extended_type extype{tok.get_extended_type()};
		if(extype == token::extended_type::unknown) {
			if(tok.type_ == token::type::add) {
				if(val2.valid()) {
					result.value = (val1.value + val2.value);
				} else {
					result.value = (+val1.value);
				}
			} else if(tok.type_ == token::type::subtract) {
				if(val2.valid()) {
					result.value = (val1.value - val2.value);
				} else {
					result.value = (-val1.value);
				}
			} else if(tok.type_ == token::type::multiply) {
				result.value = (val1.value * val2.value);
			} else if(tok.type_ == token::type::divide) {
				result.value = (val1.value / val2.value);
			} else if(tok.type_ == token::type::bit_xor) {
				result.value = (val1.value ^ val2.value);
			} else if(tok.type_ == token::type::bit_and) {
				result.value = (val1.value | val2.value);
			} else if(tok.type_ == token::type::bit_or) {
				result.value = (val1.value & val2.value);
			} else if(tok.type_ == token::type::logic_not) {
				result.value = (!val1.value);
			} else if(tok.type_ == token::type::modulo) {
				result.value = (val1.value % val2.value);
			} else if(tok.type_ == token::type::assign) {
				result.value = (val2.value);
				assign = true;
			} else if(tok.type_ == token::type::bit_not) {
				result.value = (~val1.value);
			}
		} else if(extype == token::extended_type::plus_plus) {
			result.value = (val1.value + 1);
			assign = true;
		} else if(extype == token::extended_type::minus_minus) {
			result.value = (val1.value - 1);
			assign = true;
		} else if(extype == token::extended_type::bit_shift_left) {
			result.value = (val1.value << val2.value);
		} else if(extype == token::extended_type::bit_shift_right) {
			result.value = (val1.value >> val2.value);
		} else if(extype == token::extended_type::logic_and) {
			result.value = (val1.value && val2.value);
		} else if(extype == token::extended_type::logic_or) {
			result.value = (val1.value || val2.value);
		} else if(extype == token::extended_type::plus_equal) {
			result.value = (val1.value + val2.value);
			assign = true;
		} else if(extype == token::extended_type::minus_equal) {
			result.value = (val1.value - val2.value);
			assign = true;
		} else if(extype == token::extended_type::multiply_equal) {
			result.value = (val1.value * val2.value);
			assign = true;
		} else if(extype == token::extended_type::divide_equal) {
			result.value = (val1.value / val2.value);
			assign = true;
		} else if(extype == token::extended_type::not_equal) {
			result.value = (val1.value != val2.value);
		} else if(extype == token::extended_type::equal) {
			result.value = (val1.value == val2.value);
		} else if(extype == token::extended_type::lesser_equal) {
			result.value = (val1.value <= val2.value);
		} else if(extype == token::extended_type::greater_equal) {
			result.value = (val1.value >= val2.value);
		} else if(extype == token::extended_type::lesser) {
			result.value = (val1.value < val2.value);
		} else if(extype == token::extended_type::greater) {
			result.value = (val1.value > val2.value);
		} else if(extype == token::extended_type::modulo_equal) {
			result.value = (val1.value % val2.value);
			assign = true;
		} else if(extype == token::extended_type::bit_shift_left_equal) {
			result.value = (val1.value << val2.value);
			assign = true;
		} else if(extype == token::extended_type::bit_shift_right_equal) {
			result.value = (val1.value >> val2.value);
			assign = true;
		} else if(extype == token::extended_type::bit_xor_equal) {
			result.value = (val1.value ^ val2.value);
			assign = true;
		} else if(extype == token::extended_type::bit_and_equal) {
			result.value = (val1.value | val2.value);
			assign = true;
		} else if(extype == token::extended_type::bit_or_equal) {
			result.value = (val1.value & val2.value);
			assign = true;
		}

		if(assign) {
			if(!val1.is_identifier()) {
				error(u8"cant assign number or string"_sv);
				return false;
			} else {
				//error(u8"assigning identifiers not implemented"_sv);
				//return false;
			}
		}

		//ucstring opstr{};
		//tok.to_string(opstr);

		if(result.value.empty()) {
			error(u8"unknown operator: {}"_sv, tok.as_string());
			return false;
		}

		//log_expression_parser().info(u8"{} {} {}"_sv, val1.value, opstr, val2.value);

		return true;
	}

	bool expression_parser::read_accessor(const token_vec_t &subtokens, type_holder &result)
	{
		accessor_parser subparser{};
		if(!subparser.parse(subtokens, result, callbacks_)) {
			error(u8"failed to parse accessor: {}"_sv, as_string<ucstring>(subtokens));
			return false;
		}
		return true;
	}

	void expression_parser::error(const ucstring_view &str)
	{
		super::error(str);

		log_expression_parser().error(str);
	}

	bool expression_parser::parse(const token_vec_t &tokens, univalue &result, const interfaces::expression_parser_callbacks *callback)
	{
		if(tokens.empty()) {
			return false;
		}

		token tok{};
		bool ret{parse(tokens, tok, callback) && currvalue.valid()};
		if(ret) {
			result = tok.value;
		} else {
			result.clear();
		}
		return ret;
	}

	bool expression_parser::parse(const token_vec_t &tokens, token &result, const interfaces::expression_parser_callbacks *callback)
	{
		if(tokens.empty()) {
			return false;
		}

		clear();

		callbacks_ = callback;

		bool ret{super::parse(tokens) && currvalue.valid()};
		if(ret) {
			result = currvalue;
		} else {
			result.clear();
		}

		return ret;
	}

	bool expression_parser::parse(const ucstring_view &str, univalue &result, const interfaces::expression_parser_callbacks *callback)
	{
		if(str.empty()) {
			return false;
		}

		clear();

		callbacks_ = callback;

		bool ret{super::parse(str) && currvalue.valid()};
		if(ret) {
			result = currvalue.value;
		} else {
			result.clear();
		}

		return ret;
	}

	void expression_parser::invert(token_vec_t &subtokens)
	{
		subtokens.insert(subtokens.cbegin(), {token::type::logic_not});
		subtokens.insert(subtokens.cbegin()+1, {token::extended_type::parenthesis_left});
		subtokens.emplace_back(token::extended_type::parenthesis_right);
	}

	void expression_parser::append(token_vec_t &tokens1, const token_vec_t &tokens2)
	{
		tokens1.insert(tokens1.cbegin(), {token::extended_type::parenthesis_left});
		tokens1.emplace_back(token::extended_type::parenthesis_right);

		tokens1.emplace_back(token::extended_type::logic_and);

		tokens1.emplace_back(token::extended_type::parenthesis_left);
		for(const token &tok : tokens2) {
			tokens1.emplace_back(tok);
		}
		tokens1.emplace_back(token::extended_type::parenthesis_right);
	}

	extern "C"
	{
		MFW_CORE_API bool MFW_CORE_CALL parse_expression(const ucstring_view &str, univalue &result, const interfaces::expression_parser_callbacks *callbacks)
		{
			expression_parser subparser{};
			return subparser.parse(str, result, callbacks);
		}

		MFW_CORE_API void MFW_CORE_CALL invert_expression(const ucstring &expr1, ucstring &result)
		{
			if(expr1.empty()) {
				return;
			}

			result = expr1;
			result.insert(0, u8"!("_sv);
			result += u8')';
		}

		MFW_CORE_API void MFW_CORE_CALL append_expression(const ucstring &expr1, const ucstring &expr2, ucstring &result)
		{
			if(!expr1.empty() && !expr2.empty()) {
				result = expr1;
				result.insert(0, 1, u8'(');
				result += u8')';
				result += u8" && ("_sv;
				result += expr2;
				result += u8')';
			} else if(!expr1.empty()) {
				result = expr1;
			} else if(!expr2.empty()) {
				result = expr2;
			}
		}
	}
}