#pragma once
#include "../pRegexNode.h"

namespace nspRegexAST {
	// discontinued as it will likely not be needed in lexer
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
			visitor.visit(*this);
		}

		template <class visitor_type>
		void accept(Visitor<visitor_type>* visitor) {
			visitor->visit(*this);
		}

		char get_type() override {
			return 'e';
		}
	};
}