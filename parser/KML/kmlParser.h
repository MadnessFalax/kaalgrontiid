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
	using Stack = nspParser::pParserStack<kmlToken, kmlRule, kmlHandler>;
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
		lexer->add_token_definition(kmlToken::RBRACKET, "]", "RBRACKET");
		lexer->add_token_definition(kmlToken::LBRACKET, "[", "LBRACKET");
		lexer->add_token_definition(kmlToken::DASH, R"(\-)", "DASH");
		lexer->add_token_definition(kmlToken::QMARK, R"(\?)", "QMARK");
		lexer->add_token_definition(kmlToken::EXCLAMATION, "!", "EXCLAMATION");
		lexer->add_token_definition(kmlToken::QUOTE, "'", "QUOTE");
		lexer->add_token_definition(kmlToken::DOUBLEQUOTE, "\"", "DOUBLEQUOTE");
		lexer->add_token_definition(kmlToken::EQUALS, "=", "EQUALS");
		lexer->add_token_definition(kmlToken::WS, R"(\s+)", "WS");
		lexer->add_token_definition(kmlToken::SLASH, "/", "SLASH");
		lexer->add_token_definition(kmlToken::COMMA, R"(\,)", "COMMA");
		lexer->add_token_definition(kmlToken::NUMBER, R"(\-?\d+(\.\d+)?,\-?\d+(\.\d+)?(,\-?\d+(\.\d+)?)?( \s*\-?\d+(\.\d+)?,\-?\d+(\.\d+)?(,\-?\d+(\.\d+)?)?)*)", "NUMBER");
		lexer->add_token_definition(kmlToken::STRING, R"([^<>" =]+)", "STRING");
		return lexer;
	}

	Parser* setup_parser() {
		Lexer* lexer = new Lexer();
		lexer->add_token_definition(kmlToken::ERROR, "ERR", "ERROR");
		lexer->add_token_definition(kmlToken::LANGLEBRACKET, "<", "LANGLEBRACKET");
		lexer->add_token_definition(kmlToken::RANGLEBRACKET, ">", "RANGLEBRACKET");
		lexer->add_token_definition(kmlToken::RBRACKET, R"(\])", "RBRACKET");
		lexer->add_token_definition(kmlToken::LBRACKET, R"(\[)", "LBRACKET");
		lexer->add_token_definition(kmlToken::DASH, R"(\-)", "DASH");
		lexer->add_token_definition(kmlToken::QMARK, R"(\?)", "QMARK");
		lexer->add_token_definition(kmlToken::EXCLAMATION, "!", "EXCLAMATION");
		lexer->add_token_definition(kmlToken::QUOTE, "'", "QUOTE");
		lexer->add_token_definition(kmlToken::DOUBLEQUOTE, "\"", "DOUBLEQUOTE");
		lexer->add_token_definition(kmlToken::EQUALS, "=", "EQUALS");
		lexer->add_token_definition(kmlToken::WS, R"(\s+)", "WS");
		lexer->add_token_definition(kmlToken::SLASH, "/", "SLASH");
		lexer->add_token_definition(kmlToken::COMMA, R"(\,)", "COMMA");
		lexer->add_token_definition(kmlToken::NUMBER, R"(\-?\d+(\.\d+)?,\-?\d+(\.\d+)?(,\-?\d+(\.\d+)?)?( \s*\-?\d+(\.\d+)?,\-?\d+(\.\d+)?(,\-?\d+(\.\d+)?)?)*)", "NUMBER");
		lexer->add_token_definition(kmlToken::STRING, R"([^\[\]<>"' =]+)", "STRING");

		auto* rules = new Array<Rule*>();

		auto* first_rule = new Rule(kmlRule::EntryRule, "EntryRule");
		(*first_rule) += &((*(new Sequence())) << new EntryNode(kmlRule::KML, kmlRule::EntryRule));
		rules->push_back(first_rule);

		auto* rule = new Rule(kmlRule::KML, "KML");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::LANGLEBRACKET)
			<< new ForwardNode(kmlRule::AnyTagContent));
		rules->push_back(rule);

		rule = new Rule(kmlRule::AnyTagContent, "AnyTagContent");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::QMARK)
			<< new ConsumeNode(kmlToken::STRING)
			<< new ForwardNode(kmlRule::OptionalAttr)
			<< new ConsumeNode(kmlToken::QMARK)
			<< new ConsumeNode(kmlToken::RANGLEBRACKET)
			<< new ForwardNode(kmlRule::ConsumeWS)
			<< new ForwardNode(kmlRule::PostInstruction));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::EXCLAMATION)
			<< new ForwardNode(kmlRule::CommentOrCDATA));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(kmlRule::NormalTag));
		rules->push_back(rule);

		rule = new Rule(kmlRule::CommentOrCDATA, "CommentOrCDATA");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::DASH)
			<< new ConsumeNode(kmlToken::DASH)
			<< new ForwardNode(kmlRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::LBRACKET)
			<< new ConsumeNode(kmlToken::STRING, "CDATA")
			<< new ConsumeNode(kmlToken::LBRACKET)
			<< new ForwardNode(kmlRule::CDATAContent)
			<< new ConsumeNode(kmlToken::RANGLEBRACKET)
			<< new ForwardNode(kmlRule::ConsumeWS));
		rules->push_back(rule);

		rule = new Rule(kmlRule::CDATAContent, "CDATAContent");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::EXCLAMATION)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::QMARK)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::LANGLEBRACKET)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::RANGLEBRACKET)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::DASH)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::LBRACKET)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::RBRACKET)
			<< new ForwardNode(kmlRule::CDATAFirstRBracket));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::EQUALS)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::WS)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::SLASH)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::COMMA)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::QUOTE)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::DOUBLEQUOTE)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::NUMBER)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::STRING)
			<< new ForwardNode(kmlRule::CDATAContent));
		rules->push_back(rule);

		rule = new Rule(kmlRule::CDATAFirstRBracket, "CDATAFirstRBracket");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::EXCLAMATION)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::QMARK)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::LANGLEBRACKET)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::RANGLEBRACKET)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::DASH)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::LBRACKET)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::RBRACKET)
			<< new ForwardNode(kmlRule::CDATASecondRBracket));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::EQUALS)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::WS)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::SLASH)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::COMMA)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::QUOTE)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::DOUBLEQUOTE)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::NUMBER)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::STRING)
			<< new ForwardNode(kmlRule::CDATAContent));
		rules->push_back(rule);

		rule = new Rule(kmlRule::CDATASecondRBracket, "CDATASecondRBracket");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::EXCLAMATION)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::QMARK)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::LANGLEBRACKET)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::DASH)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::LBRACKET)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::RBRACKET)
			<< new ForwardNode(kmlRule::CDATASecondRBracket));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::EQUALS)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::WS)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::SLASH)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::COMMA)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::QUOTE)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::DOUBLEQUOTE)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::NUMBER)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::STRING)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(kmlRule::PostInstruction, "PostInstruction");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::LANGLEBRACKET)
			<< new ForwardNode(kmlRule::AnyTagContent));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(kmlRule::ConsumeWS, "ConsumeWS");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::WS));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(kmlRule::CommentTag, "CommentTag");
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(kmlRule::CommentContent)
			<< new ConsumeNode(kmlToken::DASH)
			<< new ConsumeNode(kmlToken::DASH)
			<< new ConsumeNode(kmlToken::RANGLEBRACKET)
			<< new ForwardNode(kmlRule::ConsumeWS));
		rules->push_back(rule);

		rule = new Rule(kmlRule::CommentContent, "CommentContent");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::EXCLAMATION)
			<< new ForwardNode(kmlRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::WS)
			<< new ForwardNode(kmlRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::QMARK)
			<< new ForwardNode(kmlRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::EQUALS)
			<< new ForwardNode(kmlRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::COMMA)
			<< new ForwardNode(kmlRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::SLASH)
			<< new ForwardNode(kmlRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::QUOTE)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::DOUBLEQUOTE)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::NUMBER)
			<< new ForwardNode(kmlRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::STRING)
			<< new ForwardNode(kmlRule::CommentContent));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(kmlRule::OptionalTextContent, "OptionalTextContent");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::EXCLAMATION)
			<< new ForwardNode(kmlRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::WS)
			<< new ForwardNode(kmlRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::QMARK)
			<< new ForwardNode(kmlRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::EQUALS)
			<< new ForwardNode(kmlRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::COMMA)
			<< new ForwardNode(kmlRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::SLASH)
			<< new ForwardNode(kmlRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::DASH)
			<< new ForwardNode(kmlRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::QUOTE)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::DOUBLEQUOTE)
			<< new ForwardNode(kmlRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::NUMBER)
			<< new ForwardNode(kmlRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::STRING)
			<< new ForwardNode(kmlRule::CommentContent));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(kmlRule::NormalTag, "NormalTag");
		(*rule) += &((*(new Sequence()))
			<< new ExtractNode(kmlToken::STRING, R"(Point)")
			<< new ForwardNode(kmlRule::OptionalAttr)
			<< new ForwardNode(kmlRule::GeometrySingleOrPairTag));
		(*rule) += &((*(new Sequence()))
			<< new ExtractNode(kmlToken::STRING, R"(LineString)")
			<< new ForwardNode(kmlRule::OptionalAttr)
			<< new ForwardNode(kmlRule::GeometrySingleOrPairTag));
		(*rule) += &((*(new Sequence()))
			<< new ExtractNode(kmlToken::STRING, R"(LinearRing)")
			<< new ForwardNode(kmlRule::OptionalAttr)
			<< new ForwardNode(kmlRule::GeometrySingleOrPairTag));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::STRING)
			<< new ForwardNode(kmlRule::OptionalAttr)
			<< new ForwardNode(kmlRule::GenericSingleOrPairTag));
		rules->push_back(rule);

		rule = new Rule(kmlRule::OptionalAttr, "OptionalAttr");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::WS)
			<< new ForwardNode(kmlRule::OptionalAttrPossible));
		(*rule) += &((*(new Sequence())));
		rules->push_back(rule);

		rule = new Rule(kmlRule::OptionalAttrPossible, "OptionalAttrPossible");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::STRING)
			<< new ConsumeNode(kmlToken::EQUALS)
			<< new ForwardNode(kmlRule::AttrValue)
			<< new ForwardNode(kmlRule::OptionalAttr));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(kmlRule::AttrValue, "AttrValue");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::QUOTE)
			<< new ForwardNode(kmlRule::AttrValueQuoted)
			<< new ConsumeNode(kmlToken::QUOTE));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::DOUBLEQUOTE)
			<< new ForwardNode(kmlRule::AttrValueDoubleQuoted)
			<< new ConsumeNode(kmlToken::DOUBLEQUOTE));
		rules->push_back(rule);

		rule = new Rule(kmlRule::AttrValueQuoted, "AttrValueQuoted");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::EXCLAMATION)
			<< new ForwardNode(kmlRule::AttrValueQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::WS)
			<< new ForwardNode(kmlRule::AttrValueQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::QMARK)
			<< new ForwardNode(kmlRule::AttrValueQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::EQUALS)
			<< new ForwardNode(kmlRule::AttrValueQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::LBRACKET)
			<< new ForwardNode(kmlRule::AttrValueQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::RBRACKET)
			<< new ForwardNode(kmlRule::AttrValueQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::COMMA)
			<< new ForwardNode(kmlRule::AttrValueQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::SLASH)
			<< new ForwardNode(kmlRule::AttrValueQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::DASH)
			<< new ForwardNode(kmlRule::AttrValueQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::DOUBLEQUOTE)
			<< new ForwardNode(kmlRule::AttrValueQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::NUMBER)
			<< new ForwardNode(kmlRule::AttrValueQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::STRING)
			<< new ForwardNode(kmlRule::AttrValueQuoted));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(kmlRule::AttrValueDoubleQuoted, "AttrValueDoubleQuoted");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::EXCLAMATION)
			<< new ForwardNode(kmlRule::AttrValueDoubleQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::WS)
			<< new ForwardNode(kmlRule::AttrValueDoubleQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::QMARK)
			<< new ForwardNode(kmlRule::AttrValueDoubleQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::EQUALS)
			<< new ForwardNode(kmlRule::AttrValueDoubleQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::LBRACKET)
			<< new ForwardNode(kmlRule::AttrValueDoubleQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::RBRACKET)
			<< new ForwardNode(kmlRule::AttrValueDoubleQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::COMMA)
			<< new ForwardNode(kmlRule::AttrValueDoubleQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::SLASH)
			<< new ForwardNode(kmlRule::AttrValueDoubleQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::DASH)
			<< new ForwardNode(kmlRule::AttrValueDoubleQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::QUOTE)
			<< new ForwardNode(kmlRule::AttrValueDoubleQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::NUMBER)
			<< new ForwardNode(kmlRule::AttrValueDoubleQuoted));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::STRING)
			<< new ForwardNode(kmlRule::AttrValueDoubleQuoted));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(kmlRule::GenericSingleOrPairTag, "GenericSingleOrPairTag");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::SLASH)
			<< new ConsumeNode(kmlToken::RANGLEBRACKET)
			<< new ForwardNode(kmlRule::ConsumeWS));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::RANGLEBRACKET)
			<< new ForwardNode(kmlRule::ConsumeWS)
			<< new ForwardNode(kmlRule::OptionalPairTagAnyContent));
		rules->push_back(rule);

		rule = new Rule(kmlRule::OptionalPairTagAnyContent, "OptionalPairTagAnyContent");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::LANGLEBRACKET)
			<< new ForwardNode(kmlRule::NestedOrEndingTag));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(kmlRule::OptionalTextContent)
			<< new ForwardNode(kmlRule::OptionalPairTagAnyContent));
		rules->push_back(rule);

		rule = new Rule(kmlRule::NestedOrEndingTag, "NestedOrEndingTag");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::SLASH)
			<< new ConsumeNode(kmlToken::STRING)
			<< new ConsumeNode(kmlToken::RANGLEBRACKET)
			<< new ForwardNode(kmlRule::ConsumeWS));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(kmlRule::AnyTagContent)
			<< new ForwardNode(kmlRule::OptionalPairTagAnyContent));
		rules->push_back(rule);

		rule = new Rule(kmlRule::GeometrySingleOrPairTag, "GeometrySingleOrPairTag");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::SLASH)
			<< new ConsumeNode(kmlToken::RANGLEBRACKET)
			<< new ForwardNode(kmlRule::ConsumeWS));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::RANGLEBRACKET)
			<< new ForwardNode(kmlRule::ConsumeWS)
			<< new ForwardNode(kmlRule::OptionalPairTagGeometryContent));
		rules->push_back(rule);

		rule = new Rule(kmlRule::OptionalPairTagGeometryContent, "OptionalPairTagGeometryContent");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::LANGLEBRACKET)
			<< new ForwardNode(kmlRule::GeometryNestedOrEndingTag));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(kmlRule::OptionalTextContent)
			<< new ForwardNode(kmlRule::OptionalPairTagGeometryContent));
		rules->push_back(rule);

		rule = new Rule(kmlRule::GeometryNestedOrEndingTag, "GeometryNestedOrEndingTag");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::SLASH)
			<< new ConsumeNode(kmlToken::STRING)
			<< new ConsumeNode(kmlToken::RANGLEBRACKET)
			<< new ForwardNode(kmlRule::ConsumeWS));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(kmlRule::AnyGeometryNestedTagContent)
			<< new ForwardNode(kmlRule::OptionalPairTagGeometryContent));
		rules->push_back(rule);

		rule = new Rule(kmlRule::AnyGeometryNestedTagContent, "AnyGeometryNestedTagContent");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::QMARK)
			<< new ConsumeNode(kmlToken::STRING)
			<< new ForwardNode(kmlRule::OptionalAttr)
			<< new ConsumeNode(kmlToken::QMARK)
			<< new ConsumeNode(kmlToken::RANGLEBRACKET)
			<< new ForwardNode(kmlRule::ConsumeWS)
			<< new ForwardNode(kmlRule::PostInstruction));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::EXCLAMATION)
			<< new ForwardNode(kmlRule::CommentOrCDATA));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(kmlRule::NormalGeometryNestedTag));
		rules->push_back(rule);

		rule = new Rule(kmlRule::NormalGeometryNestedTag, "NormalGeometryNestedTag");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::STRING, "coordinates")
			<< new ForwardNode(kmlRule::OptionalAttr)
			<< new ForwardNode(kmlRule::CoordinatesSingleOrPairTag));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::STRING)
			<< new ForwardNode(kmlRule::OptionalAttr)
			<< new ForwardNode(kmlRule::GenericSingleOrPairTag));
		rules->push_back(rule);

		rule = new Rule(kmlRule::CoordinatesSingleOrPairTag, "CoordinatesSingleOrPairTag");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::SLASH)
			<< new ConsumeNode(kmlToken::RANGLEBRACKET)
			<< new ForwardNode(kmlRule::ConsumeWS));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::RANGLEBRACKET)
			<< new ForwardNode(kmlRule::ConsumeWS)
			<< new ForwardNode(kmlRule::OptionalPairTagCoordinatesContent));
		rules->push_back(rule);

		rule = new Rule(kmlRule::OptionalPairTagCoordinatesContent, "OptionalPairTagCoordinatesContent");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::LANGLEBRACKET)
			<< new ForwardNode(kmlRule::CoordinatesNestedOrEndingTag));
		(*rule) += &((*(new Sequence()))
			<< new ExtractNode(kmlToken::NUMBER, ExtractNode::ExtractType::NUMBER)
			<< new CustomNode(kmlHandler::BufferNumber)
			<< new ForwardNode(kmlRule::CoordTupleTail)
			<< new ForwardNode(kmlRule::OptionalPairTagCoordinatesContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::DASH)
			<< new ExtractNode(kmlToken::NUMBER, ExtractNode::ExtractType::NUMBER)
			<< new CustomNode(kmlHandler::MakeNegative)
			<< new CustomNode(kmlHandler::BufferNumber)
			<< new ForwardNode(kmlRule::CoordTupleTail)
			<< new ForwardNode(kmlRule::OptionalPairTagCoordinatesContent));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(kmlRule::CoordinatesNestedOrEndingTag, "CoordinatesNestedOrEndingTag");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::SLASH)
			<< new ConsumeNode(kmlToken::STRING, "coordinates")
			<< new ConsumeNode(kmlToken::RANGLEBRACKET)
			<< new CustomNode(kmlHandler::CommitShape)
			<< new ForwardNode(kmlRule::ConsumeWS));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::EXCLAMATION)
			<< new ForwardNode(kmlRule::CommentOrCDATA)
			<< new ForwardNode(kmlRule::OptionalPairTagCoordinatesContent));
		rules->push_back(rule);

		rule = new Rule(kmlRule::Coord, "Coord");
		(*rule) += &((*(new Sequence()))
			<< new ExtractNode(kmlToken::NUMBER, ExtractNode::ExtractType::NUMBER)
			<< new CustomNode(kmlHandler::BufferNumber));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::DASH)
			<< new ExtractNode(kmlToken::NUMBER, ExtractNode::ExtractType::NUMBER)
			<< new CustomNode(kmlHandler::MakeNegative)
			<< new CustomNode(kmlHandler::BufferNumber));
		rules->push_back(rule);
		
		rule = new Rule(kmlRule::CoordTupleTail, "CoordTupleTail");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::COMMA)
			<< new ForwardNode(kmlRule::Coord)
			<< new ForwardNode(kmlRule::CoordTupleTail));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(kmlRule::ConsumeWS)
			<< new CustomNode(kmlHandler::BufferPoint));
		rules->push_back(rule);

		auto* stack = new Stack(first_rule);

		auto* visitor = new kmlVisitor(lexer, rules, stack);

		Parser* parser = new Parser(rules, first_rule, visitor, lexer, stack);

		return parser;
	};
}