#pragma once
#include "../LL1/parser/pParserVisitor.h"
#include "../LL1/parser/pErrorReporter.h"
#include "kmlEnums.h"

namespace nsKML {
	class kmlVisitor : public nspParser::pParserVisitor<kmlToken, kmlRule, kmlHandler, kmlVisitor> {
		using Rule = nspParser::pRule<kmlToken, kmlRule>;
		template<class T>
		using Array = nspArray::pArray<T>;
		using pErrorReporter = nspParser::pErrorReporter;

	public:
		kmlVisitor(Lexer* lexer, Array<Rule*>* rules) : nspParser::pParserVisitor<kmlToken, kmlRule, kmlHandler, kmlVisitor>(lexer, rules) {
		}

		~kmlVisitor() {
		}

		void custom_visit_root(CustomNode& node) override {
			switch (node.get_handler_id()) {
			case 0:
				break;
			default:
				break;
			}
		}

		template<kmlHandler handler_id>
		void resolve_custom_visit(CustomNode& node) {
			printf("You have implemented this visitor wrong, you moron!");
		}
	};
}