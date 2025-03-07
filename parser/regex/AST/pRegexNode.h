#pragma once
#include <cstdio>
#include "../../../abstract/pBaseVisitor.h"
#include "../../../container/pArray.h"

namespace nspRegexAST {
	constexpr unsigned char qual_min = 32;
	constexpr unsigned char qual_max_utf8 = 255;
	constexpr unsigned char qual_max = 127;

	class pRegexNode  {
	public:
		template<class value_type>
		using Array = nspArray::pArray<value_type>;

		struct Context {
			pRegexNode* left_node = nullptr;
			pRegexNode* right_node = nullptr;
			pRegexNode* parent_node = nullptr;
			Array<pRegexNode*>* children_nodes = nullptr;
			pRegexNode* node = nullptr;

			Context(pRegexNode* in_node) {
				children_nodes = new Array<pRegexNode*>();
				node = in_node;
			}

			~Context() {
				delete children_nodes;
				children_nodes = nullptr;
				left_node = nullptr;
				right_node = nullptr;
				parent_node = nullptr;
				node = nullptr;
			}

			void set_left(pRegexNode* left) {
				left_node = left;
				left->context->right_node = node;
			}

			void set_right(pRegexNode* right) {
				right_node = right;
				right->context->left_node = node;
			}

			void set_parent(pRegexNode* parent) {
				parent_node = parent;
				parent->context->add_child(node);
			}

			void add_child(pRegexNode* child) {
				children_nodes->push_back(child);
				child->context->parent_node = node;
			}
		};

		template<class visitor_type>
		using Visitor = pBaseVisitor<visitor_type>;

		Context* context = new Context(this);

		virtual ~pRegexNode() { delete context; };

		virtual void print(unsigned int indent = 0) {
			printf("generic RegexNode\n");
		}

		template <class visitor_type>
		void accept(Visitor<visitor_type>& visitor) {
			visitor.visit(*this);
		}

		template <class visitor_type>
		void accept(Visitor<visitor_type>* visitor) {
			visitor->visit(*this);
		}

		virtual char get_type() {
			return 'b';
		}
	};
}