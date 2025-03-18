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
			auto current_dim = _point->size();
			if (current_dim > _context.dimension) {
				_context.dimension = current_dim;
			}

			_points->push_back(_point);
			_point = new Array<double>();

			_context.last_status = Context::LastStatus::OK;
		}

		template<>
		void resolve_custom_visit<kmlHandler::CommitShape>(CustomNode& node) {
			size_t p_size = _points->size();

			if (p_size == 1) {
				_context.item_type = DataShape::DS_SPHERE;
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

			cSpaceDescriptor* desc = _space_desc_2d;
			if (_context.dimension == 3) {
				desc = _space_desc_3d;
			}

			_context.last_item_sd = desc;

			cNTuple** tuples = new cNTuple * [p_size];
			for (size_t i = 0; i < p_size; i++) {
				tuples[i] = new cNTuple(desc);
				for (size_t j = 0; j < _context.dimension; j++) {
					tuples[i]->SetValue((unsigned int)j, (int)(*(*_points)[i])[j], desc);
				}
			}

			_context.item = cDataShape<cNTuple>::CreateDataShape(_context.item_type, tuples, (unsigned int) _points->size(), desc);
			
			tuples = nullptr;

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