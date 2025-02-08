#pragma once
#include "../pRegexNode.h"

namespace nspRegexAST {
	class pStartNode : public pRegexNode {
		pStartNode() = default;
		pStartNode(pStartNode& other) = default;
		pStartNode(pStartNode&& other) = default;
		~pStartNode() = default;
		pStartNode& operator=(pStartNode& other) = default;
		pStartNode& operator=(pStartNode&& other) = default;
	};
}