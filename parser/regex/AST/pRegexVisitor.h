#pragma once
#include "../../../abstract/pASTVisitor.h"
#include "pRegexNode.h"
#include "node/pQuantifierNode.h"
#include "node/pAlternationNode.h"
#include "node/pQualifierNode.h"
#include "node/pConcatNode.h"
#include "../NFA/pState.h"
#include "../NFA/pAutomaton.h"
#include "../pBadPatternException.h"
#include <type_traits>

namespace nspRegexAST {
	using baseNode = pRegexNode;

	template<class value_type>
	using Array = nspArray::pArray<value_type>;
	using NFA = nspNFA::pAutomaton;
	using State = nspNFA::pState;

	class pRegexVisitor : public nspAST::pASTVisitor<pRegexVisitor> {
		pRegexNode* _root_node = nullptr;
		State* _starting_state = nullptr;
		State* _connector_state = nullptr;
		Array<State*>* _state_closure = new Array<State*>();

		State* _new_state() {
			auto* state = new State();
			_state_closure->push_back(state);
			return state;
		}

	public:
		pRegexVisitor() {
			_starting_state = _new_state();
			_connector_state = _starting_state;
		};

		// takes ownership of root_node
		pRegexVisitor(pRegexNode* root_node) : _root_node(root_node) {
			_starting_state = _new_state();
			_connector_state = _starting_state;
		};
		~pRegexVisitor() {
			_starting_state = nullptr;
			_connector_state = nullptr;
			
			if (_state_closure) {
				auto size = _state_closure->size();
				for (size_t i = 0; i < size; i++) {
					delete (*_state_closure)[i];
					(*_state_closure)[i] = nullptr;
				}
				delete _state_closure;
			}

			delete _root_node;
			_root_node = nullptr;
		}

		NFA* create_NFA() {
			auto* automaton = new NFA(_state_closure, _starting_state);
			_state_closure = nullptr;
			_starting_state = nullptr;
			return automaton;
		}

		void resolve_tree() {
			auto node_type = _root_node->get_type();
			if (node_type == 'a') {
				dynamic_cast<pAlternationNode*>(_root_node)->accept(*this);
			}
			else if (node_type == 'c') {
				dynamic_cast<pConcatNode*>(_root_node)->accept(*this);
			}
			else if (node_type == 'l') {
				dynamic_cast<pQualifierNode*>(_root_node)->accept(*this);
			}
			else if (node_type == 'n') {
				dynamic_cast<pQuantifierNode*>(_root_node)->accept(*this);
			}
			else {
				throw pBadPatternException();
			}

			_connector_state->set_final();
		}

		void resolve_visit(pRegexNode& node) {
			throw pBadPatternException();
		}

		void resolve_visit(pQuantifierNode& node) {
			auto* ending_state = _new_state();

			auto* starting_state = _connector_state;

			if (node._min == 0) {
				starting_state->register_epsilon(ending_state);
			}

			auto node_type = node._qual_node->get_type();
			pAlternationNode* alt_node = nullptr;
			pConcatNode* conc_node = nullptr;
			pQualifierNode* qual_node = nullptr;
			if (node_type == 'a') {
				alt_node = dynamic_cast<pAlternationNode*>(node._qual_node);
			}
			else if (node_type == 'c') {
				conc_node = dynamic_cast<pConcatNode*>(node._qual_node);
			}
			else if (node_type == 'l') {
				qual_node = dynamic_cast<pQualifierNode*>(node._qual_node);
			}
			else {
				throw pBadPatternException();
			}
			for (unsigned int i = 1; i <= node._max; i++) {
				auto* sequence_start = _connector_state;

				if (alt_node) {
					alt_node->accept(*this);
				}
				else if (conc_node) {
					conc_node->accept(*this);
				}
				else if (qual_node) {
					qual_node->accept(*this);
				}

				auto* sequence_end = _connector_state;

				if (node._is_infinite && i == node._max) {
					sequence_end->register_epsilon(sequence_start);
				}
				if (i >= node._min) {
					sequence_end->register_epsilon(ending_state);
				}
			}

			_connector_state = ending_state;
		}

		void resolve_visit(pQualifierNode& node) {
			auto* ending_state = _new_state();

			auto* starting_state = _connector_state;
			
			for (auto ch : *node._qualified_characters) {
				auto* l_state = _new_state();

				auto* r_state = _new_state();

				starting_state->register_epsilon(l_state);
				l_state->register_transition(ch, r_state);
				r_state->register_epsilon(ending_state);
			}
			
			_connector_state = ending_state;
		}

		void resolve_visit(pConcatNode& node) {

			for (auto* rn : *node._child_nodes) {
				auto node_type = rn->get_type();

				if (node_type == 'a') {
					dynamic_cast<pAlternationNode*>(rn)->accept(*this);
				}
				else if (node_type == 'c') {
					dynamic_cast<pConcatNode*>(rn)->accept(*this);
				}
				else if (node_type == 'l') {
					dynamic_cast<pQualifierNode*>(rn)->accept(*this);
				}
				else if (node_type == 'n') {
					dynamic_cast<pQuantifierNode*>(rn)->accept(*this);
				}
			}
		}

		void resolve_visit(pAlternationNode& node) {
			auto* starting_state = _connector_state;
			auto* end_state = _new_state();

			auto* left_start_state = _new_state();
			auto* right_start_state = _new_state();

			starting_state->register_epsilon(left_start_state);
			starting_state->register_epsilon(right_start_state);

			_connector_state = left_start_state;

			auto left_type = node._left->get_type();
			if (left_type == 'a') {
				dynamic_cast<pAlternationNode*>(node._left)->accept(*this);
			}
			else if (left_type == 'c') {
				dynamic_cast<pConcatNode*>(node._left)->accept(*this);
			}
			else if (left_type == 'l') {
				dynamic_cast<pQualifierNode*>(node._left)->accept(*this);
			}
			else if (left_type == 'n') {
				dynamic_cast<pQuantifierNode*>(node._left)->accept(*this);
			}
			else {
				throw pBadPatternException();
			}

			auto* left_end_state = _connector_state;
			left_end_state->register_epsilon(end_state);

			_connector_state = right_start_state;

			auto right_type = node._right->get_type();
			if (right_type == 'a') {
				dynamic_cast<pAlternationNode*>(node._right)->accept(*this);
			}
			else if (right_type == 'c') {
				dynamic_cast<pConcatNode*>(node._right)->accept(*this);
			}
			else if (right_type == 'l') {
				dynamic_cast<pQualifierNode*>(node._right)->accept(*this);
			}
			else if (right_type == 'n') {
				dynamic_cast<pQuantifierNode*>(node._right)->accept(*this);
			}
			else {
				throw pBadPatternException();
			}

			auto* right_end_state = _connector_state;
			right_end_state->register_epsilon(end_state);

			_connector_state = end_state;
		}

	};
}