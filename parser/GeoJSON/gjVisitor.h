#pragma once
#include "../LL1/parser/pParserVisitor.h"
#include "gjEnums.h"

namespace nsGeoJSON {
	class gjVisitor : public nspParser::pParserVisitor<gjToken, gjRule> {

	public:
		gjVisitor() {

		}
	};
}