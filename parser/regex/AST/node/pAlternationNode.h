#pragma once
#include "../pRegexNode.h"

namespace nspRegexAST {
	class pAlternationNode : public pRegexNode {
	public:
		pRegexNode* _left = nullptr;
		pRegexNode* _right = nullptr;

		pAlternationNode() = default;
		pAlternationNode(pRegexNode* left, pRegexNode* right) : _left(left), _right(right) {};
		pAlternationNode(pAlternationNode& other) = default;
		pAlternationNode(pAlternationNode&& other) = default;
		~pAlternationNode() { _left = _right = nullptr; };
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

		char get_type() override {
			return 'a';
		};
	};
}