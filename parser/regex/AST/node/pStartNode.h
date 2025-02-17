#pragma once
#include "../pRegexNode.h"

namespace nspRegexAST {
	// discontinued as it will likely not be needed in lexer
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
			visitor.visit(*this);
		}

		template <class visitor_type>
		void accept(Visitor<visitor_type>* visitor) {
			visitor->visit(*this);
		}

		char get_type() override {
			return 's';
		}
	};
}