#pragma once
#include "../../../abstract/pBaseVisitor.h"
#include "../lexer/pTokenInstance.h"
#include "node/pParserNode.h"

namespace nspParser {
	template<typename enum_t, typename enum_r>
	class pParserVisitor : pBaseVisitor<pParserNode<enum_t, enum_r>> {
		using TokenInstance = nspLexer::pTokenInstance<enum_t, nspLexer::PrototypeKind::REGEX>;
		struct Context {
			enum_r current_rule = enum_r{};
			enum_t current_token = enum_t{};
			TokenInstance* current_instance = nullptr;

			Context() {

			}

			~Context() {
				current_instance = nullptr;
			}
		};

		Context _context;

	public:
		pParserVisitor() {

		}

		Context& get_context() {
			return _context;
		}
	};
}