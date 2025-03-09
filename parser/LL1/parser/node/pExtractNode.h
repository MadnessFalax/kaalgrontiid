#pragma once
#include "pParserNode.h"
#include "../../../regex/pRegex.h"
#include <cstdio>

namespace nspParser {
	template<typename enum_t, typename enum_r>
	class pExtractNode : public pParserNode<enum_t, enum_r> {
	public:
		enum ExtractType {
			NUMBER,
			STRING
		};

	private:
		using Regex = nspRegex::pRegex;
		enum_t _token_type = enum_t{};
		bool _restricted_type = false;
		Regex* _pattern = nullptr;
		bool _restricted_pattern = false;
		ExtractType _extract_type = ExtractType::STRING;
	
	public:
		pExtractNode() {}

		pExtractNode(enum_t token_type, ExtractType extract_type = ExtractType::STRING) : _token_type(token_type), _restricted_type(true), _extract_type(extract_type) {}

		// for regex extraction a string is being extracted
		pExtractNode(enum_t token_type, String pattern) : _token_type(token_type), _restricted_type(true), _restricted_pattern(true) {
			_pattern = new Regex(pattern);
			if (!_pattern->compile()) {
				printf("Failed to compile regex of pExtractNode. Token ID: %i, Pattern: %s\n", token_type, reinterpret_cast<const char*>(pattern.c_str()));
				delete _pattern;
				_pattern = nullptr;
			}
		}

		~pExtractNode() {
			delete _pattern;
			_pattern = nullptr;
		}

		template<class visitor_t>
		void accept(pParserNode<enum_t, enum_r>::Visitor<visitor_t>* visitor) {
			visitor->visit(*this);
		}

		template<class visitor_t>
		void accept(pParserNode<enum_t, enum_r>::Visitor<visitor_t>& visitor) {
			visitor.visit(*this);
		}

		enum_t get_token_type() const {
			return _token_type;
		}

		bool is_restricted_type() const {
			return _restricted_type;
		}

		Regex* get_pattern() const {
			return _pattern;
		}

		bool is_restricted_pattern() const {
			return _restricted_pattern;
		}

		ExtractType get_extract_type() const {
			return _extract_type;
		}

		char get_type() override {
			return 'e';
		}
	};
}