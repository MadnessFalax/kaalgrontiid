#pragma once
#include "../container/pString.h"

namespace nspMatch {
	using String = nspString::pString;

	bool boundary_match(const String& input, const String& boundary) {
		auto bound_len = boundary.length();
		auto input_len = input.length();

		if (input_len < bound_len) {
			return false;
		}

		if (input_len == bound_len) {
			return input == boundary;
		}

		auto offset = input_len - bound_len;

		for (size_t i = 0; i < bound_len; i++) {
			if (input[i + offset] != boundary[i]) {
				return false;
			}
		}

		return true;
	}

	bool primitive_match(const String& input, const String& pattern) {
		if (input == pattern) {
			return true;
		}

		return false;
	}
}