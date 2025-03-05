#pragma once
#include "../LL1/lexer/pLexer.h"
#include "gjTokenEnum.h"

namespace nsGeoJSON {

	using Lexer = nspLexer::pLexer<gjToken>;

	Lexer setup_lexer() {
		Lexer l = Lexer();
		l.add_token_definition(gjToken::ERROR, "ERR");
		l.add_token_definition(gjToken::WS, R"(\s+)");
		l.add_token_definition(gjToken::LBRACE, R"(\{)");
		l.add_token_definition(gjToken::RBRACE, R"(\})");
		l.add_token_definition(gjToken::LBRACKET, R"(\[)");
		l.add_token_definition(gjToken::RBRACKET, R"(\])");
		l.add_token_definition(gjToken::COMMA, R"(\,)");
		l.add_token_definition(gjToken::COLON, R"(:)");
		l.add_token_definition(gjToken::STRING, R"(".+")");
		l.add_token_definition(gjToken::TRUE, "true");
		l.add_token_definition(gjToken::FALSE, "false");
		l.add_token_definition(gjToken::NULL, "null");
		l.add_token_definition(gjToken::NUMBER, R"(\-?(0|[1-9]\d*)(\.\d+)?([eE][\+\-]?\d+)?)");
		return l;
	}
}