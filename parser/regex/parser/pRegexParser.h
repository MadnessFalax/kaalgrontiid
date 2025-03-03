#pragma once
#include <cstdlib>
#include "../../../container/pString.h"
#include "pRegexLexer.h"
#include "../AST/node/pAlternationNode.h"
#include "../AST/node/pConcatNode.h"
#include "../AST/node/pQualifierNode.h"
#include "../AST/node/pQuantifierNode.h"
#include "pRegexParserException.h"

namespace nspRegexParser {

	using String = nspString::pString;
	template<class T>
	using Array = nspArray::pArray<T>;

	using AltNode = nspRegexAST::pAlternationNode;
	using ConcNode = nspRegexAST::pConcatNode;
	using QualNode = nspRegexAST::pQualifierNode;
	using QuantNode = nspRegexAST::pQuantifierNode;
	using BaseNode = nspRegexAST::pRegexNode;

	enum RuleId {
		BEGIN,
		REGEX,
		REGEX_POST,
		CONC,
		CONC_POST,
		QUAL_QUANT,
		QUANT,
		QUANT_CORE,
		QUANT_CORE_POST,
		QUAL,
		QUAL_RANGE,
		QUAL_RANGE_POST,
		CHAR,
		NUM,
		NUM_TAIL
	};

	class pRegexParser {
		String _pattern = "";
		pRegexLexer _lexer;
		Array<Token> _tokens = Array<Token>();
		size_t _cur = 0;

		Token _peek() { return _cur < _tokens.size() ? _tokens[_cur] : Token(TokenType::END); }
		Token _consume() { return _cur < _tokens.size() ? _tokens[_cur++] : Token(TokenType::END); }
		Token _consume(TokenType type) {
			auto t = _peek();
			if (t.type == type) {
				return _consume();
			}
			throw pRegexParserException("Invalid token type in _consume");
		}

		char _get_char() {
			auto t = _peek();
			if (t.value.length() == 1) {
				return t.value[0];
			}
			throw pRegexParserException("Invalid token type or value in _get_char");
		}
		
		unsigned int _get_num() {
			unsigned int num = 0;
			auto t = _peek();
			if (t.type == TokenType::NUM) {
				num = strtol(t.value.c_str(), NULL, 10);
				return num;
			}
			throw pRegexParserException("Invalid token type or value in _get_num");
		}

		bool _check_range(char c) {
			return (c >= nspRegexAST::qual_min && c <= nspRegexAST::qual_max) || c == '\n' || c == '\t' || c == '\r';
		}

		bool _match(TokenType type) { return _peek().type == type; }

	public:
		pRegexParser() = delete;
		pRegexParser(String pattern) : _pattern(pattern), _lexer(pRegexLexer(pattern)) {};
		pRegexParser(pRegexParser& other) = default;
		pRegexParser(pRegexParser&& other) noexcept = default;

		pRegexParser& operator= (pRegexParser& other) = default;
		pRegexParser& operator= (pRegexParser&& other) noexcept = default;

		// resulting tree is owned by the caller
		BaseNode* parse() {
			_tokens = _lexer.tokenize();
			BaseNode* _root = _parse_regex();

			return _root;
		}
		
	private:
		BaseNode* _parse_regex(RuleId parent_rule = RuleId::BEGIN, TokenType preceding = TokenType::END) {
			BaseNode* node = nullptr;
			BaseNode* conc = _parse_conc(RuleId::REGEX);
			node = _parse_regex_post(conc, RuleId::REGEX, TokenType::END);
			return node;
		}

		BaseNode* _parse_regex_post(BaseNode* prec, RuleId parent_rule = RuleId::BEGIN, TokenType preceding = TokenType::END) {
			BaseNode* node = nullptr;
			if (_match(TokenType::PIPE)) {
				_consume();
				BaseNode* alt = _parse_regex(RuleId::REGEX_POST, TokenType::PIPE);
				node = new AltNode(prec, alt);
			}
			else {
				node = prec;
			}

			return node;
		}

		BaseNode* _parse_conc(RuleId parent_rule = RuleId::BEGIN, TokenType preceding = TokenType::END, Array<BaseNode*> node_arr = Array<BaseNode*>()) {
			BaseNode* node = nullptr;
			if (_match(TokenType::RPAR)) {
				node = new ConcNode(new Array<BaseNode*>(node_arr));
			}
			else if (_match(TokenType::DOT) || _match(TokenType::BSLASH) || _match(TokenType::LPAR) || _match(TokenType::LBRACKET) || _match(TokenType::NUM) || _match(TokenType::CHAR)) {
				node_arr.push_back(_parse_qual_quant(RuleId::CONC, preceding));
				node = _parse_conc_post(RuleId::CONC, preceding, node_arr);
			}
			else if (_match(TokenType::END)) {
				node = new ConcNode(new Array<BaseNode*>(node_arr));
			}
			else {
				throw pRegexParserException("Invalid token in _parse_conc");
			}
			return node;
		}

		BaseNode* _parse_conc_post(RuleId parent_rule = RuleId::BEGIN, TokenType preceding = TokenType::END, Array<BaseNode*> node_arr = Array<BaseNode*>()) {
			BaseNode* node = nullptr;
			if (_match(TokenType::DOT) || _match(TokenType::BSLASH) || _match(TokenType::LPAR) || _match(TokenType::LBRACKET) || _match(TokenType::NUM) || _match(TokenType::CHAR)) {
				node_arr.push_back(_parse_qual_quant(RuleId::CONC_POST, preceding));
				node = _parse_conc(RuleId::CONC_POST, preceding, node_arr);
			}
			else {
				node = new ConcNode(new Array<BaseNode*>(node_arr));
			}
			return node;
		}

		BaseNode* _parse_qual_quant(RuleId parent_rule = RuleId::BEGIN, TokenType preceding = TokenType::END) {
			BaseNode* node = nullptr;
			BaseNode* qual = _parse_qual(RuleId::QUAL_QUANT, TokenType::END);
			node = _parse_quant(qual, RuleId::QUAL_QUANT, TokenType::END);
			return node;
		}

		BaseNode* _parse_quant(BaseNode* prec, RuleId parent_rule = RuleId::BEGIN, TokenType preceding = TokenType::END) {
			BaseNode* node = nullptr;
			if (_match(TokenType::LBRACE)) {
				_consume();
				node = _parse_quant_core(prec, RuleId::QUANT, TokenType::LBRACE);
				_consume(TokenType::RBRACE);
			}
			else if (_match(TokenType::PLUS)) {
				_consume();
				node = new QuantNode(1, 0, prec);
			}
			else if (_match(TokenType::STAR)) {
				_consume();
				node = new QuantNode(0, 0, prec);
			}
			else if (_match(TokenType::QMARK)) {
				_consume();
				node = new QuantNode(0, 1, prec);
			}
			else {
				node = prec;
			}
			return node;
		}

		BaseNode* _parse_quant_core(BaseNode* prec, RuleId parent_rule = RuleId::BEGIN, TokenType preceding = TokenType::END) {
			BaseNode* node = nullptr;
			if (_match(TokenType::COMMA)) {
				_consume();
				unsigned int r_num = _get_num();
				node = new QuantNode(0, r_num, prec);
			}
			else if (_match(TokenType::NUM)) {
				unsigned int l_num = _get_num();
				_consume();
				_consume(TokenType::COMMA);
				Token t = _peek();
				if (t.type == TokenType::NUM) {
					unsigned int r_num = _get_num();
					node = new QuantNode(l_num, r_num, prec);
					_consume();
				}
				else if (t.type == TokenType::RBRACE) {
					node = new QuantNode(l_num, 0, prec);
				}
				else {
					throw pRegexParserException("Invalid token in _parse_quant_core");
				}
			}
			return node;
		}

		BaseNode* _parse_qual(RuleId parent_rule = RuleId::BEGIN, TokenType preceding = TokenType::END) {
			BaseNode* node = nullptr;
			if (_match(TokenType::DOT)) {
				_consume();
				node = new QualNode(nspRegexAST::qual_min, nspRegexAST::qual_max);
			}
			else if (_match(TokenType::LBRACKET)) {
				_consume();
				node = _parse_qual_range(RuleId::QUAL, TokenType::LBRACKET);
				_consume(TokenType::RBRACKET);
			}
			else if (_match(TokenType::LPAR)) {
				_consume();
				node = _parse_regex(RuleId::QUAL, TokenType::LPAR);
				_consume(TokenType::RPAR);
			}
			else if (_match(TokenType::BSLASH)) {
				_consume();
				unsigned char value = _get_char();
				if (value == 'd') {
					_consume();
					node = new QualNode('0', '9');
				}
				else if (value == 's') {
					_consume();
					Array<unsigned char> arr = Array<unsigned char>();
					arr.push_back(' ');
					arr.push_back('\t');
					arr.push_back('\n');
					arr.push_back('\r');
					node = new QualNode(new Array<unsigned char>(arr));					// possibility to add more whitespace characters in future
				}
				else if (_check_range(value)) {
					_consume();
					node = new QualNode(value);
				}
				else {
					throw pRegexParserException("Invalid escape sequence in _parse_qual: only characters \\n, \\r, \\t and characters in range char(32) - char(136) are allowed");
				}
			}
			else if (_match(TokenType::CHAR)) {
				node = new QualNode(_get_char());
				_consume();
				return node;
			}
			else {
				throw pRegexParserException("Invalid token in _parse_qual");
			}

			return node;
		}

		BaseNode* _parse_qual_range(RuleId parent_rule = RuleId::BEGIN, TokenType preceding = TokenType::END, Array<unsigned char> char_arr = Array<unsigned char>()) {
			auto token = _peek();
			BaseNode* node = nullptr;
			auto character = _get_char();
			if (token.type == TokenType::RBRACKET) {
				node = new QualNode(new Array<unsigned char>(char_arr));
			}
			else if (token.type == TokenType::BSLASH) {
				_consume();
				token = _peek();
				unsigned char value = _get_char();
				if (value == 'd') {
					_consume();
					if (_peek().type == TokenType::DASH) {
						throw pRegexParserException("Invalid token type in _parse_qual_range");
					}
					else {
						for (unsigned char i = '0'; i <= '9'; i++) {
							char_arr.push_back(i);
						}
						node = _parse_qual_range_post('\0', RuleId::QUAL_RANGE, TokenType::CHAR, char_arr);
					}
				}
				else if (value == 's') {
					_consume();
					if (_peek().type == TokenType::DASH) {
						throw pRegexParserException("Invalid token type in _parse_qual_range");
					}
					else {
						char_arr.push_back(' ');
						char_arr.push_back('\t');
						char_arr.push_back('\n');
						char_arr.push_back('\r');
						node = _parse_qual_range_post('\0', RuleId::QUAL_RANGE, TokenType::CHAR, char_arr);
					}
				}
				else if (_check_range(value)) {
					_consume();
					node = _parse_qual_range_post(value, RuleId::QUAL_RANGE, TokenType::CHAR, char_arr);
				}
				else {
					throw pRegexParserException("Invalid escape sequence in _parse_qual_range: \\r, \\t, \\n are not supported throughout implementation");
				}
			}
			else if (token.type == TokenType::CHAR) {
				_consume();
				node = _parse_qual_range_post(character, RuleId::QUAL_RANGE, TokenType::CHAR, char_arr);
			}
			else {
				throw pRegexParserException("Invalid token type in _parse_qual_range");
			}

			return node;
		}

		BaseNode* _parse_qual_range_post(unsigned char character, RuleId parent_rule = RuleId::BEGIN, TokenType preceding = TokenType::END, Array<unsigned char> char_arr = Array<unsigned char>()) {
			auto token = _peek();
			BaseNode* node = nullptr;
			if (token.type == TokenType::DASH) {
				_consume();
				token = _peek();
				if (token.type == TokenType::CHAR) {
					if (character > _get_char()) {
						throw pRegexParserException("Invalid range in _parse_qual_range_post");
					}
					for (unsigned char i = character; i <= _get_char(); i++) {
						char_arr.push_back(i);
					}
					_consume();
					node = _parse_qual_range(RuleId::QUAL_RANGE_POST, TokenType::CHAR, char_arr);
				}
				else if (token.type == TokenType::BSLASH) {
					_consume();
					token = _peek();
					unsigned char value = _get_char();
					if (_check_range(value) && value != 'd' && value != 's') {
						if (character > value) {
							throw pRegexParserException("Invalid range in _parse_qual_range_post");
						}
						else {
							for (unsigned char i = character; i <= value; i++) {
								char_arr.push_back(i);
							}
							_consume();
							node = _parse_qual_range_post('\0', RuleId::QUAL_RANGE_POST, TokenType::CHAR, char_arr);
						}
					}
				}
				else {
					throw pRegexParserException("Invalid token type in _parse_qual_range_post");
				}
			}
			else if (token.type == TokenType::CHAR || token.type == TokenType::BSLASH) {
				char_arr.push_back(character);
				node = _parse_qual_range(RuleId::QUAL_RANGE_POST, preceding, char_arr);
			}
			else {
				if (character != '\0') {
					char_arr.push_back(character);
				}
				node = new QualNode(new Array<unsigned char>(char_arr));
			}

			return node;
		}

	};
}