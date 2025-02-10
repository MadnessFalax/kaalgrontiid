#pragma once
#include "../pRegexNode.h"

namespace nspRegexAST {
	class pQuantifierNode : public pRegexNode {
	public:
		unsigned int _min = 1;
		unsigned int _max = 1;
		bool _is_infinite = false;
		pRegexNode* _qual_node = nullptr;
	
		pQuantifierNode() = default;
		// max lower than min means that higher bound is infinite
		pQuantifierNode(unsigned int min, unsigned int max, pRegexNode* qual) : _min{ min }, _max{ max }, _qual_node(qual) {
			if (_max < _min || _max == 0) {
				_is_infinite = true;
			}
			if (_is_infinite) {
				_max = (_min == 0 ? 1 : _min);
			}
		};
		pQuantifierNode(const pQuantifierNode& other) = default;
		pQuantifierNode(pQuantifierNode&& other) = default;
		~pQuantifierNode() { 
			delete _qual_node;
			_qual_node = nullptr;
		};
		pQuantifierNode& operator=(const pQuantifierNode& other) = default;
		pQuantifierNode& operator=(pQuantifierNode&& other) = default;

		template <class visitor_type>
		void accept(Visitor<visitor_type>& visitor) {
			visitor.visit(*this);
		}

		template <class visitor_type>
		void accept(Visitor<visitor_type>* visitor) {
			visitor->visit(*this);
		}

		char get_type() override {
			return 'n';
		}
	};
}