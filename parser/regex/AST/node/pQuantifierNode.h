#pragma once
#include "../pRegexNode.h"

namespace nspRegexAST {
	class pQuantifierNode : public pRegexNode {
	public:
		unsigned int _min = 0;
		unsigned int _max = 0;
		pRegexNode* _qual_node = nullptr;
	
		pQuantifierNode() = default;
		pQuantifierNode(unsigned int min, unsigned int max, pRegexNode* qual) : _min{ min }, _max{ max }, _qual_node(qual) {};
		pQuantifierNode(const pQuantifierNode& other) = default;
		pQuantifierNode(pQuantifierNode&& other) = default;
		~pQuantifierNode() { _qual_node = nullptr; };
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