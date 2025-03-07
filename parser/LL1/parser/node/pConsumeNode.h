#pragma once
#include "pParserNode.h"

namespace nspParser {
	template<typename enum_t>
	class pConsumeNode : public pParserNode<enum_t> {
		using Visitor = pBaseVisitor<pParserNode<enum_t>>;
		
		TokenPrototype<enum_t>* _proto = nullptr;
		
	public:
		pConsumeNode(TokenPrototype<enum_t>* proto) : _proto(proto) {

		}

		~pConsumeNode() {
			_proto = nullptr;
		}

		template<class visitor_t>
		void accept(Visitor* visitor) {

		}
	};
}