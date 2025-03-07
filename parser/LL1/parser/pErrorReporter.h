#pragma once
#include "../../../container/pString.h"

namespace nspParser {
	using String = nspString::pString;

	class pErrorReporter {
	public:
		static void report(size_t position, String value, String token_name, String message = "") {
			String empty_str = "";
			if (message != empty_str) {
				printf("Error at '%s'@%d with value: %s Message:\n%s\n", reinterpret_cast<const char*>(token_name.c_str()), position, reinterpret_cast<const char*>(value.c_str()), reinterpret_cast<const char*>(message.c_str()));
			}
			else {
				printf("Error at '%s'@%d with value: %s\n", reinterpret_cast<const char*>(token_name.c_str()), position, reinterpret_cast<const char*>(value.c_str()));
			}
		}
	}
}