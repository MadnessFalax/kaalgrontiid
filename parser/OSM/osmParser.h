#pragma once
#include "../LL1/lexer/pLexer.h"
#include "../LL1/parser/pParser.h"
#include "../../container/pArray.h"
#include "osmEnums.h"
#include "osmVisitor.h"
#include "../LL1/parser/node/pConsumeNode.h"
#include "../LL1/parser/node/pCustomNode.h"
#include "../LL1/parser/node/pExtractNode.h"
#include "../LL1/parser/node/pForwardNode.h"
#include "../LL1/parser/node/pEntryNode.h"

namespace nsOSM {

	using Lexer = nspLexer::pLexer<osmToken>;
	using Parser = nspParser::pParser<osmToken, osmRule, osmHandler, osmVisitor>;
	using Rule = nspParser::pRule<osmToken, osmRule>;
	using Stack = nspParser::pParserStack<osmToken, osmRule, osmHandler>;
	template<class T>
	using Array = nspArray::pArray<T>;
	using Sequence = nspParser::pSequence<osmToken, osmRule>;
	using ConsumeNode = nspParser::pConsumeNode<osmToken, osmRule>;
	using CustomNode = nspParser::pCustomNode<osmToken, osmRule, osmHandler>;
	using ExtractNode = nspParser::pExtractNode<osmToken, osmRule>;
	using ForwardNode = nspParser::pForwardNode<osmToken, osmRule>;
	using EntryNode = nspParser::pEntryNode<osmToken, osmRule>;

	Lexer* setup_lexer() {
		Lexer* lexer = new Lexer();
		lexer->add_token_definition(osmToken::ERROR, "ERR", "ERROR");
		lexer->add_token_definition(osmToken::LANGLEBRACKET, "<", "LANGLEBRACKET");
		lexer->add_token_definition(osmToken::RANGLEBRACKET, ">", "RANGLEBRACKET");
		lexer->add_token_definition(osmToken::RBRACKET, "]", "RBRACKET");
		lexer->add_token_definition(osmToken::LBRACKET, "[", "LBRACKET");
		lexer->add_token_definition(osmToken::DASH, R"(\-)", "DASH");
		lexer->add_token_definition(osmToken::QMARK, R"(\?)", "QMARK");
		lexer->add_token_definition(osmToken::EXCLAMATION, "!", "EXCLAMATION");
		lexer->add_token_definition(osmToken::EQUALS, "=", "EQUALS");
		lexer->add_token_definition(osmToken::WS, R"(\s+)", "WS");
		lexer->add_token_definition(osmToken::SLASH, "/", "SLASH");
		lexer->add_token_definition(osmToken::COMMA, R"(\,)", "COMMA");
		lexer->add_token_definition(osmToken::ATTRIBUTE, R"("[^"]+")", "ATTRIBUTE");
		lexer->add_token_definition(osmToken::NUMBER, R"(\-?\d+(\.\d+)?,\-?\d+(\.\d+)?(,\-?\d+(\.\d+)?)?( \s*\-?\d+(\.\d+)?,\-?\d+(\.\d+)?(,\-?\d+(\.\d+)?)?)*)", "NUMBER");
		lexer->add_token_definition(osmToken::STRING, R"([^<>" =]+)", "STRING");
		return lexer;
	}

	Parser* setup_parser() {
		Lexer* lexer = new Lexer();
		lexer->add_token_definition(osmToken::ERROR, "ERR", "ERROR");
		lexer->add_token_definition(osmToken::LANGLEBRACKET, "<", "LANGLEBRACKET");
		lexer->add_token_definition(osmToken::RANGLEBRACKET, ">", "RANGLEBRACKET");
		lexer->add_token_definition(osmToken::RBRACKET, R"(\])", "RBRACKET");
		lexer->add_token_definition(osmToken::LBRACKET, R"(\[)", "LBRACKET");
		lexer->add_token_definition(osmToken::DASH, R"(\-)", "DASH");
		lexer->add_token_definition(osmToken::QMARK, R"(\?)", "QMARK");
		lexer->add_token_definition(osmToken::EXCLAMATION, "!", "EXCLAMATION");
		lexer->add_token_definition(osmToken::EQUALS, "=", "EQUALS");
		lexer->add_token_definition(osmToken::WS, R"(\s+)", "WS");
		lexer->add_token_definition(osmToken::SLASH, "/", "SLASH");
		lexer->add_token_definition(osmToken::COMMA, R"(\,)", "COMMA");
		lexer->add_token_definition(osmToken::ATTRIBUTE, R"("[^"]+")", "ATTRIBUTE");
		lexer->add_token_definition(osmToken::NUMBER, R"(\-?\d+(\.\d+)?,\-?\d+(\.\d+)?(,\-?\d+(\.\d+)?)?( \s*\-?\d+(\.\d+)?,\-?\d+(\.\d+)?(,\-?\d+(\.\d+)?)?)*)", "NUMBER");
		lexer->add_token_definition(osmToken::STRING, R"([^\[\]<>" =]+)", "STRING");

		auto* rules = new Array<Rule*>();

		auto* first_rule = new Rule(osmRule::EntryRule, "EntryRule");
		(*first_rule) += &((*(new Sequence())) << new EntryNode(osmRule::OSM, osmRule::EntryRule));
		rules->push_back(first_rule);

		auto* rule = new Rule(osmRule::OSM, "OSM");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::LANGLEBRACKET)
			<< new ForwardNode(osmRule::AnyTagContent));
		rules->push_back(rule);

		rule = new Rule(osmRule::AnyTagContent, "AnyTagContent");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::QMARK)
			<< new ConsumeNode(osmToken::STRING)
			<< new ForwardNode(osmRule::OptionalAttr)
			<< new ConsumeNode(osmToken::QMARK)
			<< new ConsumeNode(osmToken::RANGLEBRACKET)
			<< new ForwardNode(osmRule::ConsumeWS)
			<< new ForwardNode(osmRule::PostInstruction));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::EXCLAMATION)
			<< new ForwardNode(osmRule::CommentOrCDATA));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(osmRule::NormalTag));
		rules->push_back(rule);

		rule = new Rule(osmRule::CommentOrCDATA, "CommentOrCDATA");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::DASH)
			<< new ConsumeNode(osmToken::DASH)
			<< new ForwardNode(osmRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::LBRACKET)
			<< new ConsumeNode(osmToken::STRING, "CDATA")
			<< new ConsumeNode(osmToken::LBRACKET)
			<< new ForwardNode(osmRule::CDATAContent)
			<< new ConsumeNode(osmToken::RANGLEBRACKET)
			<< new ForwardNode(osmRule::ConsumeWS));
		rules->push_back(rule);

		rule = new Rule(osmRule::CDATAContent, "CDATAContent");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::EXCLAMATION)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::QMARK)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::LANGLEBRACKET)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::RANGLEBRACKET)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::DASH)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::LBRACKET)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::RBRACKET)
			<< new ForwardNode(osmRule::CDATAFirstRBracket));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::EQUALS)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::WS)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::SLASH)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::COMMA)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::ATTRIBUTE)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::NUMBER)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::STRING)
			<< new ForwardNode(osmRule::CDATAContent));
		rules->push_back(rule);

		rule = new Rule(osmRule::CDATAFirstRBracket, "CDATAFirstRBracket");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::EXCLAMATION)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::QMARK)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::LANGLEBRACKET)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::RANGLEBRACKET)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::DASH)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::LBRACKET)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::RBRACKET)
			<< new ForwardNode(osmRule::CDATASecondRBracket));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::EQUALS)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::WS)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::SLASH)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::COMMA)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::ATTRIBUTE)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::NUMBER)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::STRING)
			<< new ForwardNode(osmRule::CDATAContent));
		rules->push_back(rule);

		rule = new Rule(osmRule::CDATASecondRBracket, "CDATASecondRBracket");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::EXCLAMATION)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::QMARK)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::LANGLEBRACKET)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::DASH)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::LBRACKET)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::RBRACKET)
			<< new ForwardNode(osmRule::CDATASecondRBracket));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::EQUALS)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::WS)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::SLASH)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::COMMA)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::ATTRIBUTE)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::NUMBER)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::STRING)
			<< new ForwardNode(osmRule::CDATAContent));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(osmRule::PostInstruction, "PostInstruction");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::LANGLEBRACKET)
			<< new ForwardNode(osmRule::AnyTagContent));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(osmRule::ConsumeWS, "ConsumeWS");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::WS));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(osmRule::CommentTag, "CommentTag");
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(osmRule::CommentContent)
			<< new ConsumeNode(osmToken::DASH)
			<< new ConsumeNode(osmToken::DASH)
			<< new ConsumeNode(osmToken::RANGLEBRACKET)
			<< new ForwardNode(osmRule::ConsumeWS));
		rules->push_back(rule);

		rule = new Rule(osmRule::CommentContent, "CommentContent");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::EXCLAMATION)
			<< new ForwardNode(osmRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::WS)
			<< new ForwardNode(osmRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::QMARK)
			<< new ForwardNode(osmRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::EQUALS)
			<< new ForwardNode(osmRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::COMMA)
			<< new ForwardNode(osmRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::SLASH)
			<< new ForwardNode(osmRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::NUMBER)
			<< new ForwardNode(osmRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::STRING)
			<< new ForwardNode(osmRule::CommentContent));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(osmRule::OptionalTextContent, "OptionalTextContent");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::EXCLAMATION)
			<< new ForwardNode(osmRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::WS)
			<< new ForwardNode(osmRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::QMARK)
			<< new ForwardNode(osmRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::EQUALS)
			<< new ForwardNode(osmRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::COMMA)
			<< new ForwardNode(osmRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::SLASH)
			<< new ForwardNode(osmRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::DASH)
			<< new ForwardNode(osmRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::NUMBER)
			<< new ForwardNode(osmRule::CommentContent));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::STRING)
			<< new ForwardNode(osmRule::CommentContent));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(osmRule::NormalTag, "NormalTag");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::STRING, "node")
			<< new ForwardNode(osmRule::NodeAttr)
			<< new ForwardNode(osmRule::GenericSingleOrPairTag));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::STRING, "way")
			<< new ForwardNode(osmRule::WayAttr)
			<< new ForwardNode(osmRule::WaySingleOrPairTag));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::STRING)
			<< new ForwardNode(osmRule::OptionalAttr)
			<< new ForwardNode(osmRule::GenericSingleOrPairTag));
		rules->push_back(rule);

		rule = new Rule(osmRule::OptionalAttr, "OptionalAttr");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::WS)
			<< new ForwardNode(osmRule::AttrPossible));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(osmRule::AttrPossible, "AttrPossible");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::STRING)
			<< new ConsumeNode(osmToken::EQUALS)
			<< new ConsumeNode(osmToken::ATTRIBUTE)
			<< new ForwardNode(osmRule::OptionalAttr));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(osmRule::GenericSingleOrPairTag, "GenericSingleOrPairTag");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::SLASH)
			<< new ConsumeNode(osmToken::RANGLEBRACKET)
			<< new ForwardNode(osmRule::ConsumeWS));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::RANGLEBRACKET)
			<< new ForwardNode(osmRule::ConsumeWS)
			<< new ForwardNode(osmRule::OptionalPairTagAnyContent));
		rules->push_back(rule);

		rule = new Rule(osmRule::OptionalPairTagAnyContent, "OptionalPairTagAnyContent");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::LANGLEBRACKET)
			<< new ForwardNode(osmRule::NestedOrEndingTag));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(osmRule::OptionalTextContent)
			<< new ForwardNode(osmRule::OptionalPairTagAnyContent));
		rules->push_back(rule);

		rule = new Rule(osmRule::NestedOrEndingTag, "NestedOrEndingTag");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::SLASH)
			<< new ConsumeNode(osmToken::STRING)
			<< new ConsumeNode(osmToken::RANGLEBRACKET)
			<< new ForwardNode(osmRule::ConsumeWS));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(osmRule::AnyTagContent)
			<< new ForwardNode(osmRule::OptionalPairTagAnyContent));
		rules->push_back(rule);

		rule = new Rule(osmRule::NodeAttr, "NodeAttr");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::WS)
			<< new ForwardNode(osmRule::NodeAttrPossible));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(osmRule::NodeAttrPossible, "NodeAttrPossible");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::STRING, "id")
			<< new ConsumeNode(osmToken::EQUALS)
			<< new ExtractNode(osmToken::ATTRIBUTE, ExtractNode::ExtractType::STRING)
			<< new ForwardNode(osmRule::NodeAttr));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::STRING, "lat")
			<< new ConsumeNode(osmToken::EQUALS)
			<< new ExtractNode(osmToken::ATTRIBUTE, ExtractNode::ExtractType::STRING)
			<< new ForwardNode(osmRule::NodeAttr));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::STRING, "lon")
			<< new ConsumeNode(osmToken::EQUALS)
			<< new ExtractNode(osmToken::ATTRIBUTE, ExtractNode::ExtractType::STRING)
			<< new ForwardNode(osmRule::NodeAttr));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::STRING)
			<< new ConsumeNode(osmToken::EQUALS)
			<< new ConsumeNode(osmToken::ATTRIBUTE)
			<< new ForwardNode(osmRule::NodeAttr));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(osmRule::WayAttr, "WayAttr");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::WS)
			<< new ForwardNode(osmRule::WayAttrPossible));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(osmRule::WayAttrPossible, "WayAttrPossible");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::STRING, "id")
			<< new ConsumeNode(osmToken::EQUALS)
			<< new ConsumeNode(osmToken::ATTRIBUTE)
			<< new ForwardNode(osmRule::WayAttr));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::STRING)
			<< new ConsumeNode(osmToken::EQUALS)
			<< new ConsumeNode(osmToken::ATTRIBUTE)
			<< new ForwardNode(osmRule::WayAttr));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(osmRule::WaySingleOrPairTag, "WaySingleOrPairTag");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::SLASH)
			<< new ConsumeNode(osmToken::RANGLEBRACKET)
			<< new ForwardNode(osmRule::ConsumeWS));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::RANGLEBRACKET)
			<< new ForwardNode(osmRule::ConsumeWS)
			<< new ForwardNode(osmRule::OptionalPairTagAnyContent));
		rules->push_back(rule);

		rule = new Rule(osmRule::OptionalPairTagWayContent, "OptionalPairTagWayContent");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::LANGLEBRACKET)
			<< new ForwardNode(osmRule::WayNestedOrEndingTag));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(osmRule::OptionalTextContent)
			<< new ForwardNode(osmRule::OptionalPairTagWayContent));
		rules->push_back(rule);

		rule = new Rule(osmRule::WayNestedOrEndingTag, "WayNestedOrEndingTag");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::SLASH)
			<< new ConsumeNode(osmToken::STRING)
			<< new ConsumeNode(osmToken::RANGLEBRACKET)
			<< new ForwardNode(osmRule::ConsumeWS));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(osmRule::AnyWayNestedTagContent)
			<< new ForwardNode(osmRule::OptionalPairTagWayContent));
		rules->push_back(rule);

		rule = new Rule(osmRule::AnyWayNestedTagContent, "AnyWayNestedTagContent");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::QMARK)
			<< new ConsumeNode(osmToken::STRING)
			<< new ForwardNode(osmRule::OptionalAttr)
			<< new ConsumeNode(osmToken::QMARK)
			<< new ConsumeNode(osmToken::RANGLEBRACKET)
			<< new ForwardNode(osmRule::ConsumeWS)
			<< new ForwardNode(osmRule::PostInstruction));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::EXCLAMATION)
			<< new ForwardNode(osmRule::CommentOrCDATA));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(osmRule::NormalWayNestedTag));
		rules->push_back(rule);

		rule = new Rule(osmRule::NormalWayNestedTag, "NormalWayNestedTag");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::STRING, "nd")
			<< new ForwardNode(osmRule::NdAttr)
			<< new ForwardNode(osmRule::GenericSingleOrPairTag));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::STRING)
			<< new ForwardNode(osmRule::OptionalAttr)
			<< new ForwardNode(osmRule::GenericSingleOrPairTag));
		rules->push_back(rule);

		rule = new Rule(osmRule::NdAttr, "NdAttr");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::WS)
			<< new ForwardNode(osmRule::NdAttrPossible));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(osmRule::NdAttrPossible, "NdAttrPossible");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::STRING, "ref")
			<< new ConsumeNode(osmToken::EQUALS)
			<< new ExtractNode(osmToken::ATTRIBUTE, ExtractNode::ExtractType::STRING)
			<< new ForwardNode(osmRule::NdAttr));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(osmToken::STRING)
			<< new ConsumeNode(osmToken::EQUALS)
			<< new ConsumeNode(osmToken::ATTRIBUTE)
			<< new ForwardNode(osmRule::NdAttr));
		(*rule) += new Sequence();
		rules->push_back(rule);

		auto* stack = new Stack(first_rule);

		auto* visitor = new osmVisitor(lexer, rules, stack);

		Parser* parser = new Parser(rules, first_rule, visitor, lexer, stack);

		return parser;
	};
}