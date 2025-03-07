#pragma once
#include <type_traits>
#include "../../../../container/pString.h"
#include "../../lexer/pTokenPrototype.h"
#include "../../../../abstract/pBaseVisitor.h"

namespace nspParser {
	using String = nspString::pString;
	
	template<class enum_t>
	using TokenPrototype = nspLexer::pTokenPrototype<enum_t, nspLexer::pPrototypeKind::REGEX>;
	
	template<class enum_t, typename = std::enable_if_t<std::is_enum_v<enum_t>>>
	class pParserNode {
		using Visitor = pBaseVisitor<pParserNode<enum_t>>;

	public:
	};
}