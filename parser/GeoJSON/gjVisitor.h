#pragma once
#include "../LL1/parser/pParserVisitor.h"
#include "gjEnums.h"

namespace nsGeoJSON {
	template<class enum_t, class enum_r, typename = std::enable_if_t<std::is_enum_v<enum_t> && std::is_enum_v<enum_r>>>
	class gjVisitor : public nspParser::pParserVisitor<gjToken, gjRule> {
		using Rule = nspParser::pRule<gjToken, gjRule>;
		template<class T>
		using Array = nspArray::pArray<T>;

	public:
		gjVisitor(Lexer* lexer, Array<Rule*>* rules) : nspParser::pParserVisitor<gjToken, gjRule>(lexer, rules) {

		}

		~gjVisitor() {
		}
	};
}