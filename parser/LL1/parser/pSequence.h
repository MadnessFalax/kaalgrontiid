#pragma once
#include "node/pConsumeNode.h"
#include "node/pCustomNode.h"
#include "node/pExtractNode.h"
#include "node/pParserNode.h"
#include "node/pForwardNode.h"

namespace nspParser {
	// represents one right side of rule
	class pSequence {
		bool _is_empty = true;
		Array<pRule*> _rules = Array<pRule*>();
	};
}