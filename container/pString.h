#pragma once

#include "pArray.h"
#include "cstdlib"
#include "cstring"
#include "../abstract/pHashable.h"
#include "../utils/exception/pOutOfRangeException.h"
#include "../utils/string.h"

// REDO to hold null terminator

namespace nspString {

	using Hashable = nspHashable::pHashable;

	constexpr size_t START_SIZE = 16;
	constexpr size_t LOW_RESIZE = 10;		// higher = less chance that a reallocation will be needed after initial alloc, lower = less unused space after initial allocation
	constexpr size_t INCR_FACTOR = 2;		// higher = reallocating less often, lower = less unused space after reallocation

	// stores trailing null terminator
	// operator+ is not implemented so that new heap objects arent constructed unintentionally
	class pString : public Hashable
	{
		unsigned char* _data = nullptr;			// holds trailing '\0'
		size_t _size = START_SIZE;			// total bytes allocated ('\0' included)
		size_t _count = 0;			// string unsigned char length ('\0' NOT included)

	public:
		pString() : _data(new unsigned char[START_SIZE]) {
			memset(_data, '\0', _size);
		}

		pString(size_t start_size_override) : _size(start_size_override) {
			_data = new unsigned char[_size];
			memset(_data, '\0', _size);
		}

		pString(const pString& other) : _size(other._size), _count(other._count) {
			
			_data = new unsigned char[_size];
			memset(_data, '\0', _size);
			memcpy(_data, other._data, _count);
		}

		pString(pString&& other) noexcept : _size(other._size), _count(other._count), _data(other._data) {
			
			other._data = nullptr;
		}

		// creates copy, delete heap allocated input manually
		pString(const unsigned char* other) {
			if (other != nullptr) {
				_count = strlen(other);
			}
			else {
				_count = 0;
			}
			_size = _count + START_SIZE;
			_data = new unsigned char[_size];

			memset(this->_data, '\0', this->_size);

			if (other != nullptr) {
				memcpy(this->_data, other, this->_count);
			}
		}

		pString(const char* other) {
			auto* cast_other = reinterpret_cast<const unsigned char*>(other);
			if (other != nullptr) {
				_count = strlen(cast_other);
			}
			else {
				_count = 0;
			}
			_size = _count + START_SIZE;
			_data = new unsigned char[_size];
			memset(this->_data, '\0', this->_size);
			if (cast_other != nullptr) {
				memcpy(this->_data, cast_other, _count);
			}
		}

		~pString() {
			delete[] _data;
			_data = nullptr;
		}

		void operator= (const pString& other) noexcept {
			if (this != &other) {
				_count = other._count;

				if (_count >= this->_size) {
					_size = _count + START_SIZE;
					delete[] _data;
					_data = nullptr;
					_data = new unsigned char[_size];
				}
				memset(this->_data, '\0', this->_size);

				memcpy(this->_data, other._data, _count);
			}
		}

		void operator= (pString&& other) noexcept {
			delete[] _data;

			_count = other._count;
			_size = other._size;
			_data = other._data;

			other._data = nullptr;
		}

		void operator= (const unsigned char* other) {
			if (other != nullptr) {
				if (other != this->_data) {
					_count = strlen(other);

					if (_count + 1 > this->_size) {
						delete[] _data;
						_data = nullptr;
						this->_size = _count + START_SIZE;
						_data = new unsigned char[_size];
						memset(this->_data, '\0', this->_size);
					}

					memcpy(this->_data, other, _count);
				}
			}
		}

		void operator= (const unsigned char other) {
			memset(this->_data, '\0', this->_size);
			_count = 1;
			_data[0] = other;
		}

		pString operator+ (const pString& other) {
			pString tmp = pString(*this);
			tmp += other;
			return tmp;
		}

		void operator+= (const pString& other) {
			if (other._count > 0) {
				if (this != &other) {
					size_t tmp_count = other._count + _count;
					if (tmp_count + 1 >= _size) {
						increase_size(other._count + 1);
					}
					memcpy(&(_data[_count]), other._data, other._count);
					_count = tmp_count;
				}
				else {
					size_t target_count = _count * 2;
					if (target_count + 1 >= _size) {
						increase_size(_count + 1);
					}
					memmove(&(_data[_count]), _data, _count);
					_count = target_count;
				}
			}
		}

		void operator+= (const unsigned char* other) {
			if (other != nullptr) {
				if (this->_data != other) {
					size_t tmp_count = strlen(other);
					if (_count + tmp_count + 1 >= _size) {
						increase_size(tmp_count + 1);
					}

					memcpy(&(_data[_count]), other, tmp_count);
					_count += tmp_count;
				}
				else {
					size_t target_count = _count * 2;
					if (target_count + 1 >= _size) {
						increase_size(_count + 1);
					}
					memmove(&(_data[_count]), _data, _count);
					_count = target_count;
				}
			}
		}

		void operator+= (const unsigned char other) {
			if (other == '\0') {
				return;
			}

			if (_count + 2 >= _size) {
				increase_size(2);
			}

			_data[_count] = other;
			_count++;
		}

		void operator--(int) {
			if (_count > 0) {
				_count--;
				_data[_count] = '\0';
			}
		}

		bool operator== (const unsigned char* other) const {
			if (other == nullptr) {
				return false;
			}
			if (this->_data == other) {
				return true;
			}
			if (strcmp(this->_data, other) == 0) {
				return true;
			}
			return false;
		}

		bool operator== (pString& other) const {
			if (this == &other) {
				return true;
			}
			if (strcmp(this->_data, other._data) == 0) {
				return true;
			}
			return false;
		}

		bool operator== (const pString& other) const {
			if (this == &other) {
				return true;
			}
			if (strcmp(this->_data, other._data) == 0) {
				return true;
			}
			return false;
		}

		bool operator== (pString&& other) const {
			if (strcmp(this->_data, other._data) == 0) {
				return true;
			}
			return false;
		}

		bool operator!= (pString& other) const {
			return !(this->operator==(other));
		}

		bool operator< (const pString& other) const {
			return strcmp(this->_data, other._data) < 0;
		}

		unsigned char& operator[](const size_t index) const {
			if (index >= _count + 1 || index < 0)
				throw pOutOfRangeException();
			else
				return _data[index];
		}

		size_t length() const {
			return this->_count;
		}

		const unsigned char* c_str() const {
			return this->_data;
		}

		void clear() {
			for (size_t i = 0; i < _count; i++) {
				_data[i] = '\0';
			}
			this->_count = 0;
		}

		unsigned long long hash(unsigned long long bucket_count_override) const override {
			unsigned long long base = 0xcbf29ce484222325;

			for (size_t i = 0; i < length(); i++) {
				base ^= _data[i];
				base *= 0x100000001b3;
			}

			return base;
		}


	private:

		// prefered if we know new size
		// increases size by incr
		void increase_size(size_t incr) {
			if (incr < LOW_RESIZE)
				incr = LOW_RESIZE;
			this->_size += incr;
			unsigned char* tmp_buf = new unsigned char[this->_size];

			memset(tmp_buf, '\0', this->_size);
			memcpy(tmp_buf, this->_data, this->_count);

			delete[] this->_data;
			this->_data = nullptr;
			this->_data = tmp_buf;
		}

		// suggested for usage in loop
		size_t increase_size() {
			this->_size *= INCR_FACTOR;
			unsigned char* tmp_buf = new unsigned char[this->_size];

			memset(tmp_buf, '\0', this->_size);
			memcpy(tmp_buf, this->_data, this->_count);

			delete[] this->_data;
			this->_data = nullptr;
			this->_data = tmp_buf;

			return this->_size;
		}

	public:
		struct Iterator {
			unsigned char* m_ptr;

		public:
			Iterator(unsigned char* ptr) {
				m_ptr = ptr;
			}

			unsigned char& operator*() const { return *m_ptr; }
			unsigned char* operator->() { return m_ptr; }
			Iterator& operator++() { m_ptr++; return *this; }
			Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
			friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
			friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };
		};

		Iterator begin() {
			return Iterator(&_data[0]);
		}

		Iterator end() {
			return Iterator(&_data[_count]);
		}
	};

	struct pStringCmp
	{
		bool operator()(const pString& lhs, const pString& rhs) const
		{
			return lhs < rhs;
		}
	};

}