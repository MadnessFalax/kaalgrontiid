#pragma once
#include "../../../abstract/pException.h"

class pRegexParserException : public pException {
public:
	pRegexParserException() {
		_message = "An error occured while parsing regex!";
	}

	pRegexParserException(const char* message) {
		_message = message;
	}
};