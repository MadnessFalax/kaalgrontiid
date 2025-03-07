#pragma once

namespace nspString {

	void memcpy(unsigned char* dest, const unsigned char* src, size_t count) {
		for (size_t i = 0; i < count; i++) {
			dest[i] = src[i];
		}
	}

	size_t strlen(const unsigned char* str) {
		size_t count = 0;
		while (str[count] != '\0') {
			count++;
		}
		return count;
	}

	size_t strcmp(const unsigned char* l, const unsigned char* r) {
		size_t l_count = strlen(l);
		size_t r_count = strlen(r);
		if (l_count != r_count) {
			return l_count - r_count;
		}
		else {
			for (size_t i = 0; i < l_count; i++) {
				if (l[i] != r[i]) {
					return l[i] - r[i];
				}
			}
		}
		return 0;
	}
}