#pragma once
#include "../pRegexNode.h"

namespace nspRegexAST {
	class pEndNode : public pRegexNode {
	public:
		pEndNode() = default;
		pEndNode(pEndNode& other) = default;
		pEndNode(pEndNode&& other) = default;
		~pEndNode() = default;
		pEndNode& operator=(pEndNode& other) = default;
		pEndNode& operator=(pEndNode&& other) = default;

		template <class visitor_type>
		void accept(Visitor<visitor_type>& visitor) {
			visitor->visit(*this);
		}
	};
}