#pragma once
#include "pTokenPrototype.h"

namespace nspLexer {
	template<class enum_t, pPrototypeKind proto_t = pPrototypeKind::DEFAULT>
	class pTokenInstance {
		pTokenPrototype<enum_t, proto_t>* _prototype = nullptr;
		String _value = "";
		size_t _position = 0;
	public:
		pTokenInstance() {};
		// doesn't own prototype
		pTokenInstance(pTokenPrototype<enum_t, proto_t>* prototype, String value, size_t position) : _prototype(prototype), _value(value), _position(position) {};
		~pTokenInstance() {
			_prototype = nullptr;
		}

		void print() {
			printf("%s, %s\n", _prototype ? reinterpret_cast<const char*>(_prototype->get_name().c_str()) : "UNNAMED_TOKEN", _value.c_str());
		}

		const size_t& get_position() const { return _position; }	
		String& get_value() { return _value; }	
		pTokenPrototype<enum_t, proto_t>* get_prototype() const { return _prototype; }
	};
}