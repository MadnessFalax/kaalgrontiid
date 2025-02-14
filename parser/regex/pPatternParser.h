#pragma once
#include "../../container/pString.h"
#include <type_traits>
#include "AST/pRegexNode.h"
#include "AST/node/pQuantifierNode.h"
#include "AST/node/pAlternationNode.h"
#include "AST/node/pQualifierNode.h"
#include "AST/node/pConcatNode.h"
#include "pBadPatternException.h"

namespace nspRegex {

	/*
	* Handles pattern parsing and validation
	* check this shit out https://matt.might.net/articles/parsing-regex-with-recursive-descent/#:~:text=In%20order%20to%20parse%2C%20we%20need%20a%20context-free,%3Cchar%3E%20%7C%20%27%27%20%3Cchar%3E%20%7C%20%27%28%27%20%3Cregex%3E%20%27%29%27
	*/
	class pPatternParser {
		using AltNode = nspRegexAST::pAlternationNode;
		using ConcNode = nspRegexAST::pConcatNode;
		using QualNode = nspRegexAST::pQualifierNode;
		using QuantNode = nspRegexAST::pQuantifierNode;
		using RegexNode = nspRegexAST::pRegexNode;
		using String = nspString::pString;

		String* _pattern = nullptr;

		char _cur = '\0';
		char _lookahead = '\0';
		size_t _pattern_length = 0;
		size_t _current_index = 0;
		char _qualifier_min = 32;
		char _qualifier_max = 126;


		ConcNode* _root_node = nullptr;
		RegexNode* _last_node = nullptr;

		void _set_context(char cur, char lookahead);

		RegexNode* _consume(RegexNode* preceding, RegexNode* parent);

		RegexNode* _consume_alternation(RegexNode* preceding, RegexNode* parent);

		RegexNode* _consume_escaped(RegexNode* preceding, RegexNode* parent);

		RegexNode* _consume_quantifier(RegexNode* preceding, RegexNode* parent);

		RegexNode* _consume_qualifier(RegexNode* preceding, RegexNode* parent);

		RegexNode* _consume_wildcard(RegexNode* preceding, RegexNode* parent);

		RegexNode* _consume_star(RegexNode* preceding, RegexNode* parent);

		RegexNode* _consume_plus(RegexNode* preceding, RegexNode* parent);

		RegexNode* _consume_question(RegexNode* preceding, RegexNode* parent);

		RegexNode* _consume_range(RegexNode* preceding, RegexNode* parent);

		RegexNode* _consume_concat_begin(RegexNode* preceding, RegexNode* parent);

		RegexNode* _consume_concat_end(RegexNode* preceding, RegexNode* parent);

		RegexNode* _consume_class_start(RegexNode* preceding, RegexNode* parent);

		RegexNode* _consume_class_end(RegexNode* preceding, RegexNode* parent);

	public:
		pPatternParser() {}

		pPatternParser(String& pattern) : _pattern(new String(pattern)) {
			_pattern_length = _pattern->length();
			_root_node = new ConcNode();
			_last_node = _root_node;
		}
		pPatternParser(const char* pattern) : _pattern(new String(pattern)) {
			_pattern_length = _pattern->length();
			_root_node = new ConcNode();
			_last_node = _root_node;
		}
		~pPatternParser() {
			delete _pattern;
			_pattern = nullptr;
		}

		void parse() {
			for (; _current_index < _pattern_length; _current_index++) {
				_set_context((*_pattern)[_current_index], (*_pattern)[_current_index + 1]);
				_last_node = _consume(_last_node, _root_node);
			}
		}

	};


}