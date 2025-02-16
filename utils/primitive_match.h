#pragma once
#include "../container/pString.h"

namespace nspPattern {
	class pPattern {
		using String = nspString::pString;
		String* _pattern = nullptr;

	public:
		pPattern() = delete;
		pPattern(const String& pattern) : _pattern(new String(pattern)) {};

		pPattern(String& pattern) : _pattern(new String(pattern)) {};

		pPattern(pPattern& other) : _pattern(new String(*other._pattern)) {};

		pPattern(pPattern&& other) noexcept : _pattern(other._pattern) {
			other._pattern = nullptr;
		}

		~pPattern() {
			delete _pattern;
			_pattern = nullptr;
		}

		pPattern& operator=(pPattern& other) {
			if (this != &other) {
				delete _pattern;
				_pattern = nullptr;
				_pattern = new String(*other._pattern);
			}

			return *this;
		}

		pPattern& operator=(pPattern&& other) noexcept {
			if (this != &other) {
				delete _pattern;
				_pattern = nullptr;
				_pattern = other._pattern;
				other._pattern = nullptr;
			}

			return *this;
		}

		bool match(String& input) {
			return input == *_pattern;
		}

		bool match(String* input) {
			return *input == *_pattern;
		}
	};
}