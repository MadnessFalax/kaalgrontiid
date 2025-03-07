#pragma once
#include "pRule.h"
#include "node/pParserNode.h"

namespace nspParser {
	using Rule = nspParser::pRule;
	
	class pParser {
		Array<Rule*> _rules = Array<Rule*>();	
		Rule* _entry_rule = nullptr;

	public:
		pParser() {

		}

		pParser(Array<Rule*> rules, Rule* entry_rule) : _rules(rules), _entry_rule(entry_rule) {

		}

		void add_rule(Rule* rule) {
			_rules.push_back(rule);
		}

		void set_entry_rule(Rule* rule) {
			_entry_rule = rule;
		}

		pParserNode* parse() {

		}
	};

}