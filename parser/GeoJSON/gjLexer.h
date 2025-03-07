#pragma once
#include "../LL1/lexer/pLexer.h"
#include "gjTokenEnum.h"

namespace nsGeoJSON {

	using Lexer = nspLexer::pLexer<gjToken>;

	Lexer setup_lexer() {
		Lexer l = Lexer();
		l.add_token_definition(gjToken::ERROR, "ERR", "ERROR");
		l.add_token_definition(gjToken::END, "\0", "END");
		l.add_token_definition(gjToken::WS, R"(\s+)", "WS", true);
		l.add_token_definition(gjToken::LBRACE, R"(\{)", "LBRACE");
		l.add_token_definition(gjToken::RBRACE, R"(\})", "RBRACE");
		l.add_token_definition(gjToken::LBRACKET, R"(\[)", "LBRACKET");
		l.add_token_definition(gjToken::RBRACKET, R"(\])", "RBRACKET");
		l.add_token_definition(gjToken::COMMA, R"(\,)", "COMMA");
		l.add_token_definition(gjToken::COLON, R"(:)", "COLON");
		l.add_token_definition(gjToken::STRING, R"(".+")", "STRING");
		l.add_token_definition(gjToken::TRUE, "true", "TRUE");
		l.add_token_definition(gjToken::FALSE, "false", "FALSE");
		l.add_token_definition(gjToken::NULL, "null", "NULL");
		l.add_token_definition(gjToken::NUMBER, R"(\-?(0|[1-9]\d*)(\.\d+)?([eE][\+\-]?\d+)?)", "NUMBER");
		return l;
	}
}