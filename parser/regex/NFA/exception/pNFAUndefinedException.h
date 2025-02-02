#pragma once
#include "../../../../abstract/pException.h"

class pNFAUndefinedException : protected pException {
public:
	pNFAUndefinedException() {
		_message = "Automaton states are not properly defined!";
	}
};