#pragma once
#include <type_traits>
#include "../../../container/pString.h"
#include "../../regex/pRegex.h"
#include "../../../utils/match.h"
#include "pPrototypeKind.h"

namespace nspLexer {
	
	using String = nspString::pString;
	using Regex = nspRegex::pRegex;
	using PrototypeKind = pPrototypeKind;

	template<typename enum_t, PrototypeKind t = PrototypeKind::DEFAULT, typename = std::enable_if_t<std::is_enum_v<enum_t>>>
	class pTokenPrototype {
		enum_t _type_id = enum_t{};
		String _pattern = "";
		Regex* _regex = nullptr;

	public:

		template<PrototypeKind class_t = t>
		pTokenPrototype(enum_t type_id, std::enable_if_t<class_t != PrototypeKind::REGEX, String> pattern) : _type_id(type_id), _pattern(pattern) {};

		template<PrototypeKind class_t = t>
		pTokenPrototype(enum_t type_id, std::enable_if_t<class_t == PrototypeKind::REGEX, String> pattern) : _type_id(type_id), _pattern(pattern) {
			_regex = new Regex(pattern);
			_regex->compile();
		};

		template<PrototypeKind class_t = t>
		bool match(std::enable_if_t<class_t == PrototypeKind::DEFAULT, String&> input) {
			return true;
		}

		template<PrototypeKind class_t = t>
		bool match(std::enable_if_t<class_t == PrototypeKind::REGEX, String&> input) {
			return _regex->match(input);
		}

		template<PrototypeKind class_t = t>
		bool match(std::enable_if_t<class_t == PrototypeKind::PRIMITIVE, String> input) {
			return nspMatch::primitive_match(input, _pattern);
		}

		template<PrototypeKind class_t = t>
		bool match(std::enable_if_t<class_t == PrototypeKind::BOUNDARY, String> input) {
			return nspMatch::boundary_match(input, _pattern);
		}

		~pTokenPrototype() {
			delete _regex;
			_regex = nullptr;
		}
	};
}