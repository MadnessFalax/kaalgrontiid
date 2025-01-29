#pragma once


class pException {

protected:
	const char* _message = "An unspecified exception has been thrown!";

public:
	pException() {}

	const char* what() const {
		return _message;
	}
};