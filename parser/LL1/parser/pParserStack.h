#pragma once
#include "../lexer/pLexer.h"
#include "node/pParserNode.h"
#include "node/pEntryNode.h"
#include "node/pForwardNode.h"
#include "node/pConsumeNode.h"
#include "node/pCustomNode.h"
#include "node/pExtractNode.h"
#include "pRule.h"
#include "pSequence.h"
#include "pErrorReporter.h"

namespace nspParser {
	template<typename enum_t, typename enum_r, typename enum_h>
	class pParserStack {
		using TokenInstance = nspLexer::pTokenInstance<enum_t, nspLexer::PrototypeKind::REGEX>;
		using TokenStack = nspArray::pArray<TokenInstance*>;
		using ExtractNode = pExtractNode<enum_t, enum_r>;
		using ConsumeNode = pConsumeNode<enum_t, enum_r>;
		using ForwardNode = pForwardNode<enum_t, enum_r>;
		using CustomNode = pCustomNode<enum_t, enum_r, enum_h>;
		using ParserNode = pParserNode<enum_t, enum_r>;
		using EntryNode = pEntryNode<enum_t, enum_r>;
		using Rule = pRule<enum_t, enum_r>;
		template<class T, class U, class V = unsigned char>
		using Map = nspMap::pMap<T, U, V>;

		// Array of rules, rules are not owned by this class

	public:

		struct StackBlock {
			Array<pSequence<enum_t, enum_r>*>* qualified_sequences = nullptr;	// sequences are owned by rules in parser definition 
			size_t selected_sequence = 0;
			size_t selected_node = 0;
			size_t node_count = 0;
			size_t sequence_count = 0;
			bool sequence_locked = false;
			bool has_epsilon_sequence = false;

			StackBlock(Rule* rule) {
				qualified_sequences = &(rule->get_rhs());
				sequence_count = qualified_sequences->size();
				has_epsilon_sequence = rule->has_epsilon();
				node_count = (*qualified_sequences)[selected_sequence]->get_nodes().size();
			}

			~StackBlock() {
				qualified_sequences = nullptr;		// sequences are owned by rules in parser definition 
			}

			// returns true if new index points to a valid node
			bool next_node() {
				sequence_locked = true;
				selected_node++;
				return selected_node < node_count;
			}

			// returns true if new index points to a valid sequence
			bool next_sequence() {
				if (sequence_locked) {
					return false;
				}
				selected_sequence++;
				if (selected_sequence >= sequence_count) {
					return false;
				}
				if ((*qualified_sequences)[selected_sequence]->is_empty()) {
					selected_sequence++;
				}
				if (selected_sequence >= sequence_count) {
					return false;
				}
				node_count = (*qualified_sequences)[selected_sequence]->get_nodes().size();
				return selected_sequence < sequence_count && selected_node < node_count;
			}

			ParserNode* get_node() {
				ParserNode* retval = nullptr;
				if (selected_sequence < sequence_count && selected_node < node_count) {
					retval = (*qualified_sequences)[selected_sequence]->get_nodes()[selected_node];
				}

				return retval;
			}
		};
		
	private:
		Array<StackBlock*> _node_stack = Array<StackBlock*>();
		Rule* _rule_buffer = nullptr;

		void _push() {
			_node_stack.push_back(new StackBlock(_rule_buffer));
			_rule_buffer = nullptr;
		}

	public:
		pParserStack(Rule* top_level_rule) {
			_rule_buffer = top_level_rule;
			_push();
		}

		~pParserStack() {
			auto stack_size = _node_stack.size();
			for (size_t i = 0; i < stack_size; i++) {
				delete _node_stack[i];
				_node_stack[i] = nullptr;
			}
		}

		void register_rule(Rule* rule) {
			_rule_buffer = rule;
		}

		ParserNode* get_node() {
			if (_node_stack.size() == 0) {
				return nullptr;
			}
			return _node_stack[_node_stack.size() - 1]->get_node();
		}

	private:
		bool _remove_stack_block() {
			delete _node_stack--;
			if (_node_stack.size() > 0) {
				return true;
			}
			return false;
		}

	public:
		// returns whether it is possible to obtain another node
		bool success() {
			bool status = _node_stack.size() > 0 ? _node_stack[_node_stack.size() - 1]->next_node() : false;
			if (!status) {
				status = _remove_stack_block();
			}
			if (_rule_buffer) {
				_push();
				status = true;
			}
			return status;
		}

		// returns whether it is possible to obtain another node
		bool fail() {
			if (_node_stack[_node_stack.size() - 1]->selected_node == 0) {
				// let's try the next sequence
				bool status = _node_stack[_node_stack.size() - 1]->next_sequence();
				if (!status) {
					// there's no more sequence in current block, this is ok if the block has an epsilon sequence
					bool has_epsilon = _node_stack[_node_stack.size() - 1]->has_epsilon_sequence;
					if (has_epsilon) {
						// block has epsilon, it can safely be removed
						status = _remove_stack_block();
					}
					else {
						// no epsilon, this node should not have failed, the input or grammar is erroneous
						return false;	// end parse
					}
				}
				if (_rule_buffer) {
					_push();
					status = true;
				}
				return status;
			}
			return false;	// end parse
		}

	};
}