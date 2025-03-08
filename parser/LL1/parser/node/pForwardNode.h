#pragma once
#include "pParserNode.h"

namespace nspParser {
	template<typename enum_t, typename enum_r>
	class pForwardNode : public pParserNode<enum_t, enum_r> {
		enum_r _forward_to = enum_r{};
		

	public:
		pForwardNode(enum_r forward_to) : _forward_to(forward_to) {}

		template<class visitor_t>
		void accept(pParserNode<enum_t, enum_r>::Visitor<visitor_t>* visitor) {
			visitor->visit(*this);
		}

		template<class visitor_t>
		void accept(pParserNode<enum_t, enum_r>::Visitor<visitor_t>& visitor) {
			visitor.visit(*this);
		}

	};
}