#pragma once
#include "pParserNode.h"

namespace nspParser {
	template<typename enum_t, typename enum_r>
	class pCustomNode : public pParserNode<enum_t, enum_r> {
		using Visitor = pBaseVisitor<pParserNode<enum_t, enum_r>>;

		pCustomNode() {

		}
	};
}