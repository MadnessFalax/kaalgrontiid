#include "pPatternParser.h"

namespace nspRegex {


	void pPatternParser::_set_context(char cur, char lookahead) {
		_cur = cur;
		_lookahead = lookahead;
	}

	pPatternParser::RegexNode* pPatternParser::_consume(RegexNode* prec, RegexNode* parent) {
		if (_cur == '\\') {
			return _consume_escaped(prec, parent);
		}
		else if (_cur == '{') {
			return _consume_quantifier(prec, parent);
		}
		else if (_cur == '*') {
			return _consume_star(prec, parent);
		}
		else if (_cur == '+') {
			return _consume_plus(prec, parent);
		}
		else if (_cur == '?') {
			return _consume_question(prec, parent);
		}
		else if (_cur == '^') {
			return _consume_start(prec, parent);
		}
		else if (_cur == '$') {
			return _consume_end(prec, parent);
		}
		else if (_cur == '|') {
			return _consume_alternation(prec, parent);
		}
		else if (_cur == '.') {
			return _consume_wildcard(prec, parent);
		}
		else if (_cur == '(') {
			return _consume_concat_begin(prec, parent);
		}
		else if (_cur == '[') {
			return _consume_class_start(prec, parent);
		}
		else {
			return _consume_qualifier(prec, parent);
		}
	}

	pPatternParser::RegexNode* pPatternParser::_consume_alternation(RegexNode* preceding, RegexNode* parent)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_escaped(RegexNode* preceding, RegexNode* parent)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_quantifier(RegexNode* preceding, RegexNode* parent)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_qualifier(RegexNode* preceding, RegexNode* parent)
	{
		if (_cur < _qualifier_min || _cur > _qualifier_max) {
			throw pBadPatternException();
		}

		auto* arr = new nspArray::pArray<unsigned char>();
		arr->push_back(_cur);

		auto new_node = new QualNode(arr);
		new_node->context->set_left(preceding);
		if (!(_lookahead == '{' || _lookahead == '*' || _lookahead == '+' || _lookahead == '?')) {
			new_node->context->set_parent(parent);
		}

		return new_node;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_start(RegexNode* preceding, RegexNode* parent)
	{
		if (preceding->get_type() != 'c') {
			throw pBadPatternException();
		}
		auto new_node = new StartNode();
		new_node->context->set_left(preceding);
		new_node->context->set_parent(preceding);

		return new_node;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_end(RegexNode* preceding, RegexNode* parent)
	{
		auto new_node = new EndNode();
		new_node->context->set_left(preceding);
		new_node->context->set_parent(parent);
		return new_node;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_wildcard(RegexNode* preceding, RegexNode* parent)
	{
		auto prec_type = preceding->get_type();
		if (prec_type == 'e') {
			throw pBadPatternException();
		}
		auto* arr = new nspArray::pArray<unsigned char>();
		for (unsigned char i = pPatternParser::_qualifier_min; i <= pPatternParser::_qualifier_max; i++) {
			arr->push_back(i);
		}
		auto new_node = new QualNode(arr);
		new_node->context->set_left(preceding);
		if (!(_lookahead == '{' || _lookahead == '*' || _lookahead == '+' || _lookahead == '?')) {
			new_node->context->set_parent(parent);
		}

		return new_node;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_star(RegexNode* preceding, RegexNode* parent)
	{
		auto prec_type = preceding->get_type();

		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_plus(RegexNode* preceding, RegexNode* parent)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_question(RegexNode* preceding, RegexNode* parent)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_range(RegexNode* preceding, RegexNode* parent)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_concat_begin(RegexNode* preceding, RegexNode* parent)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_concat_end(RegexNode* preceding, RegexNode* parent)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_class_start(RegexNode* preceding, RegexNode* parent)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_class_end(RegexNode* preceding, RegexNode* parent)
	{
		return nullptr;
	}



}