#pragma once
#include "../pRegexNode.h"

namespace nspRegexAST {
	class pEndNode : public pRegexNode {
		pEndNode() = default;
		pEndNode(pEndNode& other) = default;
		pEndNode(pEndNode&& other) = default;
		~pEndNode() = default;
		pEndNode& operator=(pEndNode& other) = default;
		pEndNode& operator=(pEndNode&& other) = default;
	};
}