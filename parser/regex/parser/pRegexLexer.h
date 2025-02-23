#pragma once
#include "../../../container/pString.h"
#include "../../../container/pArray.h"

namespace nspRegexParser {
	using String = nspString::pString;
	template<class T>
	using Array = nspArray::pArray<T>;

	enum class TokenType {
		PIPE, PLUS, STAR, QMARK, LBRACE, RBRACE, LPAR, RPAR, LBRACKET, RBRACKET, COMMA, DOT, BSLASH, DASH, NUM, CHAR, END
	};

	struct Token {
		TokenType type;
		String value;

		Token(TokenType type, String value = "") {
			this->type = type;
			this->value = value;
		}
	};

	class pRegexLexer {
		String _pattern = "";
		size_t _pos = 0;
		Token _last_token = Token(TokenType::END);

		Token _next_token() {
			while (_pos < _pattern.length()) {
				char c = _consume();
				switch (c) {
				case '|':
					return Token(TokenType::PIPE, "|");
				case '+':
					return Token(TokenType::PLUS, "+");
				case '*':
					return Token(TokenType::STAR, "*");
				case '?':
					return Token(TokenType::QMARK, "?");
				case '(':
					return Token(TokenType::LPAR, "(");
				case ')':
					return Token(TokenType::RPAR, ")");
				case '{':
					return Token(TokenType::LBRACE, "{");
				case '}':
					return Token(TokenType::RBRACE, "}");
				case '[':
					return Token(TokenType::LBRACKET, "[");
				case ']':
					return Token(TokenType::RBRACKET, "]");
				case ',':
					return Token(TokenType::COMMA, ",");
				case '.':
					return Token(TokenType::DOT, ".");
				case '\\':
					return Token(TokenType::BSLASH, "\\");
				case '-':
					return Token(TokenType::DASH, "-");
				default:
					if (c >= '0' && c <= '9') {
						if (_last_token.type == TokenType::LBRACE || _last_token.type == TokenType::NUM || _last_token.type == TokenType::COMMA) {
							String num = "";
							num += c;
							while (_peek() >= '0' && _peek() <= '9') {
								num += _consume();
							}
							return Token(TokenType::NUM, num);
						}
						else {
							String chr = "";
							chr += c;
							return Token(TokenType::CHAR, chr);
						}
					}
					else {
						String chr = "";
						chr += c;
						return Token(TokenType::CHAR, chr);
					}
				}
			}
			return Token(TokenType::END);
		}

		char _peek() {
			return _pos < _pattern.length() ? _pattern[_pos] : '\0';
		}

		char _consume() {
			return _pos < _pattern.length() ? _pattern[_pos++] : '\0';
		}

	public:
		pRegexLexer() = delete;
		pRegexLexer(String pattern) : _pattern(pattern) {};
		pRegexLexer(pRegexLexer& other) = default;
		pRegexLexer(pRegexLexer&& other) = default;
		~pRegexLexer() = default;
		Array<Token> tokenize() {
			auto tokens = Array<Token>();
			Token cur = Token(TokenType::END, "");
			while ((cur = _next_token()).type != TokenType::END) {
				tokens.push_back(cur);
				_last_token = cur;
			}
			tokens.push_back(cur);
			return tokens;
		}

		pRegexLexer& operator=(pRegexLexer& other) = default;
		pRegexLexer& operator=(pRegexLexer&& other) = default;
	};
};