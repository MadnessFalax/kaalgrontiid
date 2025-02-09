#include "pPatternParser.h"

namespace nspRegex {


	void pPatternParser::_set_context(char cur, char lookahead) {
		_cur = cur;
		_lookahead = lookahead;
	}

	pPatternParser::RegexNode* pPatternParser::_consume(RegexNode* prec) {
		if (_cur == '\\') {
			return _consume_escaped(prec);
		}
		else if (_cur == '{') {
			return _consume_quantifier(prec);
		}
		else if (_cur == '*') {
			return _consume_star(prec);
		}
		else if (_cur == '+') {
			return _consume_plus(prec);
		}
		else if (_cur == '?') {
			return _consume_question(prec);
		}
		else if (_cur == '^') {
			return _consume_start(prec);
		}
		else if (_cur == '$') {
			return _consume_end(prec);
		}
		else if (_cur == '|') {
			return _consume_alternation(prec);
		}
		else if (_cur == '.') {
			return _consume_wildcard(prec);
		}
		else if (_cur == '(') {
			return _consume_concat_begin(prec);
		}
		else if (_cur == '[') {
			return _consume_class_start(prec);
		}
		else {
			return _consume_qualifier(prec);
		}
	}

	pPatternParser::RegexNode* pPatternParser::_consume_alternation(RegexNode* preceding)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_escaped(RegexNode* preceding)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_quantifier(RegexNode* preceding)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_qualifier(RegexNode* preceding)
	{
		if (_cur < _qualifier_min || _cur > _qualifier_max) {
			throw pBadPatternException();
		}

		auto* arr = new nspArray::pArray<unsigned char>();
		arr->push_back(_cur);

		auto new_node = new QualNode(arr);
		new_node->context->set_left(preceding);

		return new_node;

	}

	pPatternParser::RegexNode* pPatternParser::_consume_start(RegexNode* preceding)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_end(RegexNode* preceding)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_wildcard(RegexNode* preceding)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_star(RegexNode* preceding)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_plus(RegexNode* preceding)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_question(RegexNode* preceding)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_range(RegexNode* preceding)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_concat_begin(RegexNode* preceding)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_concat_end(RegexNode* preceding)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_class_start(RegexNode* preceding)
	{
		return nullptr;
	}

	pPatternParser::RegexNode* pPatternParser::_consume_class_end(RegexNode* preceding)
	{
		return nullptr;
	}



}