#pragma once
#include "pRule.h"
#include "node/pParserNode.h"
#include "pParserVisitor.h"

namespace nspParser {

	template<class enum_t, class enum_r>	
	class pParser {
		using Rule = nspParser::pRule<enum_t, enum_r>;
		using Visitor = pParserVisitor<enum_t, enum_r>;
		template<class T, class U, class V = unsigned char> 
		using Map = nspMap::pMap<T, U, V>;
		using Lexer = nspLexer::pLexer<enum_t>;

		Array<Rule*>* _rules = nullptr;	
		Map<enum_r, Rule*, unsigned char>* _rule_map = new Map<enum_r, Rule*, unsigned char>();	
		Rule* _entry_rule = nullptr;
		Visitor* _visitor = nullptr;
		Lexer* _lexer = nullptr;

	public:
		pParser() = delete;

		// takes ownership of all passed resources including each rule in rules array
		pParser(Array<Rule*>* rules, Rule* entry_rule, Visitor* visitor, Lexer* lexer) : _rules(rules), _entry_rule(entry_rule), _visitor(visitor), _lexer(lexer) {
			for (Rule* rule : *rules) {
				(*_rule_map)[rule->get_id()] = rule;
			}
		}

		~pParser() {
			_entry_rule = nullptr;
			auto rules_size = _rules->size();
			for (size_t i = 0; i < rules_size; i++) {
				delete (*_rules)[i];
				(*_rules)[i] = nullptr;
			}
			delete _rule_map;
			_rule_map = nullptr;
			delete _rules;
			_rules = nullptr;
			delete _visitor;
			_visitor = nullptr;
			delete _lexer;
			_lexer = nullptr;
		}

		void open(String path) {
			_lexer->open(path);
		}

		void parse() {
			auto* top_level_node = _entry_rule->get_rhs()[0]->get_nodes()[0];
			if (top_level_node->get_type() == 'n') {
				auto* entry_node = static_cast<pEntryNode<enum_t, enum_r>*>(top_level_node);
				entry_node->accept(_visitor);
			}
			else {
				printf("Entry rule should have pEntryNode pointing to first Rule!\nGot type: %c\n", top_level_node->get_type());
			}
		}
	};

}