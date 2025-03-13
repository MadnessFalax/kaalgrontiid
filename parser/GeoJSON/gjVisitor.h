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

		size_t _depth = 0;
		size_t _point_depth = 0;
		bool _point_depth_set = false;
		size_t _dimension = 0;
		Array<size_t>* _line_indices = nullptr;
		Array<double>* _coordinates = nullptr;

	public:
		gjVisitor(Lexer* lexer, Array<Rule*>* rules) : nspParser::pParserVisitor<gjToken, gjRule, gjHandler, gjVisitor>(lexer, rules) {
			_line_indices = new Array<size_t>();
			_line_indices->push_back(0);
			_coordinates = new Array<double>();
		}

		~gjVisitor() {
			delete _line_indices;
			_line_indices = nullptr;
			delete _coordinates;
			_coordinates = nullptr;
		}

		void custom_visit_root(CustomNode& node) override {
			switch (node.get_handler_id()) {
			case gjHandler::CoordinatesHandler:
				resolve_custom_visit<gjHandler::CoordinatesHandler>(node);
				break;
			case gjHandler::CoordinatesExit:
				resolve_custom_visit<gjHandler::CoordinatesExit>(node);
				break;
			case gjHandler::DepthInHandler:
				resolve_custom_visit<gjHandler::DepthInHandler>(node);
				break;
			case gjHandler::DepthOutHandler:
				resolve_custom_visit<gjHandler::DepthOutHandler>(node);
				break;
			case gjHandler::GeoObjectHandler:
				resolve_custom_visit<gjHandler::GeoObjectHandler>(node);
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
				_coordinates->push_back(double{ _context.last_extracted_number });
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
		void resolve_custom_visit<gjHandler::CoordinatesExit>(CustomNode& node) {
			_dimension = _coordinates->size();
		}

		template<>
		void resolve_custom_visit<gjHandler::DepthInHandler>(CustomNode& node) {
			_depth++;
		}

		template<>
		void resolve_custom_visit<gjHandler::DepthOutHandler>(CustomNode& node) {
			if (!_point_depth_set) {
				_point_depth = _depth;
				_point_depth_set = true;
			}
			if (_point_depth - 1 == _depth) {
				_line_indices->push_back(_coordinates->size());
			}
			_depth--;
		}

		template<>
		void resolve_custom_visit<gjHandler::GeoObjectHandler>(CustomNode& node) {
			auto _li_size = _line_indices->size();
			for (size_t i = 1; i < _li_size; i++) {
				printf("%s:\n", reinterpret_cast<const char*>(_context.last_extracted_string.c_str()));
				printf("[");
				for (size_t j = (*_line_indices)[i - 1]; j < (*_line_indices)[i]; j++) {
					printf("%f ", (*_coordinates)[j]);
					printf(", ");
				}
				printf("]\n");
			}

			_depth = 0;
			_point_depth = 0;
			_point_depth_set = false;
			_dimension = 0;
			delete _line_indices;
			_line_indices = new Array<size_t>();
			_line_indices->push_back(0);
			delete _coordinates;
			_coordinates = new Array<double>();
		}

		void print_result() {
			for (auto& c : *_coordinates) {
				printf("%f ", c);
			}
		}
	};
}