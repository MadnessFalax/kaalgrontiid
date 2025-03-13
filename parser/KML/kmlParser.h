#pragma once
#include "../LL1/lexer/pLexer.h"
#include "../LL1/parser/pParser.h"
#include "../../container/pArray.h"
#include "kmlEnums.h"
#include "kmlVisitor.h"
#include "../LL1/parser/node/pConsumeNode.h"
#include "../LL1/parser/node/pCustomNode.h"
#include "../LL1/parser/node/pExtractNode.h"
#include "../LL1/parser/node/pForwardNode.h"
#include "../LL1/parser/node/pEntryNode.h"

namespace nsKML {

	using Lexer = nspLexer::pLexer<kmlToken>;
	using Parser = nspParser::pParser<kmlToken, kmlRule, kmlHandler, kmlVisitor>;
	using Rule = nspParser::pRule<kmlToken, kmlRule>;
	template<class T>
	using Array = nspArray::pArray<T>;
	using Sequence = nspParser::pSequence<kmlToken, kmlRule>;
	using ConsumeNode = nspParser::pConsumeNode<kmlToken, kmlRule>;
	using CustomNode = nspParser::pCustomNode<kmlToken, kmlRule, kmlHandler>;
	using ExtractNode = nspParser::pExtractNode<kmlToken, kmlRule>;
	using ForwardNode = nspParser::pForwardNode<kmlToken, kmlRule>;
	using EntryNode = nspParser::pEntryNode<kmlToken, kmlRule>;

	Lexer* setup_lexer() {
		Lexer* lexer = new Lexer();
		lexer->add_token_definition(kmlToken::ERROR, "ERR", "ERROR");
		lexer->add_token_definition(kmlToken::LANGLEBRACKET, "<", "LANGLEBRACKET");
		lexer->add_token_definition(kmlToken::RANGLEBRACKET, ">", "RANGLEBRACKET");
		lexer->add_token_definition(kmlToken::DASH, R"(\-)", "DASH");
		lexer->add_token_definition(kmlToken::QMARK, R"(\?)", "QMARK");
		lexer->add_token_definition(kmlToken::EXCLAMATION, "!", "EXCLAMATION");
		lexer->add_token_definition(kmlToken::EQUALS, "=", "EQUALS");
		lexer->add_token_definition(kmlToken::WS, R"(\s+)", "WS");
		lexer->add_token_definition(kmlToken::DOUBLEQUOTE, R"(")", "DOUBLEQUOTE");
		lexer->add_token_definition(kmlToken::SLASH, "/", "SLASH");
		lexer->add_token_definition(kmlToken::NUMBER, R"(\-?\d+(\.\d+)?,\-?\d+(\.\d+)?(,\-?\d+(\.\d+)?)?( \s*\-?\d+(\.\d+)?,\-?\d+(\.\d+)?(,\-?\d+(\.\d+)?)?)*)", "NUMBER");
		lexer->add_token_definition(kmlToken::STRING, R"([^<>"&']+)", "STRING");
		return lexer;
	}

	/*Parser* setup_parser() {

	}*/
}