#pragma once
#include "../../../abstract/pASTVisitor.h"
#include "../../../container/pArray.h"

namespace nspRegexAST {
	class pRegexNode  {
	public:
		virtual ~pRegexNode() = 0;
		virtual void accept() = 0;
	};
}