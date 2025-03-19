#pragma once
#include "pRule.h"
#include "node/pParserNode.h"
#include "pParserVisitor.h"

namespace nspParser {

	template<class enum_t, class enum_r, class enum_c, class custom_visitor_t>	
	class pParser {
		using Rule = nspParser::pRule<enum_t, enum_r>;
		using Visitor = pParserVisitor<enum_t, enum_r, enum_c, custom_visitor_t>;
		template<class T, class U, class V = unsigned char> 
		using Map = nspMap::pMap<T, U, V>;
		using Lexer = nspLexer::pLexer<enum_t>;
		using Stack = nspParser::pParserStack<enum_t, enum_r, enum_c>;
		using EntryNode = nspParser::pEntryNode<enum_t, enum_r>;
		using ConsumeNode = nspParser::pConsumeNode<enum_t, enum_r>;
		using CustomNode = nspParser::pCustomNode<enum_t, enum_r, enum_c>;
		using ExtractNode = nspParser::pExtractNode<enum_t, enum_r>;
		using ForwardNode = nspParser::pForwardNode<enum_t, enum_r>;
		using ParserNode = nspParser::pParserNode<enum_t, enum_r>;
		using Context = typename Visitor::Context;

		Array<Rule*>* _rules = nullptr;	
		Map<enum_r, Rule*, unsigned char>* _rule_map = new Map<enum_r, Rule*, unsigned char>();	
		Rule* _entry_rule = nullptr;
		Visitor* _visitor = nullptr;
		Lexer* _lexer = nullptr;
		Stack* _stack = nullptr;
		Context* _context = nullptr;

		bool _resolve_current_node() {
			auto* node = _stack->get_node();
			if (node) {
				EntryNode* entry_node = nullptr;
				ConsumeNode* consume_node = nullptr;
				CustomNode* custom_node = nullptr;
				ExtractNode* extract_node = nullptr;
				ForwardNode* forward_node = nullptr;

				switch (node->get_type()) {
				case 'n':
					entry_node = static_cast<EntryNode*>(node);
					entry_node->accept(*_visitor);
					break;
				case 'c':
					consume_node = static_cast<ConsumeNode*>(node);
					consume_node->accept(*_visitor);
					break;
				case 'e':
					extract_node = static_cast<ExtractNode*>(node);
					extract_node->accept(*_visitor);
					break;
				case 'f':
					forward_node = static_cast<ForwardNode*>(node);
					forward_node->accept(*_visitor);
					break;
				case 'u':
					custom_node = static_cast<CustomNode*>(node);
					custom_node->accept(*_visitor);
					break;
				default:
					printf("Unknown node type: %c\n", node->get_type());
					return false;
				}

				if (_visitor->get_status()) {
					return _stack->success();
				}
				else {
					return _stack->fail();
				}
			}
			return false;
		}

	public:
		pParser() = delete;

		// takes ownership of all passed resources including each rule in rules array
		pParser(Array<Rule*>* rules, Rule* entry_rule, Visitor* visitor, Lexer* lexer, Stack* stack) : _rules(rules), _entry_rule(entry_rule), _visitor(visitor), _lexer(lexer), _stack(stack) {
			for (Rule* rule : *rules) {
				(*_rule_map)[rule->get_id()] = rule;
			}
			_context = _visitor->get_context();
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
			delete _stack;
			_stack = nullptr;
			delete _lexer;
			_lexer = nullptr;
		}

		void open(String path) {
			_lexer->open(path);
		}

		//void parse() {
		//	auto* top_level_node = _entry_rule->get_rhs()[0]->get_nodes()[0];
		//	if (top_level_node->get_type() == 'n') {
		//		auto* entry_node = static_cast<pEntryNode<enum_t, enum_r>*>(top_level_node);
		//		entry_node->accept(_visitor);
		//	}
		//	else {
		//		printf("Entry rule should have pEntryNode pointing to first Rule!\nGot type: %c\n", top_level_node->get_type());
		//	}
		//}

		DataShape get_shape_type() {
			return _context->item_type;
		}

		cSpaceDescriptor* get_space_descriptor() {
			return _context->last_item_sd;
		}

		cDataType* get_item() {
			while (!_context->end && !_context->has_item) {
				if (!_resolve_current_node()) {
					break;
				}
			}
			auto* item = _context->item;
			_context->item = nullptr;
			_context->has_item = false;
			return item;
		}
	};

}