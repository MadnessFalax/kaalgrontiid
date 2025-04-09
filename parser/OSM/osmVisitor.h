#pragma once
#include "../LL1/parser/pParserVisitor.h"
#include "../LL1/parser/pErrorReporter.h"
#include "osmEnums.h"

namespace nsOSM {
	class osmVisitor : public nspParser::pParserVisitor<osmToken, osmRule, osmHandler, osmVisitor> {
		using String = nspString::pString;
		using Rule = nspParser::pRule<osmToken, osmRule>;
		template<class T>
		using Array = nspArray::pArray<T>;
		using pErrorReporter = nspParser::pErrorReporter;
		using ExtractType = nspParser::pExtractNode<osmToken, osmRule>::ExtractType;

		struct Point {
			double longitude = 0;
			double latitude = 0;
			double elevation = 0;
			size_t dim = 2;
			size_t id = 0;
			bool is_used = false;
		};

		struct Tag {
			String key = "";
			double num_value = 0;
			String text_value = "";
			bool key_set = false;
			bool value_set = false;
			ExtractType type = ExtractType::STRING;
		} last_tag = Tag{};

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
			case osmHandler::SetKey:
				resolve_custom_visit<osmHandler::SetKey>(node);
				break;
			case osmHandler::SetValue:
				resolve_custom_visit<osmHandler::SetValue>(node);
				break;
			case osmHandler::CommitTag:
				resolve_custom_visit<osmHandler::CommitTag>(node);
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
		void resolve_custom_visit<osmHandler::SetKey>(CustomNode& node) {
			last_tag.key = _context.last_extracted_string;
			last_tag.key_set = true;
			_context.last_status = Context::LastStatus::OK;
		}

		template<>
		void resolve_custom_visit<osmHandler::SetValue>(CustomNode& node) {
			last_tag.type = _context.last_extract_type;
			if (last_tag.type == ExtractType::NUMBER) {
				last_tag.num_value = _context.last_extracted_number;
				last_tag.value_set = true;
			}
			else {
				last_tag.text_value = _context.last_extracted_string;
				last_tag.value_set = true;
			}
			_context.last_status = Context::LastStatus::OK;
		}

		template<>
		void resolve_custom_visit<osmHandler::CommitTag>(CustomNode& node) {
			if (last_tag.key_set && last_tag.value_set) {
				if (last_tag.type == ExtractType::NUMBER && last_tag.key == "ele") {
					cur_point.elevation = last_tag.num_value;
					cur_point.dim = 3;
				}
			}

			last_tag = Tag{};

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
			cur_point = Point{};
			_context.last_status = Context::LastStatus::OK;
		}

		template<>
		void resolve_custom_visit<osmHandler::CommitShape>(CustomNode& node) {
			auto shape_dim = 2;

			auto p_size = _points->size();
			if (p_size == 0) {
				_context.last_status = Context::LastStatus::FAIL;
				return;
			}

			auto* first_point = (*_points)[0];
			auto* last_point = (*_points)[p_size - 1];

			if (first_point->latitude != last_point->latitude || first_point->longitude != last_point->longitude) {
				_context.item_type = DataShape::DS_LINESTRING;
			}
			else {
				_context.item_type = DataShape::DS_POLYGON;
			}

			cSpaceDescriptor* sp_desc = _space_desc_3d;

			cNTuple** tuples = new cNTuple * [p_size];
			for (size_t i = 0; i < p_size; i++) {
				auto dim = (*(*_points)[i]).dim;
				if (dim == 3) {
					shape_dim = 3;
				}
				tuples[i] = new cNTuple(sp_desc);
				tuples[i]->SetValue((unsigned int)0, (*(*_points)[i]).longitude, nullptr);
				tuples[i]->SetValue((unsigned int)1, (*(*_points)[i]).latitude, nullptr);
				if (dim == 3) {
					tuples[i]->SetValue((unsigned int)2, (*(*_points)[i]).elevation, nullptr);
				}
				else {
					tuples[i]->SetValue((unsigned int)2, 0.0, nullptr);
				}
			}

			auto* shape_desc = _space_desc_3d;

			_context.item = cDataShape<cNTuple>::CreateDataShape(_context.item_type, tuples, p_size, shape_desc);
			_context.last_item_sd = shape_desc;

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

				_context.dimension = point->dim;

				auto* desc = _space_desc_3d;
				_context.last_item_sd = desc;

				_context.item_type = DataShape::DS_POINT;

				cNTuple* tuple = new cNTuple(desc);
				tuple->SetValue((unsigned int)0, point->longitude, nullptr);
				tuple->SetValue((unsigned int)1, point->latitude, nullptr);
				if (point->dim == 3) {
					tuple->SetValue((unsigned int)2, point->elevation, nullptr);
				}
				else {
					tuple->SetValue((unsigned int)2, 0.0, nullptr);
				}

				auto** pt_arr = new cNTuple*[1];
				pt_arr[0] = tuple;

				_context.item = cDataShape<cNTuple>::CreateDataShape(DataShape::DS_POINT, pt_arr, 1, desc);

				tuple = nullptr;
				pt_arr = nullptr;
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