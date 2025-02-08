#pragma once
#include "../pRegexNode.h"

namespace nspRegexAST {
	class pAlternationNode : public pRegexNode {
		pAlternationNode() = default;
		pAlternationNode(pAlternationNode& other) = default;
		pAlternationNode(pAlternationNode&& other) = default;
		~pAlternationNode() = default;
		pAlternationNode& operator=(pAlternationNode& other) = default;
		pAlternationNode& operator=(pAlternationNode&& other) = default;
	};
}