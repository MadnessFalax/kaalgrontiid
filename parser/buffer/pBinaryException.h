#pragma once
#include "../../abstract/pException.h"

namespace nspFile {
	class pBinaryException : protected pException {
	public:
		pBinaryException() {
			_message = "An error occured while handling binary data.";
		};
	};
}