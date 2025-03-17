#pragma once
#include "../LL1/parser/pParserVisitor.h"
#include "../LL1/parser/pErrorReporter.h"
#include "osmEnums.h"

namespace nsOSM {
	class osmVisitor : public nspParser::pParserVisitor<osmToken, osmRule, osmHandler, osmVisitor> {
		using Rule = nspParser::pRule<osmToken, osmRule>;
		template<class T>
		using Array = nspArray::pArray<T>;
		using pErrorReporter = nspParser::pErrorReporter;

		Map<size_t, Array<double>*, unsigned short> _nodes;
		size_t _dimension = 0;
		Array<double>* _point = nullptr;
		Array<Array<double>*>* _points = nullptr;

	public:
		osmVisitor(Lexer* lexer, Array<Rule*>* rules, Stack* stack) : nspParser::pParserVisitor<osmToken, osmRule, osmHandler, osmVisitor>(lexer, rules, stack) {
			_point = new Array<double>();
			_points = new Array<Array<double>*>();
		}

		~osmVisitor() {
			delete _point;
			_point = nullptr;
			if (_points) {
				size_t p_size = _points->size();
				for (size_t i = 0; i < p_size; i++) {
					delete (*_points)[i];
					(*_points)[i] = nullptr;
				}
				delete _points;
				_points = nullptr;
			}
		}

		void custom_visit_root(CustomNode& node) override {
			switch (node.get_handler_id()) {
			default:
				break;
			}
		}

		template<osmHandler handler_id>
		void resolve_custom_visit(CustomNode& node) {
			printf("You have implemented this visitor wrong, you moron!");
		}
	};
}