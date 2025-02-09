#pragma once
#include "../pRegexNode.h"

namespace nspRegexAST {
	class pAlternationNode : public pRegexNode {
	public:
		pAlternationNode() = default;
		pAlternationNode(pAlternationNode& other) = default;
		pAlternationNode(pAlternationNode&& other) = default;
		~pAlternationNode() = default;
		pAlternationNode& operator=(pAlternationNode& other) = default;
		pAlternationNode& operator=(pAlternationNode&& other) = default;
		
		template <class visitor_type>
		void accept(Visitor<visitor_type>& visitor) {
			visitor.visit(*this);
		}

		template <class visitor_type>
		void accept(Visitor<visitor_type>* visitor) {
			visitor->visit(*this);
		}
	};
}