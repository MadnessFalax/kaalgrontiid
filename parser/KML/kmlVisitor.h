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

		size_t _dimension = 0;
		Array<double>* _point = nullptr;
		Array<Array<double>*>* _points = nullptr;

	public:
		kmlVisitor(Lexer* lexer, Array<Rule*>* rules, Stack* stack) : nspParser::pParserVisitor<kmlToken, kmlRule, kmlHandler, kmlVisitor>(lexer, rules, stack) {
			_point = new Array<double>();
			_points = new Array<Array<double>*>();
		}

		~kmlVisitor() {
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
			case kmlHandler::MakeNegative:
				resolve_custom_visit<kmlHandler::MakeNegative>(node);
				break;
			case kmlHandler::BufferNumber:
				resolve_custom_visit<kmlHandler::BufferNumber>(node);
				break;
			case kmlHandler::BufferPoint:
				resolve_custom_visit<kmlHandler::BufferPoint>(node);
				break;
			case kmlHandler::CommitShape:
				resolve_custom_visit<kmlHandler::CommitShape>(node);
				break;
			}
		}

		template<kmlHandler handler_id>
		void resolve_custom_visit(CustomNode& node) {
			printf("You have implemented this visitor wrong, you moron!");
		}

		template<>
		void resolve_custom_visit<kmlHandler::MakeNegative>(CustomNode& node) {
			_context.last_extracted_number = -_context.last_extracted_number;

			_context.last_status = Context::LastStatus::OK;
		}

		template<>
		void resolve_custom_visit<kmlHandler::BufferNumber>(CustomNode& node) {
			_point->push_back(_context.last_extracted_number);

			_context.last_status = Context::LastStatus::OK;
		}

		template<>
		void resolve_custom_visit<kmlHandler::BufferPoint>(CustomNode& node) {
			_dimension = _point->size();
			_points->push_back(_point);
			_point = new Array<double>();

			_context.last_status = Context::LastStatus::OK;
		}

		template<>
		void resolve_custom_visit<kmlHandler::CommitShape>(CustomNode& node) {
			size_t p_size = _points->size();

			if (p_size == 1) {
				_context.item_type = DataShape::DS_POINT;
			}
			else {
				_context.item_type = DataShape::DS_POLYGON;
				auto* first_point = (*_points)[0];
				auto* last_point = (*_points)[p_size - 1];

				for (size_t i = 0; i < _context.dimension; i++) {
					if ((*first_point)[i] != (*last_point)[i]) {
						_context.item_type = DataShape::DS_LINESTRING;
						break;
					}
				}
			}

			switch (_context.item_type) {
			case DataShape::DS_POINT:
				_context.dimension = _dimension;
				_context.point = cDataShape<cTuple>::CreateDataShape(_context.item_type, );
				break;
			case DataShape::DS_SPHERE:
				_context.dimension = _dimension;
				_context.sphere = static_cast<cSphere<cTuple>*>(cDataShape<cTuple>::CreateDataShape());
				break;
			case DataShape::DS_LINESTRING:
				_context.dimension = _dimension;
				_context.sphere = static_cast<cSphere<cTuple>*>(cDataShape<cTuple>::CreateDataShape());
				break;
			case DataShape::DS_POLYGON:
				_context.dimension = _dimension;
				_context.sphere = static_cast<cSphere<cTuple>*>(cDataShape<cTuple>::CreateDataShape());
				break;
			}


			for (size_t i = 0; i < p_size; i++) {
				Array<double>* point = (*_points)[i];
				size_t point_size = point->size();
				for (size_t j = 0; j < point_size; j++) {
				}
			}
			
			for (size_t i = 0; i < p_size; i++) {
				delete (*_points)[i];
				(*_points)[i] = nullptr;
			}
			delete _points;
			_points = new Array<Array<double>*>();


			_context.has_item = true;
			_context.last_status = Context::LastStatus::OK;
		}
	};
}