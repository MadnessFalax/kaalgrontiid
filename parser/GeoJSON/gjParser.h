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
#include "../LL1/parser/node/pEntryNode.h"

namespace nsGeoJSON {

	using Lexer = nspLexer::pLexer<gjToken>;
	using Parser = nspParser::pParser<gjToken, gjRule, gjHandler, gjVisitor>;
	using Rule = nspParser::pRule<gjToken, gjRule>;
	template<class T>
	using Array = nspArray::pArray<T>;
	using Sequence = nspParser::pSequence<gjToken, gjRule>;
	using ConsumeNode = nspParser::pConsumeNode<gjToken, gjRule>;
	using CustomNode = nspParser::pCustomNode<gjToken, gjRule, gjHandler>;
	using ExtractNode = nspParser::pExtractNode<gjToken, gjRule>;
	using ForwardNode = nspParser::pForwardNode<gjToken, gjRule>;
	using EntryNode = nspParser::pEntryNode<gjToken, gjRule>;

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

		// Thanks god for Github Copilot!!!!!!!!!!!!
		auto* first_rule = new Rule(gjRule::EntryRule, "EntryRule");
		(*first_rule) += &((*(new Sequence())) << new EntryNode(gjRule::GeoJSON, gjRule::EntryRule));
		rules->push_back(first_rule);

		auto rule = new Rule(gjRule::GeoJSON, "GeoJSON");
		(*rule) += &((*(new Sequence())) << new ConsumeNode(gjToken::LBRACE) << new ForwardNode(gjRule::GJProperties) << new ConsumeNode(gjToken::RBRACE));
		rules->push_back(rule);
		
		rule = new Rule(gjRule::GJProperties, "GJProperties");
		(*rule) += &((*(new Sequence())) << new ForwardNode(gjRule::GJProperty) << new ForwardNode(gjRule::GJPropertiesTail));
		rules->push_back(rule);

		rule = new Rule(gjRule::GJPropertiesTail, "GJPropertiesTail");
		(*rule) += &((*(new Sequence())) << new ConsumeNode(gjToken::COMMA) << new ForwardNode(gjRule::GJProperty) << new ForwardNode(gjRule::GJPropertiesTail));
		(*rule) += &((*(new Sequence())));
		rules->push_back(rule);

		rule = new Rule(gjRule::GJProperty, "GJProperty");
		(*rule) += &((*(new Sequence())) 
			<< new ConsumeNode(gjToken::STRING, "\"type\"") 
			<< new ConsumeNode(gjToken::COLON) 
			<< new ForwardNode(gjRule::TypeVal));
		(*rule) += &((*(new Sequence())) 
			<< new ConsumeNode(gjToken::STRING, "\"features\"") 
			<< new ConsumeNode(gjToken::COLON) 
			<< new ConsumeNode(gjToken::LBRACKET) 
			<< new ForwardNode(gjRule::FeatureValues) 
			<< new ConsumeNode(gjToken::RBRACKET));
		(*rule) += &((*(new Sequence())) 
			<< new ConsumeNode(gjToken::STRING, "\"properties\"") 
			<< new ConsumeNode(gjToken::COLON) 
			<< new ConsumeNode(gjToken::LBRACE) 
			<< new ForwardNode(gjRule::GenericProperties) 
			<< new ConsumeNode(gjToken::RBRACE));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING, "\"geometry\"")
			<< new ConsumeNode(gjToken::COLON)
			<< new ConsumeNode(gjToken::LBRACE)
			<< new ForwardNode(gjRule::GProperties)
			<< new ConsumeNode(gjToken::RBRACE));
		(*rule) += &((*(new Sequence())) 
			<< new ConsumeNode(gjToken::STRING, "\"coordinates\"") 
			<< new ConsumeNode(gjToken::COLON) 
			<< new ConsumeNode(gjToken::LBRACKET) 
			<< new ForwardNode(gjRule::CoordinatesRoot) 
			<< new ConsumeNode(gjToken::RBRACKET));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING, "\"geometries\"")
			<< new ConsumeNode(gjToken::COLON)
			<< new ConsumeNode(gjToken::LBRACKET)
			<< new ForwardNode(gjRule::GeometryValues)
			<< new ConsumeNode(gjToken::RBRACKET));
		(*rule) += &((*(new Sequence())) 
			<< new ForwardNode(gjRule::GenericProperty));
		rules->push_back(rule);

		rule = new Rule(gjRule::GeometryType, "GeometryType");
		(*rule) += &((*(new Sequence())) 
			<< new ExtractNode(gjToken::STRING, "\"Point\""));
		(*rule) += &((*(new Sequence()))
			<< new ExtractNode(gjToken::STRING, "\"LineString\""));
		(*rule) += &((*(new Sequence()))
			<< new ExtractNode(gjToken::STRING, "\"Polygon\""));
		(*rule) += &((*(new Sequence()))
			<< new ExtractNode(gjToken::STRING, "\"MultiPoint\""));
		(*rule) += &((*(new Sequence()))
			<< new ExtractNode(gjToken::STRING, "\"MultiLineString\""));
		(*rule) += &((*(new Sequence()))
			<< new ExtractNode(gjToken::STRING, "\"MultiPolygon\""));
		(*rule) += &((*(new Sequence()))
			<< new ExtractNode(gjToken::STRING, "\"GeometryCollection\""));
		rules->push_back(rule);

		rule = new Rule(gjRule::TypeVal, "TypeVal");
		(*rule) += &((*(new Sequence())) << new ConsumeNode(gjToken::STRING, "\"FeatureCollection\""));
		(*rule) += &((*(new Sequence())) << new ConsumeNode(gjToken::STRING, "\"Feature\""));
		(*rule) += &((*(new Sequence())) << new ForwardNode(gjRule::GeometryType));
		rules->push_back(rule);

		rule = new Rule(gjRule::GenericProperties, "GenericProperties");
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(gjRule::GenericProperty)
			<< new ForwardNode(gjRule::GenericPropertiesTail));
		rules->push_back(rule);

		rule = new Rule(gjRule::GenericPropertiesTail, "GenericPropertiesTail");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::COMMA)
			<< new ForwardNode(gjRule::GenericProperty)
			<< new ForwardNode(gjRule::GenericPropertiesTail));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(gjRule::GenericProperty, "GenericProperty");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING)
			<< new ConsumeNode(gjToken::COLON)
			<< new ForwardNode(gjRule::Value));
		rules->push_back(rule);

		rule = new Rule(gjRule::FeatureValues, "FeatureValues");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::LBRACE)
			<< new ForwardNode(gjRule::FProperties)
			<< new ConsumeNode(gjToken::RBRACE)
			<< new ForwardNode(gjRule::FeatureValuesTail));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(gjRule::GeometryValues, "GeometryValues");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::LBRACE)
			<< new ForwardNode(gjRule::GProperties)
			<< new ConsumeNode(gjToken::RBRACE)
			<< new ForwardNode(gjRule::GeometryValuesTail));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(gjRule::GeometryValuesTail, "GeometryValuesTail");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::COMMA)
			<< new ConsumeNode(gjToken::LBRACE)
			<< new ForwardNode(gjRule::GProperties)
			<< new ConsumeNode(gjToken::RBRACE)
			<< new ForwardNode(gjRule::GeometryValuesTail));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(gjRule::FeatureValuesTail, "FeatureValuesTail");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::COMMA)
			<< new ConsumeNode(gjToken::LBRACE)
			<< new ForwardNode(gjRule::FProperties)
			<< new ConsumeNode(gjToken::RBRACE)
			<< new ForwardNode(gjRule::FeatureValuesTail));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(gjRule::FProperties, "FProperties");
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(gjRule::FProperty)
			<< new ForwardNode(gjRule::FPropertiesTail));
		rules->push_back(rule);

		rule = new Rule(gjRule::FPropertiesTail, "FPropertiesTail");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::COMMA)
			<< new ForwardNode(gjRule::FProperty)
			<< new ForwardNode(gjRule::FPropertiesTail));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(gjRule::FProperty, "FProperty");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING, "\"type\"")
			<< new ConsumeNode(gjToken::COLON)
			<< new ConsumeNode(gjToken::STRING, "\"Feature\""));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING, "\"geometry\"")
			<< new ConsumeNode(gjToken::COLON)
			<< new ConsumeNode(gjToken::LBRACE)
			<< new ForwardNode(gjRule::GProperties)
			<< new ConsumeNode(gjToken::RBRACE));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING, "\"properties\"")
			<< new ConsumeNode(gjToken::COLON)
			<< new ConsumeNode(gjToken::LBRACE)
			<< new ForwardNode(gjRule::GenericProperties)
			<< new ConsumeNode(gjToken::RBRACE));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(gjRule::GenericProperty));
		rules->push_back(rule);

		rule = new Rule(gjRule::GProperties, "GProperties");
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(gjRule::GProperty)
			<< new ForwardNode(gjRule::GPropertiesTail));
		rules->push_back(rule);

		rule = new Rule(gjRule::GPropertiesTail, "GPropertiesTail");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::COMMA)
			<< new ForwardNode(gjRule::GProperty)
			<< new ForwardNode(gjRule::GPropertiesTail));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(gjRule::GProperty, "GProperty");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING, "\"type\"")
			<< new ConsumeNode(gjToken::COLON)
			<< new ForwardNode(gjRule::GeometryType));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING, "\"coordinates\"")
			<< new ConsumeNode(gjToken::COLON)
			<< new ConsumeNode(gjToken::LBRACKET)
			<< new ForwardNode(gjRule::CoordinatesRoot)
			<< new CustomNode(gjHandler::DepthOut)
			<< new ConsumeNode(gjToken::RBRACKET)
			<< new CustomNode(gjHandler::CommitShape));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING, "\"geometries\"")
			<< new ConsumeNode(gjToken::COLON)
			<< new ConsumeNode(gjToken::LBRACKET)
			<< new ForwardNode(gjRule::GeometryValues)
			<< new ConsumeNode(gjToken::RBRACKET));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(gjRule::GenericProperty));
		rules->push_back(rule);

		rule = new Rule(gjRule::CoordinatesRoot, "CoordinatesRoot");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::LBRACKET)
			<< new ForwardNode(gjRule::CoordinatesRoot)
			<< new CustomNode(gjHandler::DepthOut)
			<< new ConsumeNode(gjToken::RBRACKET)
			<< new CustomNode(gjHandler::CommitShape)
			<< new ForwardNode(gjRule::CoordinatesRootTail));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(gjRule::Coordinates)
			<< new CustomNode(gjHandler::BufferPoint));
		rules->push_back(rule);

		rule = new Rule(gjRule::CoordinatesRootTail, "CoordinatesRootTail");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::COMMA)
			<< new ConsumeNode(gjToken::LBRACKET)
			<< new ForwardNode(gjRule::CoordinatesRoot)
			<< new CustomNode(gjHandler::DepthOut)
			<< new ConsumeNode(gjToken::RBRACKET)
			<< new CustomNode(gjHandler::CommitShape)
			<< new ForwardNode(gjRule::CoordinatesRootTail));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(gjRule::Coordinates, "Coordinates");
		(*rule) += &((*(new Sequence()))
			<< new ExtractNode(gjToken::NUMBER, ExtractNode::ExtractType::NUMBER)
			<< new CustomNode(gjHandler::BufferNumber)
			<< new ForwardNode(gjRule::CoordinatesTail));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(gjRule::CoordinatesTail, "CoordinatesTail");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::COMMA)
		    << new ExtractNode(gjToken::NUMBER, ExtractNode::ExtractType::NUMBER)
			<< new CustomNode(gjHandler::BufferNumber)
			<< new ForwardNode(gjRule::CoordinatesTail));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(gjRule::Value, "Value");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::TRUE));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::FALSE));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::NULL));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::LBRACE)
			<< new ForwardNode(gjRule::GenericProperties)
			<< new ConsumeNode(gjToken::RBRACE));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::LBRACKET)
			<< new ForwardNode(gjRule::GenericArrayValues)
			<< new ConsumeNode(gjToken::RBRACKET));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::NUMBER));
		rules->push_back(rule);

		rule = new Rule(gjRule::GenericArrayValues, "GenericArrayValues");
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(gjRule::Value)
			<< new ForwardNode(gjRule::GenericArrayValuesTail));
		rules->push_back(rule);

		rule = new Rule(gjRule::GenericArrayValuesTail, "GenericArrayValuesTail");
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::COMMA)
			<< new ForwardNode(gjRule::Value)
			<< new ForwardNode(gjRule::GenericArrayValuesTail));
		(*rule) += new Sequence();
		rules->push_back(rule);

		auto* visitor = new gjVisitor(l, rules);	

		Parser* p = new Parser(rules, first_rule, visitor, l);

		return p;
	}
}