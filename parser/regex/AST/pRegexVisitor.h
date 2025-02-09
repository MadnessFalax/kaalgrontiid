#pragma once
#include "../../../abstract/pASTVisitor.h"
#include "pRegexNode.h"
#include "node/pQuantifierNode.h"
#include "node/pAlternationNode.h"
#include "node/pQualifierNode.h"
#include "node/pStartNode.h"
#include "node/pEndNode.h"
#include "node/pConcatNode.h"

namespace nspRegexAST {
	using baseNode = pRegexNode;

	class pRegexVisitor : public nspAST::pASTVisitor<pRegexVisitor> {
	public:
		void resolve_visit(pQuantifierNode& node) {
			printf("resolve_visit for pQuantifierNode");
		}

		void resolve_visit(pQualifierNode& node) {
			printf("resolve_visit for pQualifierNode");

		}

		void resolve_visit(pConcatNode& node) {
			printf("resolve_visit for pConcatNode");

		}

		void resolve_visit(pStartNode& node) {
			printf("resolve_visit for pStartNode");

		}

		void resolve_visit(pEndNode& node) {
			printf("resolve_visit for pEndNode");

		}

		void resolve_visit(pAlternationNode& node) {
			printf("resolve_visit for pAlternationNode");
		}

	};
}