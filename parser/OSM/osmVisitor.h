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

		struct Point {
			double longitude = 0;
			double latitude = 0;
			size_t id = 0;
			bool is_used = false;
		};

		size_t cur_ref = 0;
		Point cur_point = Point{};
		Array<Point*>* _points = nullptr;	
		Map<size_t, Point, unsigned short> _nodes;		// Point owner

	public:
		osmVisitor(Lexer* lexer, Array<Rule*>* rules, Stack* stack, Rule* ending_rule = nullptr) : nspParser::pParserVisitor<osmToken, osmRule, osmHandler, osmVisitor>(lexer, rules, stack, ending_rule) {
			_points = new Array<Point*>();
		}

		~osmVisitor() {
			if (_points) {
				size_t p_size = _points->size();
				for (size_t i = 0; i < p_size; i++) {
					(*_points)[i] = nullptr;
				}
				delete _points;
				_points = nullptr;
			}
		}

		void custom_visit_root(CustomNode& node) override {
			switch (node.get_handler_id()) {
			case osmHandler::ChangeSign:
				resolve_custom_visit<osmHandler::ChangeSign>(node);
				break;
			case osmHandler::SetLongitude:
				resolve_custom_visit<osmHandler::SetLongitude>(node);
				break;
			case osmHandler::SetLatitude:
				resolve_custom_visit<osmHandler::SetLatitude>(node);
				break;
			case osmHandler::SetId:
				resolve_custom_visit<osmHandler::SetId>(node);
				break;
			case osmHandler::SetRef:
				resolve_custom_visit<osmHandler::SetRef>(node);
				break;
			case osmHandler::PointsToArray:
				resolve_custom_visit<osmHandler::PointsToArray>(node);
				break;
			case osmHandler::CommitPoint:
				resolve_custom_visit<osmHandler::CommitPoint>(node);
				break;
			case osmHandler::CommitShape:
				resolve_custom_visit<osmHandler::CommitShape>(node);
				break;
			case osmHandler::DisposePoints:
				resolve_custom_visit<osmHandler::DisposePoints>(node);
				break;
			default:
				break;
			}
		}

		template<osmHandler handler_id>
		void resolve_custom_visit(CustomNode& node) {
			printf("You have implemented this visitor wrong, you moron!");
		}

		template<>
		void resolve_custom_visit<osmHandler::ChangeSign>(CustomNode& node) {
			_context.last_extracted_number = -_context.last_extracted_number;
			_context.last_status = Context::LastStatus::OK;
		}

		template<>
		void resolve_custom_visit<osmHandler::SetLongitude>(CustomNode& node) {
			cur_point.longitude = _context.last_extracted_number;
			_context.last_status = Context::LastStatus::OK;
		}

		template<>
		void resolve_custom_visit<osmHandler::SetLatitude>(CustomNode& node) {
			cur_point.latitude = _context.last_extracted_number;
			_context.last_status = Context::LastStatus::OK;
		}

		template<>
		void resolve_custom_visit<osmHandler::SetId>(CustomNode& node) {
			cur_point.id = (size_t)_context.last_extracted_number;
			_context.last_status = Context::LastStatus::OK;
		}

		template<>
		void resolve_custom_visit<osmHandler::SetRef>(CustomNode& node) {
			cur_ref = (size_t)_context.last_extracted_number;
			_points->push_back(&_nodes[cur_ref]);
			_nodes[cur_ref].is_used = true;
			_context.last_status = Context::LastStatus::OK;
		}

		template<>
		void resolve_custom_visit<osmHandler::PointsToArray>(CustomNode& node) {
			for (auto& node : _nodes) {
				if (node.second().is_used == false) {
					_points->push_back(&node.second());
				}
			}
			_context.last_status = Context::LastStatus::OK;
		}

		template<>
		void resolve_custom_visit<osmHandler::CommitPoint>(CustomNode& node) {
			_nodes[cur_point.id] = cur_point;
			_context.last_status = Context::LastStatus::OK;
		}

		template<>
		void resolve_custom_visit<osmHandler::CommitShape>(CustomNode& node) {
			_context.dimension = 2;
			
			auto* desc = _space_desc_2d;
			_context.last_item_sd = desc;

			auto p_size = _points->size();
			auto* first_point = (*_points)[0];
			auto* last_point = (*_points)[p_size - 1];

			if (first_point->latitude != last_point->latitude || first_point->longitude != last_point->longitude) {
				_context.item_type = DataShape::DS_LINESTRING;
			}
			else {
				_context.item_type = DataShape::DS_POLYGON;
			}

			cNTuple** tuples = new cNTuple * [p_size];
			for (size_t i = 0; i < p_size; i++) {
				tuples[i] = new cNTuple(desc);
				tuples[i]->SetValue((unsigned int)0, (*(*_points)[i]).longitude, desc);
				tuples[i]->SetValue((unsigned int)0, (*(*_points)[i]).latitude, desc);
			}

			_context.item = cDataShape<cNTuple>::CreateDataShape(_context.item_type, tuples, p_size, desc);

			tuples = nullptr;

			for (size_t i = 0; i < p_size; i++) {
				(*_points)[i] = nullptr;
			}
			delete _points;
			_points = new Array<Point*>();

			_context.has_item = true;
			_context.last_status = Context::LastStatus::OK;
		}

		template<>
		void resolve_custom_visit<osmHandler::DisposePoints>(CustomNode& node) {
			Point* point = nullptr;
			if (_points->size() != 0) {
				point = (*_points)--;

				_context.dimension = 2;

				auto* desc = _space_desc_2d;
				_context.last_item_sd = desc;

				_context.item_type = DataShape::DS_POINT;

				cNTuple* tuple = new cNTuple(desc);
				tuple->SetValue((unsigned int)0, point->longitude, desc);
				tuple->SetValue((unsigned int)1, point->latitude, desc);
			
				_context.item = tuple;

				tuple = nullptr;

				point = nullptr;

				_context.has_item = true;

				_stack->register_rule(_rule_map[osmRule::FinalSequence]);

				_context.last_status = Context::LastStatus::OK;
			}
			else {
				delete _points;
				_points = nullptr;

				_context.end = true;
				_context.last_status = Context::LastStatus::OK;
			}
		}
	};
}