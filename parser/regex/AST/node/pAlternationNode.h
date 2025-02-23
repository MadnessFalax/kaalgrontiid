#pragma once

#ifdef _DEBUG
#define __PROFILE_AN printf("AlternationNode.\n");
#endif

#ifndef _DEBUG
#define __PROFILE_AN
#endif

#include "../pRegexNode.h"
#include "../../../../container/pString.h"

namespace nspRegexAST {
	class pAlternationNode : public pRegexNode {
		using String = nspString::pString;
	public:
		pRegexNode* _left = nullptr;
		pRegexNode* _right = nullptr;

		pAlternationNode() = default;
		pAlternationNode(pRegexNode* left, pRegexNode* right) : _left(left), _right(right) {		};
		pAlternationNode(pAlternationNode& other) = default;
		pAlternationNode(pAlternationNode&& other) = default;
		~pAlternationNode() { 
			delete _left;
			delete _right;
			_left = _right = nullptr;
		};
		pAlternationNode& operator=(pAlternationNode& other) = default;
		pAlternationNode& operator=(pAlternationNode&& other) = default;
		
		template <class visitor_type>
		void accept(Visitor<visitor_type>& visitor) {
			visitor.visit(*this);
		}

		void print(unsigned int indent = 0) override {
			auto prefix = String("");
			for (unsigned int i = 0; i < indent; i++) {
				prefix += "  ";
			}
			printf("%sAlternationNode\n", prefix.c_str());
			_left->print(indent + 1);
			_right->print(indent + 1);
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