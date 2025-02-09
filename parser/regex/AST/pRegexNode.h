#pragma once
#include "../../../abstract/pASTVisitor.h"
#include "../../../container/pArray.h"

namespace nspRegexAST {
	class pRegexNode  {
	public:
		template<class visitor_type>
		using Visitor = nspAST::pASTVisitor<visitor_type>;

		virtual ~pRegexNode() = 0;

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