#pragma once
#include "../LL1/parser/pParserVisitor.h"
#include "../LL1/parser/pErrorReporter.h"
#include "gjEnums.h"

namespace nsGeoJSON {
	class gjVisitor : public nspParser::pParserVisitor<gjToken, gjRule, gjHandler, gjVisitor> {
		using Rule = nspParser::pRule<gjToken, gjRule>;
		template<class T>
		using Array = nspArray::pArray<T>;
		using pErrorReporter = nspParser::pErrorReporter;

		enum depth {
			ISPOINT,
			ISLINE,
			WASLINE,
			NOTIMPORTANT
		};


		depth _depth = NOTIMPORTANT;
		size_t _dimension = 0;
		Array<double>* _temp_coords = nullptr;
		Array<Array<double>*>* _coordinates = nullptr;

	public:
		gjVisitor(Lexer* lexer, Array<Rule*>* rules) : nspParser::pParserVisitor<gjToken, gjRule, gjHandler, gjVisitor>(lexer, rules) {
			_temp_coords = new Array<double>();
			_coordinates = new Array<Array<double>*>();
		}

		~gjVisitor() {
			delete _temp_coords;
			_temp_coords = nullptr;
			auto _c_size = _coordinates->size();
			for (size_t i = 0; i < _c_size; i++) {
				delete (*_coordinates)[i];
			}
			delete _coordinates;
			_coordinates = nullptr;

		}

		void custom_visit_root(CustomNode& node) override {
			switch (node.get_handler_id()) {
			case gjHandler::CoordinatesHandler:
				resolve_custom_visit<gjHandler::CoordinatesHandler>(node);
				break;
			case gjHandler::DepthOutHandler:
				resolve_custom_visit<gjHandler::DepthOutHandler>(node);
				break;
			case gjHandler::GeoObjectHandler:
				resolve_custom_visit<gjHandler::GeoObjectHandler>(node);
				break;
			case gjHandler::CommitPointHandler:
				resolve_custom_visit<gjHandler::CommitPointHandler>(node);
				break;
			}
		}

		template<gjHandler handler_id>
		void resolve_custom_visit(CustomNode& node) {
			printf("You have implemented this visitor wrong, you moron!");
		}

		template<>
		void resolve_custom_visit<gjHandler::CoordinatesHandler>(CustomNode& node) {
			if (_context.end) {
				_context.last_status = Context::LastStatus::OK;
				return;
			}

			// perform type check
			if (_context.current_instance->get_prototype()->get_id() != gjToken::NUMBER) {
				pErrorReporter::report_rule(_rule_map[_context.current_rule]->get_name(), _context.current_instance->get_prototype()->get_name());
				pErrorReporter::report_token(_context.current_instance->get_position(), _context.current_instance->get_value(), _context.current_instance->get_prototype()->get_name(), "Got unexpected token.\n");
				_context.last_status = Context::LastStatus::FAIL;
				return;
			}

			if (_try_extract_number()) {
				_context.last_status = Context::LastStatus::OK;
				_temp_coords->push_back(double{ _context.last_extracted_number });
				_consume();
			}
			else {
				pErrorReporter::report_rule(_rule_map[_context.current_rule]->get_name(), _context.current_instance->get_prototype()->get_name());
				pErrorReporter::report_token(_context.current_instance->get_position(), _context.current_instance->get_value(), _context.current_instance->get_prototype()->get_name(), "Failed to extract number.\n");
				_context.last_status = Context::LastStatus::FAIL;
			}
			return;
		}

		template<>
		void resolve_custom_visit<gjHandler::DepthOutHandler>(CustomNode& node) {
			size_t c_size = 0;
			size_t sub_c_size = 0;
			switch (_depth) {
			case depth::ISPOINT:
				if (_context.last_extracted_string == "\"Point\"") {
					printf("Point\n");
					printf("[");
					sub_c_size = (*_coordinates)[0]->size();
					for (size_t i = 0; i < sub_c_size; i++) {
						printf("%f", (*(*_coordinates)[0])[i]);
						if (i < sub_c_size - 1) {
							printf(",");
						}
					}
					printf("]\n");
				}
				_depth = depth::ISLINE;
				break;
			case depth::ISLINE:
				_depth = depth::WASLINE;
				printf("%s\n", reinterpret_cast<const char*>(_context.last_extracted_string.c_str()));
				printf("[");
				c_size = _coordinates->size();
				for (size_t i = 0; i < c_size; i++) {
					printf("[");
					sub_c_size = (*_coordinates)[i]->size();
					for (size_t j = 0; j < sub_c_size; j++) {
						printf("%f", (*(*_coordinates)[i])[j]);
						if (j < sub_c_size - 1) {
							printf(",");
						}
					}
					printf("]");
					if (i < c_size - 1) {
						printf(",");
					}
				}
				printf("]\n");
				for (size_t i = 0; i < c_size; i++) {
					delete (*_coordinates)[i];
				}
				delete _coordinates;
				_coordinates = new Array<Array<double>*>();
				break;
			case depth::WASLINE:
				_depth = depth::NOTIMPORTANT;
				break;
			case depth::NOTIMPORTANT:
				break;
			}
		}

		template<>
		void resolve_custom_visit<gjHandler::GeoObjectHandler>(CustomNode& node) {
			auto c_size = _coordinates->size();
			for (size_t i = 0; i < c_size; i++) {
				delete (*_coordinates)[i];
			}
			delete _coordinates;
			_coordinates = new Array<Array<double>*>();
		}

		template<>
		void resolve_custom_visit<gjHandler::CommitPointHandler>(CustomNode& node) {
			_coordinates->push_back(_temp_coords);
			_dimension = _temp_coords->size();
			_temp_coords = new Array<double>();
			_depth = depth::ISPOINT;
		}
	};
}