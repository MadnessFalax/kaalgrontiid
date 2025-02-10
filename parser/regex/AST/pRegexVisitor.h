#pragma once
#include "../../../abstract/pASTVisitor.h"
#include "pRegexNode.h"
#include "node/pQuantifierNode.h"
#include "node/pAlternationNode.h"
#include "node/pQualifierNode.h"
#include "node/pStartNode.h"
#include "node/pEndNode.h"
#include "node/pConcatNode.h"
#include <type_traits>

namespace nspRegexAST {
	using baseNode = pRegexNode;

	template<class value_type>
	using Array = nspArray::pArray<value_type>;

	class pRegexVisitor : public nspAST::pASTVisitor<pRegexVisitor> {
		pRegexNode* _root_node = nullptr;

	public:
		pRegexVisitor() {};
		pRegexVisitor(pRegexNode* root_node) : _root_node(root_node) {};
		~pRegexVisitor() {
			delete _root_node;
			_root_node = nullptr;
		}

		void resolve_tree() {
			auto node_type = _root_node->get_type();
			if (node_type == 'a') {
				dynamic_cast<pAlternationNode*>(_root_node)->accept(*this);
			}
			else if (node_type == 'c') {
				dynamic_cast<pConcatNode*>(_root_node)->accept(*this);
			}
			else if (node_type == 'e') {
				dynamic_cast<pEndNode*>(_root_node)->accept(*this);
			}
			else if (node_type == 'l') {
				dynamic_cast<pQualifierNode*>(_root_node)->accept(*this);
			}
			else if (node_type == 'n') {
				dynamic_cast<pQuantifierNode*>(_root_node)->accept(*this);
			}
			else if (node_type == 's') {
				dynamic_cast<pStartNode*>(_root_node)->accept(*this);
			}
		}

		void resolve_visit(pRegexNode& node) {
			printf("resolve_visit for pRegexNode\n");
		}

		void resolve_visit(pQuantifierNode& node) {
			printf("resolve_visit for pQuantifierNode\n");
			auto node_type = node._qual_node->get_type();
			if (node_type == 'a') {
				dynamic_cast<pAlternationNode*>(node._qual_node)->accept(*this);
			}
			else if (node_type == 'c') {
				dynamic_cast<pConcatNode*>(node._qual_node)->accept(*this);
			}
			else if (node_type == 'e') {
				dynamic_cast<pEndNode*>(node._qual_node)->accept(*this);
			}
			else if (node_type == 'l') {
				dynamic_cast<pQualifierNode*>(node._qual_node)->accept(*this);
			}
			else if (node_type == 'n') {
				dynamic_cast<pQuantifierNode*>(node._qual_node)->accept(*this);
			}
			else if (node_type == 's') {
				dynamic_cast<pStartNode*>(node._qual_node)->accept(*this);
			}
		}

		void resolve_visit(pQualifierNode& node) {
			printf("resolve_visit for pQualifierNode\n");
			for (auto ch : *node._qualified_characters) {
				printf("%c\n", ch);
			}
		}

		void resolve_visit(pConcatNode& node) {
			printf("resolve_visit for pConcatNode\n");
			for (auto* rn : *node._child_nodes) {
				auto node_type = rn->get_type();

				if (node_type == 'a') {
					dynamic_cast<pAlternationNode*>(rn)->accept(*this);
				}
				else if (node_type == 'c') {
					dynamic_cast<pConcatNode*>(rn)->accept(*this);
				}
				else if (node_type == 'e') {
					dynamic_cast<pEndNode*>(rn)->accept(*this);
				}
				else if (node_type == 'l') {
					dynamic_cast<pQualifierNode*>(rn)->accept(*this);
				}
				else if (node_type == 'n') {
					dynamic_cast<pQuantifierNode*>(rn)->accept(*this);
				}
				else if (node_type == 's') {
					dynamic_cast<pStartNode*>(rn)->accept(*this);
				}
			}

		}

		void resolve_visit(pStartNode& node) {
			printf("resolve_visit for pStartNode\n");

		}

		void resolve_visit(pEndNode& node) {
			printf("resolve_visit for pEndNode\n");

		}

		void resolve_visit(pAlternationNode& node) {
			printf("resolve_visit for pAlternationNode\n");
			auto left_type = node._left->get_type();

			if (left_type == 'a') {
				dynamic_cast<pAlternationNode*>(node._left)->accept(*this);
			}
			else if (left_type == 'c') {
				dynamic_cast<pConcatNode*>(node._left)->accept(*this);
			}
			else if (left_type == 'e') {
				dynamic_cast<pEndNode*>(node._left)->accept(*this);
			}
			else if (left_type == 'l') {
				dynamic_cast<pQualifierNode*>(node._left)->accept(*this);
			}
			else if (left_type == 'n') {
				dynamic_cast<pQuantifierNode*>(node._left)->accept(*this);
			}
			else if (left_type == 's') {
				dynamic_cast<pStartNode*>(node._left)->accept(*this);
			}

			auto right_type = node._right->get_type();
			if (right_type == 'a') {
				dynamic_cast<pAlternationNode*>(node._right)->accept(*this);
			}
			else if (right_type == 'c') {
				dynamic_cast<pConcatNode*>(node._right)->accept(*this);
			}
			else if (right_type == 'e') {
				dynamic_cast<pEndNode*>(node._right)->accept(*this);
			}
			else if (right_type == 'l') {
				dynamic_cast<pQualifierNode*>(node._right)->accept(*this);
			}
			else if (right_type == 'n') {
				dynamic_cast<pQuantifierNode*>(node._right)->accept(*this);
			}
			else if (right_type == 's') {
				dynamic_cast<pStartNode*>(node._right)->accept(*this);
			}
		}

	};
}