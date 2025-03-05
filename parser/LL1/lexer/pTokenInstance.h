#pragma once
#include "pTokenPrototype.h"

namespace nspLexer {
	template<class enum_t, pPrototypeKind proto_t = pPrototypeKind::DEFAULT>
	class pTokenInstance {
	public:
		pTokenPrototype<enum_t, proto_t>* _prototype;
		String _value;
		size_t _position;

		// doesn't own prototype
		pTokenInstance(pTokenPrototype<enum_t, proto_t>* prototype, String value, size_t position) : _prototype(prototype), _value(value), _position(position) {};
		~pTokenInstance() {
			_prototype = nullptr;
		}

		void print() {
			printf("%i, %s\n", _prototype ? _prototype->_type_id : 0, _value.c_str());
		}

	};
}