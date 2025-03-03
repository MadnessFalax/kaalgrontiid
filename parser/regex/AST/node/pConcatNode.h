#pragma once
#include "../pRegexNode.h"
#include "../../../../container/pArray.h"
#include "../../../../container/pString.h"

#ifdef _DEBUG
#define __PROFILE_CN printf("ConcatNode.\n");
#endif

#ifndef _DEBUG
#define __PROFILE_CN
#endif

namespace nspRegexAST {
	template <class value_type>
	using Array = nspArray::pArray<value_type>;
	using String = nspString::pString;

	class pConcatNode : public pRegexNode {

	public:
		Array<pRegexNode*>* _child_nodes = nullptr;

		// takes ownership of qualified_characters array 
		pConcatNode() = delete;
		pConcatNode(Array<pRegexNode*>* qualified_characters) {
			if (qualified_characters == nullptr) {
				_child_nodes = new Array<pRegexNode*>();
			}
			else {
				_child_nodes = qualified_characters;
			}
		};
		pConcatNode(const pConcatNode& other) : _child_nodes(new Array<pRegexNode*>(*other._child_nodes)) {}
		pConcatNode(pConcatNode&& other) noexcept : _child_nodes(other._child_nodes) {
			other._child_nodes = nullptr;
		}
		~pConcatNode() {
			for (size_t i = 0; i < _child_nodes->size(); i++) {
				delete (*_child_nodes)[i];
				(*_child_nodes)[i] = nullptr;
			}

			delete _child_nodes;
			_child_nodes = nullptr;
		}

		void print(unsigned int indent = 0) override {
			auto prefix = String("");
			for (unsigned int i = 0; i < indent; i++) {
				prefix += "  ";
			}
			printf("%sConcatenationNode\n", prefix.c_str());
			auto size = _child_nodes->size();
			for (size_t i = 0; i < size; i++) {
				(*_child_nodes)[i]->print(indent + 1);
			}
		}

		pConcatNode& operator=(const pConcatNode& other) {
			if (this != &other) {
				delete _child_nodes;
				_child_nodes = nullptr;
				_child_nodes = new Array<pRegexNode*>(*other._child_nodes);
			}

			return *this;
		}

		pConcatNode& operator=(pConcatNode&& other) noexcept {
			if (this != &other) {
				delete _child_nodes;
				_child_nodes = nullptr;
				_child_nodes = other._child_nodes;
				other._child_nodes = nullptr;
			}

			return *this;
		}

		void operator+=(pRegexNode* node) {
			_child_nodes->push_back(node);
		}

		template <class visitor_type>
		void accept(Visitor<visitor_type>& visitor) {
			visitor.visit(*this);
		}

		template <class visitor_type>
		void accept(Visitor<visitor_type>* visitor) {
			visitor->visit(*this);
		}

		char get_type() override { 
			return 'c'; 
		}
	};
}