#pragma once
#include "../../abstract/pException.h"

class pOutOfRangeException : protected pException {
public:
	pOutOfRangeException() {
		_message = "Index is out of bounds!";
	};
};