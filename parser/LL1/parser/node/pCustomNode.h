#pragma once
#include "pParserNode.h"

namespace nspParser {
	template<typename enum_t, typename enum_r, typename enum_c>
	class pCustomNode : public pParserNode<enum_t, enum_r> {
		enum_c _handler_id;

	public:
		pCustomNode(enum_c handler_id) : _handler_id(handler_id) {
			// I believe this could have been done better using templates, unfortunatelly deadline is closing in
		}

		~pCustomNode() {

		}

		template<class visitor_t>
		void accept(pParserNode<enum_t, enum_r>::Visitor<visitor_t>* visitor) {
			visitor->visit(*this);
		}

		template<class visitor_t>
		void accept(pParserNode<enum_t, enum_r>::Visitor<visitor_t>& visitor) {
			visitor.visit(*this);
		}

		char get_type() override {
			return 'u';
		}

		enum_c get_handler_id() {
			return _handler_id;
		}
	};
}