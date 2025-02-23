#pragma once
#include "../pRegexNode.h"
#include "../../../../container/pString.h"

#ifdef _DEBUG
#define __PROFILE_QN printf("QuantifierNode.\n");
#endif

#ifndef _DEBUG
#define __PROFILE_QN
#endif

namespace nspRegexAST {
	class pQuantifierNode : public pRegexNode {
		using String = nspString::pString;

	public:
		unsigned int _min = 1;
		unsigned int _max = 1;
		bool _is_infinite = false;
		pRegexNode* _qual_node = nullptr;
	
		pQuantifierNode() = delete;
		// max lower than min means that higher bound is infinite
		pQuantifierNode(unsigned int min, unsigned int max, pRegexNode* qual) : _min{ min }, _max{ max }, _qual_node(qual) {
			
			__PROFILE_QN
			
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

		void print(unsigned int indent = 0) override {
			auto prefix = String("");
			for (unsigned int i = 0; i < indent; i++) {
				prefix += "  ";
			}
			printf("%sQuantifierNode\n", prefix.c_str());
			printf("%s  min: %d\n", prefix.c_str(), _min);
			printf("%s  max: %d\n", prefix.c_str(), _max);
			printf("%s  is_infinite: %s\n", prefix.c_str(), _is_infinite ? "true" : "false");
			_qual_node->print(indent + 1);
		}

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