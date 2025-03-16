#pragma once
#include "../../../abstract/pException.h"

class pEpsilonException : protected pException {
public:
	pEpsilonException() {
		_message = "Rule can only have one empty sequence. This cannot be the first sequence.";
	};
};