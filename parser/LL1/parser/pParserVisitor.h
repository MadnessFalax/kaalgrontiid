#pragma once
#include "../../../abstract/pBaseVisitor.h"
#include "node/pParserNode.h"

namespace nspParser {
	template<typename enum_t, typename enum_r>
	class pParserVisitor : pBaseVisitor<pParserNode<enum_t, enum_r>> {
		struct Context {

		};

		Context _context;

	public:
		pParserVisitor() {

		}
	};
}