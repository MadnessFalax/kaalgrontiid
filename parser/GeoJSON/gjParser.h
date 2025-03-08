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
		

		// Thanks god for Github Copilot!!!!!!!!!!!!
		auto* rule = new Rule(gjRule::GeoJSON);
		(*rule) += &((*(new Sequence())) << new ConsumeNode(gjToken::LBRACE) << new ForwardNode(gjRule::GJProperties) << new ConsumeNode(gjToken::RBRACE));
		rules->push_back(rule);
		
		rule = new Rule(gjRule::GJProperties);
		(*rule) += &((*(new Sequence())) << new ForwardNode(gjRule::GJProperty) << new ForwardNode(gjRule::GJPropertiesTail));
		rules->push_back(rule);

		rule = new Rule(gjRule::GJPropertiesTail);
		(*rule) += &((*(new Sequence())) << new ConsumeNode(gjToken::COMMA) << new ForwardNode(gjRule::GJProperty) << new ForwardNode(gjRule::GJPropertiesTail));
		(*rule) += &((*(new Sequence())));
		rules->push_back(rule);

		rule = new Rule(gjRule::GJProperty);
		(*rule) += &((*(new Sequence())) 
			<< new ConsumeNode(gjToken::STRING, "type") 
			<< new ConsumeNode(gjToken::COLON) 
			<< new ForwardNode(gjRule::TypeVal));
		(*rule) += &((*(new Sequence())) 
			<< new ConsumeNode(gjToken::STRING, "features") 
			<< new ConsumeNode(gjToken::COLON) 
			<< new ConsumeNode(gjToken::LBRACKET) 
			<< new ForwardNode(gjRule::FeatureValues) 
			<< new ConsumeNode(gjToken::RBRACKET));
		(*rule) += &((*(new Sequence())) 
			<< new ConsumeNode(gjToken::STRING, "properties") 
			<< new ConsumeNode(gjToken::COLON) 
			<< new ConsumeNode(gjToken::LBRACE) 
			<< new ForwardNode(gjRule::GenericProperties) 
			<< new ConsumeNode(gjToken::RBRACE));
		(*rule) += &((*(new Sequence())) 
			<< new ConsumeNode(gjToken::STRING, "geometry") 
			<< new ConsumeNode(gjToken::COLON) 
			<< new ConsumeNode(gjToken::LBRACE) 
			<< new ForwardNode(gjRule::GProperties) 
			<< new ConsumeNode(gjToken::RBRACE));
		(*rule) += &((*(new Sequence())) 
			<< new ConsumeNode(gjToken::STRING, "coordinates") 
			<< new ConsumeNode(gjToken::COLON) 
			<< new ConsumeNode(gjToken::LBRACKET) 
			<< new ForwardNode(gjRule::CoordinatesRoot) 
			<< new ConsumeNode(gjToken::RBRACKET));
		(*rule) += &((*(new Sequence())) 
			<< new ForwardNode(gjRule::GenericProperty));
		rules->push_back(rule);

		rule = new Rule(gjRule::GeometryType);
		(*rule) += &((*(new Sequence())) 
			<< new ConsumeNode(gjToken::STRING, "Point"));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING, "LineString"));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING, "Polygon"));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING, "MultiPoint"));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING, "MultiLineString"));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING, "MultiPolygon"));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING, "GeometryCollection"));
		rules->push_back(rule);

		rule = new Rule(gjRule::TypeVal);
		(*rule) += &((*(new Sequence())) << new ConsumeNode(gjToken::STRING, "FeatureCollection"));
		(*rule) += &((*(new Sequence())) << new ConsumeNode(gjToken::STRING, "Feature"));
		(*rule) += &((*(new Sequence())) << new ForwardNode(gjRule::GeometryType));
		rules->push_back(rule);

		rule = new Rule(gjRule::GenericProperties);
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(gjRule::GenericProperty)
			<< new ForwardNode(gjRule::GenericPropertiesTail));
		rules->push_back(rule);

		rule = new Rule(gjRule::GenericPropertiesTail);
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::COMMA)
			<< new ForwardNode(gjRule::GenericProperty)
			<< new ForwardNode(gjRule::GenericPropertiesTail));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(gjRule::GenericProperty);
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING)
			<< new ConsumeNode(gjToken::COLON)
			<< new ForwardNode(gjRule::Value));
		rules->push_back(rule);

		rule = new Rule(gjRule::FeatureValues);
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::LBRACE)
			<< new ForwardNode(gjRule::FProperties)
			<< new ConsumeNode(gjToken::RBRACE)
			<< new ForwardNode(gjRule::FeatureValuesTail));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(gjRule::FeatureValuesTail);
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::COMMA)
			<< new ConsumeNode(gjToken::LBRACE)
			<< new ForwardNode(gjRule::FProperties)
			<< new ConsumeNode(gjToken::RBRACE)
			<< new ForwardNode(gjRule::FeatureValuesTail));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(gjRule::FProperties);
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(gjRule::FProperty)
			<< new ForwardNode(gjRule::FPropertiesTail));
		rules->push_back(rule);

		rule = new Rule(gjRule::FPropertiesTail);
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::COMMA)
			<< new ForwardNode(gjRule::FProperty)
			<< new ForwardNode(gjRule::FPropertiesTail));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(gjRule::FProperty);
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING, "type")
			<< new ConsumeNode(gjToken::COLON)
			<< new ConsumeNode(gjToken::STRING, "feature"));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING, "geometry")
			<< new ConsumeNode(gjToken::COLON)
			<< new ConsumeNode(gjToken::LBRACE)
			<< new ForwardNode(gjRule::GProperties)
			<< new ConsumeNode(gjToken::RBRACE));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING, "properties")
			<< new ConsumeNode(gjToken::COLON)
			<< new ConsumeNode(gjToken::LBRACE)
			<< new ForwardNode(gjRule::GenericProperties)
			<< new ConsumeNode(gjToken::RBRACE));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(gjRule::GenericProperty));
		rules->push_back(rule);

		rule = new Rule(gjRule::GProperties);
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(gjRule::GProperty)
			<< new ForwardNode(gjRule::GPropertiesTail));
		rules->push_back(rule);

		rule = new Rule(gjRule::GPropertiesTail);
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::COMMA)
			<< new ForwardNode(gjRule::GProperty)
			<< new ForwardNode(gjRule::GPropertiesTail));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(gjRule::GProperty);
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING, "type")
			<< new ConsumeNode(gjToken::COLON)
			<< new ForwardNode(gjRule::GeometryType));
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::STRING, "coordinates")
			<< new ConsumeNode(gjToken::COLON)
			<< new ConsumeNode(gjToken::LBRACKET)
			<< new ForwardNode(gjRule::CoordinatesRoot)
			<< new ConsumeNode(gjToken::RBRACKET));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(gjRule::GenericProperty));
		rules->push_back(rule);

		rule = new Rule(gjRule::CoordinatesRoot);
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::LBRACKET)
			<< new ForwardNode(gjRule::CoordinatesRoot)
			<< new ConsumeNode(gjToken::RBRACKET));
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(gjRule::Coordinates));
		rules->push_back(rule);

		rule = new Rule(gjRule::Coordinates);
		(*rule) += &((*(new Sequence()))
			<< new ExtractNode()
			<< new ForwardNode(gjRule::CoordinatesTail));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(gjRule::CoordinatesTail);
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::COMMA)
			<< new ExtractNode()
			<< new ForwardNode(gjRule::CoordinatesTail));
		(*rule) += new Sequence();
		rules->push_back(rule);

		rule = new Rule(gjRule::Value);
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

		rule = new Rule(gjRule::GenericArrayValues);
		(*rule) += &((*(new Sequence()))
			<< new ForwardNode(gjRule::Value)
			<< new ForwardNode(gjRule::GenericArrayValuesTail));
		rules->push_back(rule);

		rule = new Rule(gjRule::GenericArrayValuesTail);
		(*rule) += &((*(new Sequence()))
			<< new ConsumeNode(gjToken::COMMA)
			<< new ForwardNode(gjRule::Value)
			<< new ForwardNode(gjRule::GenericArrayValuesTail));
		(*rule) += new Sequence();
		rules->push_back(rule);


		Parser* p = new Parser(rules, rule, visitor, l);

		return p;
	}
}