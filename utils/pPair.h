#pragma once

template<class Tx, class Ty>
class pPair
{
public:

	const Tx first;
	const Ty second;

	pPair(Tx first, Ty second) {
		this->first = first;
		this->second = second;
	}

	pPair(pPair& other) {
		this->first = other.first;
		this->second = other.second;
	}

	pPair(pPair&& other) {
		this->first = other.first;
		this->second = other.second;
	}

	~pPair() {

	}

	void operator==(pPair& other) {
		if (this->first == other.first && this->second == other.second)
			return true;
		else
			return false;
	}

	void operator=(pPair& other) {
		this->first = other.first;
		this->second = other.second;
	}

	void operator=(pPair&& other) {
		this->first = other.first;
		this->second = other.second;
	}
};

