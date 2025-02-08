#pragma once
#include "../pRegexNode.h"
#include "../../../../container/pArray.h"


namespace nspRegexAST {
	template <class value_type>
	using Array = nspArray::pArray<value_type>;

	class pConcatNode : public pRegexNode {
		Array<pRegexNode*>* _child_nodes = nullptr;

	public:
		// takes ownership of qualified_characters array 
		pConcatNode() : _child_nodes(new Array<pRegexNode*>()) {};
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
			delete _child_nodes;
			_child_nodes = nullptr;
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

	};
}