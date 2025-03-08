#pragma once
#include "../LL1/lexer/pLexer.h"
#include "../LL1/parser/pParser.h"
#include "../../container/pArray.h"
#include "gjEnums.h"
#include "gjVisitor.h"
#include "../LL1/parser/node/pConsumeNode.h"
#include "../LL1/parser/node/pCustomNode.h"
#include "../LL1/parser/node/pExtractNode.h"
#include "../LL1/parser/node/pForwardNode.h"

namespace nsGeoJSON {

	using Lexer = nspLexer::pLexer<gjToken>;
	using Parser = nspParser::pParser<gjToken, gjRule>;
	using Rule = nspParser::pRule<gjToken, gjRule>;
	template<class T>
	using Array = nspArray::pArray<T>;
	using Sequence = nspParser::pSequence<gjToken, gjRule>;
	using ConsumeNode = nspParser::pConsumeNode<gjToken, gjRule>;
	using CustomNode = nspParser::pCustomNode<gjToken, gjRule>;
	using ExtractNode = nspParser::pExtractNode<gjToken, gjRule>;
	using ForwardNode = nspParser::pForwardNode<gjToken, gjRule>;

	Lexer* setup_lexer() {
		Lexer* l = new Lexer();
		l->add_token_definition(gjToken::ERROR, "ERR", "ERROR");
		l->add_token_definition(gjToken::END, "\0", "END");
		l->add_token_definition(gjToken::WS, R"(\s+)", "WS", true);
		l->add_token_definition(gjToken::LBRACE, R"(\{)", "LBRACE");
		l->add_token_definition(gjToken::RBRACE, R"(\})", "RBRACE");
		l->add_token_definition(gjToken::LBRACKET, R"(\[)", "LBRACKET");
		l->add_token_definition(gjToken::RBRACKET, R"(\])", "RBRACKET");
		l->add_token_definition(gjToken::COMMA, R"(\,)", "COMMA");
		l->add_token_definition(gjToken::COLON, R"(:)", "COLON");
		l->add_token_definition(gjToken::STRING, "\"([^\"\\\\]*(\\\\.[^\"\\\\]*)*)\"", "STRING");
		l->add_token_definition(gjToken::TRUE, "true", "TRUE");
		l->add_token_definition(gjToken::FALSE, "false", "FALSE");
		l->add_token_definition(gjToken::NULL, "null", "NULL");
		l->add_token_definition(gjToken::NUMBER, R"(\-?(0|[1-9]\d*)(\.\d+)?([eE][\+\-]?\d+)?)", "NUMBER");
		return l;
	}

	Parser* setup_parser() {
		Lexer* l = new Lexer();
		l->add_token_definition(gjToken::ERROR, "ERR", "ERROR");
		l->add_token_definition(gjToken::END, "\0", "END");
		l->add_token_definition(gjToken::WS, R"(\s+)", "WS", true);
		l->add_token_definition(gjToken::LBRACE, R"(\{)", "LBRACE");
		l->add_token_definition(gjToken::RBRACE, R"(\})", "RBRACE");
		l->add_token_definition(gjToken::LBRACKET, R"(\[)", "LBRACKET");
		l->add_token_definition(gjToken::RBRACKET, R"(\])", "RBRACKET");
		l->add_token_definition(gjToken::COMMA, R"(\,)", "COMMA");
		l->add_token_definition(gjToken::COLON, R"(:)", "COLON");
		l->add_token_definition(gjToken::STRING, "\"([^\"\\\\]*(\\\\.[^\"\\\\]*)*)\"", "STRING");
		l->add_token_definition(gjToken::TRUE, "true", "TRUE");
		l->add_token_definition(gjToken::FALSE, "false", "FALSE");
		l->add_token_definition(gjToken::NULL, "null", "NULL");
		l->add_token_definition(gjToken::NUMBER, R"(\-?(0|[1-9]\d*)(\.\d+)?([eE][\+\-]?\d+)?)", "NUMBER");

		auto* rules = new Array<Rule*>();
		auto* visitor = new gjVisitor();	
		auto* rule = new Rule(gjRule::GeoJSON);
		auto* sequence = &((*(new Sequence())) << new ConsumeNode(gjToken::LBRACE) << new ForwardNode(gjRule::GJProperties) << new ConsumeNode(gjToken::RBRACE));
		(*rule) += sequence;

		Parser* p = new Parser(rules, rule, visitor, l);

		return p;
	}
}