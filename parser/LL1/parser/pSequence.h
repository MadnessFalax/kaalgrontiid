#pragma once
#include "node/pConsumeNode.h"
#include "node/pCustomNode.h"
#include "node/pExtractNode.h"
#include "node/pParserNode.h"
#include "node/pForwardNode.h"

namespace nspParser {
	template<class T>
	using Array = nspArray::pArray<T>;

	// represents one right side of rule
	template<typename enum_t, typename enum_r>
	class pSequence {
		bool _is_empty = true;
		Array<pParserNode<enum_t, enum_r>*> _nodes = Array<pParserNode<enum_t, enum_r>*>();

	public:
		pSequence() = default;

		~pSequence() {
			auto nodes_size = _nodes.size();
			for (size_t i = 0; i < nodes_size; i++) {
				delete _nodes[i];
				_nodes[i] = nullptr;
			}
		}

		// takes ownership of node
		void add_node(pParserNode<enum_t, enum_r>* node) {
			_nodes.push_back(node);
			_is_empty = false;
		}

		const Array<pParserNode<enum_t, enum_r>*>& get_nodes() const {
			return _nodes;
		}

		pSequence& operator<<(pParserNode<enum_t, enum_r>* node) {
			add_node(node);
			return *this;
		}
	};
}