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
		lexer->add_token_definition(kmlToken::SLASH, "/", "SLASH");
		lexer->add_token_definition(kmlToken::COMMA, R"(\,)", "COMMA");
		lexer->add_token_definition(kmlToken::ATTRIBUTE, R"("[^"]+")", "ATTRIBUTE");
		lexer->add_token_definition(kmlToken::NUMBER, R"(\-?\d+(\.\d+)?,\-?\d+(\.\d+)?(,\-?\d+(\.\d+)?)?( \s*\-?\d+(\.\d+)?,\-?\d+(\.\d+)?(,\-?\d+(\.\d+)?)?)*)", "NUMBER");
		lexer->add_token_definition(kmlToken::STRING, R"([^<>"&' =]+)", "STRING");
		return lexer;
	}

	Parser* setup_parser() {
		Lexer* lexer = new Lexer();
		lexer->add_token_definition(kmlToken::ERROR, "ERR", "ERROR");
		lexer->add_token_definition(kmlToken::LANGLEBRACKET, "<", "LANGLEBRACKET");
		lexer->add_token_definition(kmlToken::RANGLEBRACKET, ">", "RANGLEBRACKET");
		lexer->add_token_definition(kmlToken::DASH, R"(\-)", "DASH");
		lexer->add_token_definition(kmlToken::QMARK, R"(\?)", "QMARK");
		lexer->add_token_definition(kmlToken::EXCLAMATION, "!", "EXCLAMATION");
		lexer->add_token_definition(kmlToken::EQUALS, "=", "EQUALS");
		lexer->add_token_definition(kmlToken::WS, R"(\s+)", "WS");
		lexer->add_token_definition(kmlToken::SLASH, "/", "SLASH");
		lexer->add_token_definition(kmlToken::COMMA, R"(\,)", "COMMA");
		lexer->add_token_definition(kmlToken::ATTRIBUTE, R"("[^"]+")", "ATTRIBUTE");
		lexer->add_token_definition(kmlToken::NUMBER, R"(\-?\d+(\.\d+)?,\-?\d+(\.\d+)?(,\-?\d+(\.\d+)?)?( \s*\-?\d+(\.\d+)?,\-?\d+(\.\d+)?(,\-?\d+(\.\d+)?)?)*)", "NUMBER");
		lexer->add_token_definition(kmlToken::STRING, R"([^<>"&' =]+)", "STRING");

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
			<< new ConsumeNode(kmlToken::DASH)
			<< new ConsumeNode(kmlToken::DASH)
			<< new ForwardNode(kmlRule::CommentTag));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(kmlRule::NormalTag));
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
			<< new ConsumeNode(kmlToken::RANGLEBRACKET));
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
			<< new ConsumeNode(kmlToken::STRING)
			<< new ConsumeNode(kmlToken::EQUALS)
			<< new ConsumeNode(kmlToken::ATTRIBUTE)
			<< new ForwardNode(kmlRule::AttrSepOptional));
		(*rule) += &((*(new Sequence())));
		rules->push_back(rule);

		rule = new Rule(kmlRule::AttrSepOptional, "AttrSepOptional");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::WS)
			<< new ForwardNode(kmlRule::NextAttrPossible));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(kmlRule::NextAttrPossible, "NextAttrPossible");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::STRING)
			<< new ConsumeNode(kmlToken::EQUALS)
			<< new ConsumeNode(kmlToken::ATTRIBUTE)
			<< new ForwardNode(kmlRule::AttrSepOptional));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(kmlRule::GenericSingleOrPairTag, "GenericSingleOrPairTag");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::SLASH)
			<< new ConsumeNode(kmlToken::RANGLEBRACKET));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::RANGLEBRACKET)
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
			<< new ConsumeNode(kmlToken::RANGLEBRACKET));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(kmlRule::AnyTagContent)
			<< new ForwardNode(kmlRule::OptionalPairTagAnyContent));
		rules->push_back(rule);

		rule = new Rule(kmlRule::GeometrySingleOrPairTag, "GeometrySingleOrPairTag");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::SLASH)
			<< new ConsumeNode(kmlToken::RANGLEBRACKET));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::RANGLEBRACKET)
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
			<< new ConsumeNode(kmlToken::RANGLEBRACKET));
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
			<< new ForwardNode(kmlRule::PostInstruction));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::EXCLAMATION)
			<< new ConsumeNode(kmlToken::DASH)
			<< new ConsumeNode(kmlToken::DASH)
			<< new ForwardNode(kmlRule::CommentTag));
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
			<< new ConsumeNode(kmlToken::RANGLEBRACKET));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::RANGLEBRACKET)
			<< new ForwardNode(kmlRule::OptionalPairTagCoordinatesContent));
		rules->push_back(rule);

		rule = new Rule(kmlRule::OptionalPairTagCoordinatesContent, "OptionalPairTagCoordinatesContent");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::LANGLEBRACKET)
			<< new ForwardNode(kmlRule::CoordinatesEndingTag));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(kmlRule::CoordTuple));
		rules->push_back(rule);

		rule = new Rule(kmlRule::CoordinatesEndingTag, "CoordinatesEndingTag");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::SLASH)
			<< new ConsumeNode(kmlToken::STRING, "coordinates")
			<< new ConsumeNode(kmlToken::RANGLEBRACKET));
		rules->push_back(rule);

		rule = new Rule(kmlRule::CoordTuple, "CoordTuple");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::WS)
			<< new ExtractNode(kmlToken::NUMBER, ExtractNode::ExtractType::NUMBER)
			<< new ForwardNode(kmlRule::CoordTupleTail));	
		(*rule) += &((*(new Sequence()))
			<< new ExtractNode(kmlToken::NUMBER, ExtractNode::ExtractType::NUMBER)
			<< new ForwardNode(kmlRule::CoordTupleTail));
		rules->push_back(rule);

		rule = new Rule(kmlRule::CoordTupleTail, "CoordTupleTail");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::COMMA)
			<< new ExtractNode(kmlToken::NUMBER, ExtractNode::ExtractType::NUMBER)
			<< new ForwardNode(kmlRule::CoordTupleTail));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(kmlRule::NextTuple));
		rules->push_back(rule);

		rule = new Rule(kmlRule::NextTuple, "NextTuple");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(kmlToken::WS)
			<< new ForwardNode(kmlRule::CoordTuple));
		(*rule) += new Sequence();
		rules->push_back(rule);

		auto* visitor = new kmlVisitor(lexer, rules);

		Parser* parser = new Parser(rules, first_rule, visitor, lexer);

		return parser;
	};
}