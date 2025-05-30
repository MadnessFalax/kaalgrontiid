#pragma once
#include "pParserNode.h"

namespace nspParser {
	template<typename enum_t, typename enum_r>
	class pConsumeNode : public pParserNode<enum_t, enum_r> {
		
		enum_t _token_type;
		bool _check_value = false;
		String _value = "";
		bool _consume_any = false;

	public:
		pConsumeNode() : _consume_any(true) { }

		pConsumeNode(enum_t token_type) : _token_type(token_type) {

		}

		pConsumeNode(enum_t token_type, String value) : _token_type(token_type), _value(value), _check_value(true) {}

		~pConsumeNode() {
		}

		template<class visitor_t>
		void accept(pParserNode<enum_t, enum_r>::Visitor<visitor_t>* visitor) {
			visitor->visit(*this);
		}

		template<class visitor_t>
		void accept(pParserNode<enum_t, enum_r>::Visitor<visitor_t>& visitor) {
			visitor.visit(*this);
		}

		const bool& check_value() {
			return _check_value;
		}

		const String& get_value() {
			return _value;
		}

		const enum_t& get_proto() {
			return _token_type;
		}

		const bool& consume_any() {
			return _consume_any;
		}

		char get_type() override {
			return 'c';
		}
	};
}