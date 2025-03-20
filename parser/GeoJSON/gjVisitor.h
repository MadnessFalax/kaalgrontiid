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

		Array<double>* _point = nullptr;
		Array<Array<double>*>* _points = nullptr;
		enum depth {
			CoordsLevel,
			PointsLevel,
			ShapeLevel,
			OutsideLevel
		} _depth = OutsideLevel;

	public:
		gjVisitor(Lexer* lexer, Array<Rule*>* rules, Stack* stack) : nspParser::pParserVisitor<gjToken, gjRule, gjHandler, gjVisitor>(lexer, rules, stack) {
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
			_depth = depth::CoordsLevel;

			_context.last_status = Context::LastStatus::OK;
		}

		template<>
		void resolve_custom_visit<gjHandler::BufferPoint>(CustomNode& node) {
			if (_context.dimension < _point->size()) {
				_context.dimension = _point->size();
			}
			_points->push_back(_point);
			_point = new Array<double>();


			_context.last_status = Context::LastStatus::OK;
		}

		template<>
		void resolve_custom_visit<gjHandler::CommitShape>(CustomNode& node) {
			size_t p_size = _points->size();

			cSpaceDescriptor* desc = _space_desc_2d;
			if (_context.dimension == 3) {
				desc = _space_desc_3d;
			}

			_context.last_item_sd = desc;

			if (_depth == depth::PointsLevel && (_context.last_extracted_string == "\"Point\"" || _context.last_extracted_string == "\"MultiPoint\"")) {
				_context.item_type = DataShape::DS_POINT;

				cNTuple* tuple = new cNTuple(desc);
				for (size_t i = 0; i < _context.dimension; i++) {
					tuple->SetValue((unsigned int)i, (*(*_points)[0])[i], desc);
				}

				_context.item = tuple;

				tuple = nullptr;

				for (size_t i = 0; i < p_size; i++) {
					delete (*_points)[i];
					(*_points)[i] = nullptr;
				}
				delete _points;
				_points = new Array<Array<double>*>();

				_context.dimension = 0;
				_context.has_item = true;
			}
			else if (_depth == depth::PointsLevel && (_context.last_extracted_string == "\"LineString\"" || _context.last_extracted_string == "\"MultiLineString\"" || _context.last_extracted_string == "\"Polygon\"" || _context.last_extracted_string == "\"MultiPolygon\"")) {
				_context.item_type = DataShape::DS_POLYGON;
				auto* first_point = (*_points)[0];
				auto* last_point = (*_points)[p_size - 1];

				for (size_t i = 0; i < _context.dimension; i++) {
					if ((*first_point)[i] != (*last_point)[i]) {
						_context.item_type = DataShape::DS_LINESTRING;
						break;
					}
				}

				cNTuple** tuples = new cNTuple * [p_size];
				for (size_t i = 0; i < p_size; i++) {
					tuples[i] = new cNTuple(desc);
					for (size_t j = 0; j < _context.dimension; j++) {
						tuples[i]->SetValue((unsigned int)j, (*(*_points)[i])[j], desc);
					}
				}

				_context.item = cDataShape<cNTuple>::CreateDataShape(_context.item_type, tuples, (unsigned int)_points->size(), desc);

				tuples = nullptr;

				for (size_t i = 0; i < p_size; i++) {
					delete (*_points)[i];
					(*_points)[i] = nullptr;
				}
				delete _points;
				_points = new Array<Array<double>*>();

				_context.dimension = 0;
				_context.has_item = true;
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