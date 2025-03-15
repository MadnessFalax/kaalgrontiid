#pragma once
#include "../../../abstract/pBaseVisitor.h"
#include "../lexer/pTokenInstance.h"
#include "../lexer/pLexer.h"
#include "pRule.h"
#include "node/pParserNode.h"
#include "node/pConsumeNode.h"
#include "node/pExtractNode.h"
#include "node/pForwardNode.h"
#include "node/pCustomNode.h"
#include "node/pEntryNode.h"
#include "pErrorReporter.h"
#include <cstdlib>

namespace nspParser {
	template<typename enum_t, typename enum_r, typename enum_c, typename derived>
	class pParserVisitor : public pBaseVisitor<pParserVisitor<enum_t, enum_r, enum_c, derived>> {
	protected:
		using TokenInstance = nspLexer::pTokenInstance<enum_t, nspLexer::PrototypeKind::REGEX>;
		using Lexer = nspLexer::pLexer<enum_t>;
		using TokenStack = nspArray::pArray<TokenInstance*>;
		using ExtractNode = pExtractNode<enum_t, enum_r>;
		using ConsumeNode = pConsumeNode<enum_t, enum_r>;
		using ForwardNode = pForwardNode<enum_t, enum_r>;
		using CustomNode = pCustomNode<enum_t, enum_r, enum_c>;
		using ParserNode = pParserNode<enum_t, enum_r>;
		using EntryNode = pEntryNode<enum_t, enum_r>;
		using Rule = pRule<enum_t, enum_r>;
		template<class T, class U, class V = unsigned char>
		using Map = nspMap::pMap<T, U, V>;

		struct Context {
			enum_r current_rule = enum_r{};
			enum_t current_token = enum_t{};
			TokenInstance* current_instance = nullptr;
			TokenInstance* current_lookahead = nullptr;
			typename ExtractNode::ExtractType last_extract_type = ExtractNode::ExtractType::STRING;
			String last_extracted_string = String();
			double last_extracted_number = 0.0;
			enum LastStatus {
				OK,
				FAIL
			} last_status = LastStatus::FAIL;
			bool end = false;

			Context() = default;

			~Context() {
				delete current_instance;
				current_instance = nullptr;
				delete current_lookahead;
				current_lookahead = nullptr;
			}
		};

		Context _context;
		Lexer* _lexer = nullptr;
		// Array of rules, rules are not owned by this class
		Map<enum_r, Rule*, unsigned char> _rule_map = Map<enum_r, Rule*, unsigned char>();
		

		bool _try_extract_number() {
			char* end = nullptr;
			auto* value = reinterpret_cast<const char*>(_context.current_instance->get_value().c_str());
			auto ret_val = strtod(value, &end);
			if (ret_val == 0.0 && end == value) {
				return false;
			}
			else {
				_context.last_extract_type = ExtractNode::ExtractType::NUMBER;
				_context.last_extracted_number = ret_val;
				return true;
			}
		}

		bool _check_end() {
			if (_context.end) {
				printf("Got ERROR or END token. Exiting parser...");
				return true;
			}
			return false;
		}

		bool _consume() {
			delete _context.current_instance;
			if (_context.current_lookahead) {
				_context.current_instance = _context.current_lookahead;
				_context.current_lookahead = nullptr;
				if (_context.current_instance->get_prototype() == nullptr) {
					_context.current_token = enum_t{};
					_context.end = true;
					return false;
				}
				else {
					_context.current_token = _context.current_instance->get_prototype()->get_id();
				}
			}
			else {
				_context.current_instance = _lexer->get_token();
				if (_context.current_instance->get_prototype() == nullptr) {
					_context.current_token = enum_t{};
					_context.end = true;
					return false;
				}
				else {
					_context.current_token = _context.current_instance->get_prototype()->get_id();
				}
			}
			return true;
		}

		bool _consume(enum_t token_type) {
			if (_context.current_instance->get_prototype()->get_id() == token_type) {
				return _consume();
			}
			return false;
		}

		bool _consume(enum_t token_type, const String& value) {
			if (_context.current_instance->get_prototype()->get_id() == token_type && _context.current_instance->get_value() == value) {
				return _consume();
			}
			return false;
		}

		// how many tokens to look ahead, 0 returns current token
		TokenInstance* _lookahead() {
			if (_context.current_lookahead) {
				return _context.current_lookahead;
			}
			else {
				_context.current_lookahead = _lexer->get_token();
				return _context.current_lookahead;
			}
		}

		TokenInstance* _current() {
			return _context.current_instance;
		}

	public:
		// Does not take ownership of lexer or rules
		pParserVisitor(Lexer* lexer, Array<Rule*>* rules) : _lexer(lexer) {
			for (Rule* rule : *rules) {
				_rule_map[rule->get_id()] = rule;
			}

			rules = nullptr;

		}

		virtual ~pParserVisitor() {
			_lexer = nullptr;
		}

		Context& get_context() {
			return _context;
		}

		void resolve_visit(ParserNode& node) {
			printf("You have implemented this visitor wrong, you moron!");
		}

		void resolve_visit(EntryNode& node) {
			// load first token into _context
			_consume();
			if (_check_end()) {
				return;
			}
			
			// now behave like ForwardNode
			auto my_rule = node.get_this_node_rule_id();
			auto* rule = _rule_map[node.get_entry_rule_id()];
			auto& rhs = rule->get_rhs();
			for (auto* sequence : rhs) {
				auto& nodes = sequence->get_nodes();
				size_t node_index = 0;
				for (auto* sub_node : nodes) {
					// determine node type
					ConsumeNode* consume_node = nullptr;
					CustomNode* custom_node = nullptr;
					ExtractNode* extract_node = nullptr;
					ForwardNode* forward_node = nullptr;
					if (sub_node->get_type() == 'c') {
						consume_node = dynamic_cast<ConsumeNode*>(sub_node);
					}
					else if (sub_node->get_type() == 'e') {
						extract_node = dynamic_cast<ExtractNode*>(sub_node);
					}
					else if (sub_node->get_type() == 'f') {
						forward_node = dynamic_cast<ForwardNode*>(sub_node);
					}
					else if (sub_node->get_type() == 'u') {
						custom_node = dynamic_cast<CustomNode*>(sub_node);
					}
					else {
						pErrorReporter::report_rule(_rule_map[_context.current_rule]->get_name(), _context.current_instance->get_prototype()->get_name(), "Error when determining which node to forward to.");
						_context.last_status = Context::LastStatus::FAIL;
						return;
					}

					// visit correct node type
					if (consume_node) {
						_context.current_rule = rule->get_id();
						consume_node->accept(*this);
						_context.current_rule = my_rule;
					}
					else if (extract_node) {
						_context.current_rule = rule->get_id();
						extract_node->accept(*this);
						_context.current_rule = my_rule;
					}
					else if (forward_node) {
						_context.current_rule = rule->get_id();
						forward_node->accept(*this);
						_context.current_rule = my_rule;
					}
					else if (custom_node) {
						_context.current_rule = rule->get_id();
						custom_node->accept(*this);
						_context.current_rule = my_rule;
					}
					else {
						pErrorReporter::report_rule(_rule_map[_context.current_rule]->get_name(), _context.current_instance->get_prototype()->get_name(), "Cannot forward to selected node. Node was probably determined as node base pParserNode.");
						_context.last_status = Context::LastStatus::FAIL;
						return;
					}

					if (_context.end) {
						_context.last_status = Context::LastStatus::OK;
						return;
					}

					if (node_index == 0 && _context.last_status == Context::LastStatus::FAIL) {
						// try another sequence
						break;
					}
					else if (node_index > 0 && _context.last_status == Context::LastStatus::FAIL) {
						pErrorReporter::report_rule(_rule_map[_context.current_rule]->get_name(), _context.current_instance->get_prototype()->get_name(), "Either wrong sequence was determined, or there was unexpected token after the first sequence node.");
						return;
					}
					node_index++;
				}
			}

			if (_context.last_status == Context::LastStatus::FAIL) {
				pErrorReporter::report_rule(_rule_map[_context.current_rule]->get_name(), _context.current_instance->get_prototype()->get_name(), "Failed to parse input.");
			}
			return;
		}

		void resolve_visit(ConsumeNode& node) {
			if (_context.end) {
				_context.last_status = Context::LastStatus::OK;
				return;
			}

			if (node.consume_any()) {
				_consume();
				_context.last_status = Context::LastStatus::OK;
				return;
			}

			if (node.check_value()) {
				if (_consume(node.get_proto(), node.get_value())) {
					_context.last_status = Context::LastStatus::OK;
					return;
				}
			}
			else {
				if (_consume(node.get_proto())) {
					_context.last_status = Context::LastStatus::OK;
					return;
				}
			}
			_context.last_status = Context::LastStatus::FAIL;
			return;
		}

		void resolve_visit(ForwardNode& node) {
			if (_context.end) {
				_context.last_status = Context::LastStatus::OK;
				return;
			}
			auto my_rule = _context.current_rule;
			auto* rule = _rule_map[node.get_rule_id()];
			auto& rhs = rule->get_rhs();
			bool has_epsilon_sequence = false;
			for (auto* sequence : rhs) {
				auto& nodes = sequence->get_nodes();
				if (sequence->is_empty()) {
					has_epsilon_sequence = true;
					continue;
				}
				size_t node_index = 0;
				for (auto* sub_node : nodes) {
					// determine node type
					ConsumeNode* consume_node = nullptr;
					CustomNode* custom_node = nullptr;
					ExtractNode* extract_node = nullptr;
					ForwardNode* forward_node = nullptr;
					if (sub_node->get_type() == 'c') {
						consume_node = dynamic_cast<ConsumeNode*>(sub_node);
					}
					else if (sub_node->get_type() == 'e') {
						extract_node = dynamic_cast<ExtractNode*>(sub_node);
					}
					else if (sub_node->get_type() == 'f') {
						forward_node = dynamic_cast<ForwardNode*>(sub_node);
					}
					else if (sub_node->get_type() == 'u') {
						custom_node = dynamic_cast<CustomNode*>(sub_node);
					}
					else {
						pErrorReporter::report_rule(_rule_map[_context.current_rule]->get_name(), _context.current_instance->get_prototype()->get_name(), "Error when determining which node to forward to.");
						_context.last_status = Context::LastStatus::FAIL;
						return;
					}

					// visit correct node type
					if (consume_node) {
						_context.current_rule = rule->get_id();
						consume_node->accept(*this);
						_context.current_rule = my_rule;
					}
					else if (extract_node) {
						_context.current_rule = rule->get_id();
						extract_node->accept(*this);
						_context.current_rule = my_rule;
					}
					else if (forward_node) {
						_context.current_rule = rule->get_id();
						forward_node->accept(*this);
						_context.current_rule = my_rule;
					}
					else if (custom_node) {
						_context.current_rule = rule->get_id();
						custom_node->accept(*this);
						_context.current_rule = my_rule;
					}
					else {
						pErrorReporter::report_rule(_rule_map[_context.current_rule]->get_name(), _context.current_instance->get_prototype()->get_name(), "Cannot forward to selected node. Node was probably determined as node base pParserNode.");
						_context.last_status = Context::LastStatus::FAIL;
						return;
					}

					if (node_index == 0 && _context.last_status == Context::LastStatus::FAIL) {
						// try another sequence
						break;
					}
					if (_context.end) {
						break;
					}
					else if (node_index > 0 && _context.last_status == Context::LastStatus::FAIL) {
						pErrorReporter::report_rule(_rule_map[_context.current_rule]->get_name(), _context.current_instance->get_prototype()->get_name(), "Either wrong sequence was determined, or there was unexpected token after the first sequence node.");
						return;
					}
					node_index++;
				}
				if (_context.last_status == Context::LastStatus::OK) {
					break;
				}
			}
			if (has_epsilon_sequence) {
				_context.last_status = Context::LastStatus::OK;
			}
			return;
		}

		void resolve_visit(ExtractNode& node) {
			if (_context.end) {
				_context.last_status = Context::LastStatus::OK;
				return;
			}
			if (node.is_restricted_type()) {
				if (_context.current_instance->get_prototype()->get_id() != node.get_token_type()) {
#ifdef _DEBUG
					pErrorReporter::report_rule(_rule_map[_context.current_rule]->get_name(), _context.current_instance->get_prototype()->get_name());
					pErrorReporter::report_token(_context.current_instance->get_position(), _context.current_instance->get_value(), _context.current_instance->get_prototype()->get_name(), "Got unexpected token.\n");
#endif
					_context.last_status = Context::LastStatus::FAIL;
					return;
				}
			}
			if (node.is_restricted_pattern()) {
				if (!node.get_pattern()->match(_context.current_instance->get_value())) {
#ifdef _DEBUG
					pErrorReporter::report_rule(_rule_map[_context.current_rule]->get_name(), _context.current_instance->get_prototype()->get_name());
					pErrorReporter::report_token(_context.current_instance->get_position(), _context.current_instance->get_value(), _context.current_instance->get_prototype()->get_name(), "Got unexpected value.\n");
#endif
					_context.last_status = Context::LastStatus::FAIL;
					return;
				}
			}
			switch (node.get_extract_type()) {
			case ExtractNode::ExtractType::NUMBER:
				if (_try_extract_number()) {
					_context.last_status = Context::LastStatus::OK;
#ifdef _DEBUG
					printf("Extracted number: %f\n", _context.last_extracted_number);
#endif
					_consume();
				}
				else {
					pErrorReporter::report_rule(_rule_map[_context.current_rule]->get_name(), _context.current_instance->get_prototype()->get_name());
					pErrorReporter::report_token(_context.current_instance->get_position(), _context.current_instance->get_value(), _context.current_instance->get_prototype()->get_name(), "Failed to extract number.\n");
					_context.last_status = Context::LastStatus::FAIL;
				}
				return;
			case ExtractNode::ExtractType::STRING:
				_context.last_extract_type = ExtractNode::ExtractType::STRING;
				_context.last_extracted_string = _context.current_instance->get_value();
				_context.last_status = Context::LastStatus::OK;
#ifdef _DEBUG
				printf("Extracted string: %s\n", reinterpret_cast<const char*>(_context.last_extracted_string.c_str()));
#endif
				_consume();
				return;
			}
		}

		void resolve_visit(CustomNode& node) {
			static_cast<derived*>(this)->custom_visit_root(node);
		}

		virtual void custom_visit_root(CustomNode& node) = 0;
	};
}