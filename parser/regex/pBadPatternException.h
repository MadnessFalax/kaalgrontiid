#pragma once
#include "../../abstract/pException.h"

class pBadPatternException : protected pException {
public:
	pBadPatternException() {
		_message = "Regex pattern is incorrect!";
	}
};