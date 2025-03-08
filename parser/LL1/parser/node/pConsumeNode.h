#pragma once
#include "pParserNode.h"

namespace nspParser {
	template<typename enum_t, typename enum_r>
	class pConsumeNode : public pParserNode<enum_t, enum_r> {
		using Visitor = pBaseVisitor<pParserNode<enum_t, enum_r>>;
		
		enum_t _proto;
		bool _check_value = false;
		String _value = "";
		
	public:
		pConsumeNode(enum_t proto) : _proto(proto) {

		}

		pConsumeNode(enum_t proto, String value) : _proto(proto), _value(value), _check_value(true) {}

		~pConsumeNode() {
			_proto = nullptr;
		}

		template<class visitor_t>
		void accept(Visitor* visitor) {

		}
	};
}