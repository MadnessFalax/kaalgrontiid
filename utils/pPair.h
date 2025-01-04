#pragma once

namespace nspPair {
	
	template <class Tkey, class Tval>
	class pPair
	{

		using key_type = Tkey;
		using key_ptr = Tkey*;
		using key_ref = Tkey&;
		using value_type = Tval;
		using value_ref = Tval&;
		using value_ptr = Tval*;

		key_ptr _first = nullptr;
		value_ptr _second = nullptr;

	public:
		pPair() : _first(new key_type{}), _second(new value_type{}) {};

		pPair(key_ref key, value_ref value) : _first(new key_type(key)), _second(new value_type(value)) {};
		
		pPair(const pPair& other) : _first(new key_type(*other._first)), _second(new value_type(*other._second)) {};
		
		pPair(pPair&& other) noexcept : _first(other._first), _second(other._second) {
			other._first = nullptr;
			other._second = nullptr;
		};
		
		~pPair() {
			delete _first;
			_first = nullptr;
			delete _second;
			_second = nullptr;
		};

		pPair& operator=(const pPair& other) {
			if (this != &other) {
				delete _second;
				_second = new value_type(*other._second);
			}

			return *this;
		}

		pPair& operator=(pPair&& other) noexcept {
			if (this != &other) {
				delete _second;
				_second = other._second;
				other._second = nullptr;
			}

			return *this;
		}
		
		const Tkey& first() const {
			return *_first;
		}

		value_ref second() {
			return  *_second;
		}

	};

}
