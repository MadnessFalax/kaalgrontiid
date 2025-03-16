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

		size_t _dimension = 0;
		Array<double>* _point = nullptr;
		Array<Array<double>*>* _points = nullptr;
		enum depth {
			CoordsLevel,
			PointsLevel,
			ShapeLevel,
			OutsideLevel
		} _depth = OutsideLevel;

	public:
		gjVisitor(Lexer* lexer, Array<Rule*>* rules) : nspParser::pParserVisitor<gjToken, gjRule, gjHandler, gjVisitor>(lexer, rules) {
			_point = new Array<double>();
			_points = new Array<Array<double>*>();
		}

		~gjVisitor() {
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
			case gjHandler::BufferNumber:
				resolve_custom_visit<gjHandler::BufferNumber>(node);
				break;
			case gjHandler::BufferPoint:
				resolve_custom_visit<gjHandler::BufferPoint>(node);
				break;
			case gjHandler::CommitShape:
				resolve_custom_visit<gjHandler::CommitShape>(node);
				break;
			case gjHandler::DepthOut:
				resolve_custom_visit<gjHandler::DepthOut>(node);
				break;
			}
		}

		template<gjHandler handler_id>
		void resolve_custom_visit(CustomNode& node) {
			printf("You have implemented this visitor wrong, you moron!");
		}

		template<>
		void resolve_custom_visit<gjHandler::BufferNumber>(CustomNode& node) {
			_point->push_back(_context.last_extracted_number);

			_context.last_status = Context::LastStatus::OK;
		}

		template<>
		void resolve_custom_visit<gjHandler::BufferPoint>(CustomNode& node) {
			_points->push_back(_point);
			_point = new Array<double>();
			_depth = depth::CoordsLevel;

			_context.last_status = Context::LastStatus::OK;
		}

		template<>
		void resolve_custom_visit<gjHandler::CommitShape>(CustomNode& node) {
			if (_depth == depth::PointsLevel && (_context.last_extracted_string == "\"Point\"" || _context.last_extracted_string == "\"MultiPoint\"")) {
				_context.has_item = true;
				printf("%s:\n", reinterpret_cast<const char*>(_context.last_extracted_string.c_str()));
				printf("[");
				for (size_t i = 0; i < _points->size(); i++) {
					printf("[");
					for (size_t j = 0; j < (*_points)[i]->size(); j++) {
						printf("%f", (*(*_points)[i])[j]);
						if (j < (*_points)[i]->size() - 1) {
							printf(", ");
						}
					}
					printf("]");
					if (i < _points->size() - 1) {
						printf(", ");
					}
				}
				printf("]\n");

				for (size_t i = 0; i < _points->size(); i++) {
					delete (*_points)[i];
					(*_points)[i] = nullptr;
				}
				delete _points;
				_points = new Array<Array<double>*>();
			}
			else if (_depth == depth::ShapeLevel && (_context.last_extracted_string == "\"LineString\"" || _context.last_extracted_string == "\"MultiLineString\"" || _context.last_extracted_string == "\"Polygon\"" || _context.last_extracted_string == "\"MultiPolygon\"")) {
				_context.has_item = true;
				printf("%s:\n", reinterpret_cast<const char*>(_context.last_extracted_string.c_str()));
				printf("[");
				for (size_t i = 0; i < _points->size(); i++) {
					printf("[");
					for (size_t j = 0; j < (*_points)[i]->size(); j++) {
						printf("%f", (*(*_points)[i])[j]);
						if (j < (*_points)[i]->size() - 1) {
							printf(", ");
						}
					}
					printf("]");
					if (i < _points->size() - 1) {
						printf(", ");
					}
				}
				printf("]\n");

				for (size_t i = 0; i < _points->size(); i++) {
					delete (*_points)[i];
					(*_points)[i] = nullptr;
				}
				delete _points;
				_points = new Array<Array<double>*>();
			}

			_context.last_status = Context::LastStatus::OK;
		}

		template<>
		void resolve_custom_visit<gjHandler::DepthOut>(CustomNode& node) {
			switch (_depth) {
			case CoordsLevel:
				_depth = PointsLevel;
				break;
			case PointsLevel:
				_depth = ShapeLevel;
				break;
			case ShapeLevel:
				_depth = OutsideLevel;
				break;
			default:
				break;
			}

			_context.last_status = Context::LastStatus::OK;
		}	




	};
}