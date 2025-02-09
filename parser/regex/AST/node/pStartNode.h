#pragma once
#include "../pRegexNode.h"

namespace nspRegexAST {
	class pStartNode : public pRegexNode {
	public:
		pStartNode() = default;
		pStartNode(pStartNode& other) = default;
		pStartNode(pStartNode&& other) = default;
		~pStartNode() = default;
		pStartNode& operator=(pStartNode& other) = default;
		pStartNode& operator=(pStartNode&& other) = default;

		template <class visitor_type>
		void accept(Visitor<visitor_type>& visitor) {
			visitor->visit(*this);
		}
	};
}