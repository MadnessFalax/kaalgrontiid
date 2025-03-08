#pragma once
#include "pParserNode.h"

namespace nspParser {
	template<typename enum_t, typename enum_r>
	class pExtractNode : public pParserNode<enum_t, enum_r> {
		using Visitor = pBaseVisitor<pParserNode<enum_t, enum_r>>;

	public:
		enum ExtractType {
			NUMBER,
			ENUM,
			STRING
		};

		pExtractNode() {

		}
	};
}