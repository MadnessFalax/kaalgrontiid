#pragma once

#include "pArray.h"
#include "cstdlib"
#include "cstring"

// REDO to hold null terminator

namespace nspString {

	constexpr size_t CAPACITY = 65521;		// highest prime lower than ulong max value
	constexpr size_t START_SIZE = 16;
	constexpr size_t LOW_RESIZE = 10;		// higher = less chance that a reallocation will be needed after initial alloc, lower = less unused space after initial allocation
	constexpr size_t INCR_FACTOR = 2;		// higher = reallocating less often, lower = less unused space after reallocation

	// stores trailing null terminator
	// operator+ is not implemented so that new heap objects arent constructed unintentionally
	class pString
	{
		char* _data;			// holds trailing '\0'
		size_t _size;			// total bytes allocated ('\0' included)
		size_t _count;			// string char length ('\0' NOT included)

	public:
		pString() {
			this->_data = new char[START_SIZE];
			this->_size = START_SIZE;
			this->_count = 0;

			memset(this->_data, '\0', this->_size);
		}

		pString(const pString& other) {
			this->_size = other._size;
			this->_count = other._count;
			this->_data = new char[this->_size];

			memset(this->_data, '\0', this->_size);

			memcpy(this->_data, other._data, this->_count);
		}

		pString(pString&& other) noexcept {
			this->_size = other._size;
			this->_count = other._count;
			this->_data = other._data;

			other._data = nullptr;
		}

		pString(const char* other) {
			if (other != nullptr) {
				_count = strlen(other);
			}
			else {
				_count = 0;
			}
			_size = _count + START_SIZE;
			_data = new char[_size];

			memset(this->_data, '\0', this->_size);

			if (other != nullptr) {
				memcpy(this->_data, other, this->_count);
			}

		}

		~pString() {
			delete[] _data;
			_data = nullptr;
		}

		void operator= (const pString& other) noexcept {
			if (this != &other) {
				size_t _count = other._count;

				if (_count > this->_size) {
					_size = _count + START_SIZE;
					delete[] _data;
					_data = nullptr;
					_data = new char[_size];
					memset(this->_data, '\0', this->_size);
				}

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

		void operator= (const char* other) {
			if (other != nullptr) {
				if (other != this->_data) {
					int _count = strlen(other);

					if (_count + 1 > this->_size) {
						delete[] _data;
						_data = nullptr;
						this->_size = _count + START_SIZE;
						_data = new char[_size];
						memset(this->_data, '\0', this->_size);
					}

					memcpy(this->_data, other, _count);
				}
			}
		}

		void operator= (char other) {
			memset(this->_data, '\0', this->_size);
			_count = 1;
			_data[0] = other;
		}

		void operator+= (const pString& other) {
			if (other._count > 0) {
				if (this != &other) {
					size_t tmp_count = other._count + _count;
					if (tmp_count + 1 > _size) {
						increase_size(other._count + 1);
					}
					memcpy(&(_data[_count]), other._data, tmp_count);
					_count = tmp_count;
				}
				else {
					size_t target_count = _count * 2;
					if (target_count + 1 > _size) {
						increase_size(_count + 1);
					}
					memmove(&(_data[_count]), _data, _count);
					_count = target_count;
				}
			}
		}

		void operator+= (const char* other) {
			if (other != nullptr) {
				if (this->_data != other) {
					size_t tmp_count = strlen(other);
					if (_count + tmp_count + 1> _size) {
						increase_size(tmp_count + 1);
					}

					memcpy(&(_data[_count]), other, tmp_count);
					_count += tmp_count;
				}
				else {
					size_t target_count = _count * 2;
					if (target_count + 1 > _size) {
						increase_size(_count + 1);
					}
					memmove(&(_data[_count]), _data, _count);
					_count = target_count;
				}
			}
		}

		void operator+= (const char other) {
			if (other == '\0') {
				return;
			}

			if (_count + 1 > _size) {
				increase_size(2);
			}

			_data[_count] = other;
			_count++;
		}

		bool operator== (const char* other) const {
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

		char& operator[](const size_t index) const {
			if (index >= _count + 1 || index < 0)
				// out of bounds
				throw;
			else
				return _data[index];
		}

		size_t length() const {
			return this->_count;
		}

		const char* c_str() const {
			return this->_data;
		}

		void clear() {
			for (size_t i = 0; i < _count; i++) {
				_data[i] = '\0';
			}
			this->_count = 0;
		}

		virtual unsigned short get_hash() {
			unsigned long hash_value = 0;
			for (size_t i = 0; i < _count; i++) {
				hash_value = 37 * hash_value + _data[i];
				hash_value %= ULONG_MAX;
			}

			unsigned short result = hash_value % CAPACITY;

			return result;
		}


	private:

		// prefered if we know new size
		// increases size by incr
		void increase_size(size_t incr) {
			if (incr < LOW_RESIZE)
				incr = LOW_RESIZE;
			this->_size += incr;
			char* tmp_buf = new char[this->_size];

			memset(tmp_buf, '\0', this->_size);
			memcpy(tmp_buf, this->_data, this->_count);

			delete[] this->_data;
			this->_data = nullptr;
			this->_data = tmp_buf;
		}

		// suggested for usage in loop
		int increase_size() {
			this->_size *= INCR_FACTOR;
			char* tmp_buf = new char[this->_size];

			memset(tmp_buf, '\0', this->_size);
			memcpy(tmp_buf, this->_data, this->_count);

			delete[] this->_data;
			this->_data = nullptr;
			this->_data = tmp_buf;

			return this->_size;
		}

	public:
		struct Iterator {
			char* m_ptr;

		public:
			Iterator(char* ptr) {
				m_ptr = ptr;
			}

			char& operator*() const { return *m_ptr; }
			char* operator->() { return m_ptr; }
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