#pragma once
#include "../../../abstract/pBaseVisitor.h"
#include "../lexer/pTokenInstance.h"
#include "../lexer/pLexer.h"
#include "node/pParserNode.h"
#include "node/pConsumeNode.h"
#include "node/pExtractNode.h"
#include "node/pForwardNode.h"
#include "node/pCustomNode.h"
#include "pErrorReporter.h"

namespace nspParser {
	template<typename enum_t, typename enum_r>
	class pParserVisitor : pBaseVisitor<pParserNode<enum_t, enum_r>> {
	protected:
		using TokenInstance = nspLexer::pTokenInstance<enum_t, nspLexer::PrototypeKind::REGEX>;
		using Lexer = nspLexer::pLexer<enum_t>;
		struct Context {
			enum_r current_rule = enum_r{};
			enum_t current_token = enum_t{};
			TokenInstance* current_instance = nullptr;
			enum last_status {
				OK,
				FAIL
			} last_status = FAIL;

			Context() {

			}

			~Context() {
				current_instance = nullptr;
			}
		};

		Context _context;
		Lexer* _lexer = nullptr;

		bool _consume() {
			_context.current_instance = _lexer->get_token();
			return true;
		}

		bool _consume(enum_t token_type) {
			if (_context.current_instance->prototype()->get_id() == token_type) {
				_context.current_instance = _lexer->get_token();
				return true;
			}
			else {
				pErrorReporter::report_rule(_context.current_rule, _context.current_instance->prototype()->get_name());
				pErrorReporter::report_token(_context.current_instance->get_position(), _context.current_instance->get_value(), _context->current_instance->get_prototype()->get_name(), sprintf("Got unexpected token. Expected: %s\n", _lexer->get_token_name(token_type)));
				return false;
			}
		}

		TokenInstance* _lookahead() {

		}

	public:
		pParserVisitor(Lexer* lexer) : _lexer(lexer) {

		}

		virtual ~pParserVisitor() {
			_lexer = nullptr;
		}

		Context& get_context() {
			return _context;
		}

		void resolve_visit(pParserNode<enum_t, enum_r>& node) {
			printf("You have implemented this visitor wrong, you moron!");
		}

		void resolve_visit(pConsumeNode<enum_t, enum_r>& node) {
			if (node.check_value()) {
				if (_consume(node.get_proto().get_id(), node.get_value())) {
					_context.last_status = Context::last_status::OK;
				}
			}
			else {
				if (_consume(node.get_proto().get_id())) {
					_context.last_status = Context::last_status::OK;
				}
			}
			_context.last_status = Context::last_status::FAIL;
			return;
		}
	};
}