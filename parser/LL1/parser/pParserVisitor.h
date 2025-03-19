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
#include "pParserStack.h"
#include <cstdlib>

#include "cPolygon.h"
#include "cLineString.h"
#include "cSphere.h"

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
		using Stack = pParserStack<enum_t, enum_r, enum_c>;
		template<class T, class U, class V = unsigned char>
		using Map = nspMap::pMap<T, U, V>;

	public:
		struct Context {
			enum LastStatus {
				OK,
				FAIL
			} last_status = LastStatus::FAIL;
			
			enum_r current_rule = enum_r{};
			enum_t current_token = enum_t{};
			TokenInstance* current_instance = nullptr;
			TokenInstance* current_lookahead = nullptr;
			typename ExtractNode::ExtractType last_extract_type = ExtractNode::ExtractType::STRING;
			String last_extracted_string = String();
			double last_extracted_number = 0.0;
			bool end = false;
			
			bool has_item = false;
			DataShape item_type = DataShape{};
			cDataShape<cNTuple>* item = nullptr;
			size_t dimension = 0;
			cSpaceDescriptor* last_item_sd = nullptr;


			Context() = default;

			~Context() {
				last_item_sd = nullptr;
				if (item) {
					delete item;
					item = nullptr;
				}
				delete current_instance;
				current_instance = nullptr;
				delete current_lookahead;
				current_lookahead = nullptr;
			}
		};

	protected:
		Context _context;
		Lexer* _lexer = nullptr;
		// Array of rules, rules are not owned by this class
		Map<enum_r, Rule*, unsigned char> _rule_map = Map<enum_r, Rule*, unsigned char>();
		Stack* _stack = nullptr;									// Stack is owned by pParser class
		cSpaceDescriptor* _space_desc_2d = nullptr;
		cSpaceDescriptor* _space_desc_3d = nullptr;


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
		// Does not take ownership of lexer, rules or stack
		pParserVisitor(Lexer* lexer, Array<Rule*>* rules, Stack* stack) : _lexer(lexer), _stack(stack) {
			for (Rule* rule : *rules) {
				_rule_map[rule->get_id()] = rule;
			}

			_space_desc_2d = new cSpaceDescriptor(DIMENSION_2, new cNTuple(), new cDouble());
			_space_desc_3d = new cSpaceDescriptor(DIMENSION_3, new cNTuple(), new cDouble());
		}

		virtual ~pParserVisitor() {
			// no ownership of lexer or rules
			_lexer = nullptr;
			_stack = nullptr;
			delete _space_desc_2d;
			_space_desc_2d = nullptr;
			delete _space_desc_3d;
			_space_desc_3d = nullptr;
		}

		Context* get_context() {
			return &_context;
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
			_stack->register_rule(_rule_map[node.get_entry_rule_id()]);
			_context.current_rule = node.get_entry_rule_id();
			_context.last_status = Context::LastStatus::OK;
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
			_stack->register_rule(_rule_map[node.get_rule_id()]);
			_context.current_rule = node.get_entry_rule_id();
			_context.last_status = Context::LastStatus::OK;
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

		bool get_status() {
			return _context.last_status == Context::LastStatus::OK;
		}

		bool has_item() {
			return _context.has_item;
		}

		cDataShape<cNTuple>* get_item() {
			auto* retval = _context.item;
			_context.item = nullptr;
			_context.has_item = false;
			_context.dimension = 0;
			_context.item_type = DataShape{};
			return retval;
		}

		virtual void custom_visit_root(CustomNode& node) = 0;
	};
}