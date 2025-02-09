#pragma once
#include "../pRegexNode.h"

namespace nspRegexAST {
	class pQuantifierNode : public pRegexNode {
	public:
		unsigned int _min = 0;
		unsigned int _max = 0;
	
		pQuantifierNode() = default;
		pQuantifierNode(unsigned int min, unsigned int max) : _min{ min }, _max{ max } {};
		pQuantifierNode(const pQuantifierNode& other) = default;
		pQuantifierNode(pQuantifierNode&& other) = default;
		~pQuantifierNode() = default;
		pQuantifierNode& operator=(const pQuantifierNode& other) = default;
		pQuantifierNode& operator=(pQuantifierNode&& other) = default;

		template <class visitor_type>
		void accept(Visitor<visitor_type>& visitor) {
			visitor->visit(*this);
		}
	};
}