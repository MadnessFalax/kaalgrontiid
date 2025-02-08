#pragma once
#include "../../../abstract/pASTVisitor.h"
#include "pRegexNode.h"
#include "node/pQuantifierNode.h"
#include "node/pAlternationNode.h"
#include "node/pQualifierNode.h"
#include "node/pStartNode.h"
#include "node/pEndNode.h"

namespace nspRegexAST {
	using baseNode = pRegexNode;

	class pRegexVisitor : nspAST::pASTVisitor<pRegexVisitor, baseNode> {
		void resolve_visit(pQuantifierNode& node) {
			node.accept();
		}
	};
}