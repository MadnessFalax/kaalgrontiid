#pragma once
#include "../LL1/parser/pParserVisitor.h"
#include "gjEnums.h"

namespace nsGeoJSON {
	class gjVisitor : public nspParser::pParserVisitor<gjToken, gjRule, gjHandler, gjVisitor> {
		using Rule = nspParser::pRule<gjToken, gjRule>;
		template<class T>
		using Array = nspArray::pArray<T>;

	public:
		gjVisitor(Lexer* lexer, Array<Rule*>* rules) : nspParser::pParserVisitor<gjToken, gjRule, gjHandler, gjVisitor>(lexer, rules) {

		}

		void custom_visit_root(CustomNode& node) override {
			switch (node.get_handler_id()) {
			case gjHandler::CoordinatesHandler:
				resolve_custom_visit<gjHandler::CoordinatesHandler>(node);
				break;
			}
		}

		template<gjHandler handler_id>
		void resolve_custom_visit(CustomNode& node) {
			printf("You have implemented this visitor wrong, you moron!");
		}

		template<>
		void resolve_custom_visit<gjHandler::CoordinatesHandler>(CustomNode& node) {

		}

		template<>
		void resolve_custom_visit<gjHandler::CoordinatesRootHandler>(CustomNode& node) {

		}

		template<>
		void resolve_custom_visit<gjHandler::CoordinatesTailHandler>(CustomNode& node) {

		}

		template<>
		void resolve_custom_visit<gjHandler::CoordinatesRootTailHandler>(CustomNode& node) {

		}

		template<>
		void resolve_custom_visit<gjHandler::GPropertyHandler>(CustomNode& node) {

		}

		~gjVisitor() {
		}
	};
}