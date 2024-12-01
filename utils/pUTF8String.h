#pragma once

#include "pArray.h"
#include "cstdlib"
#include "cstring"

/**
* 
* 
*/


namespace nspUTF8String {

	class pUTF8Scanner;

	class pUTF8CharacterIndex {
		friend pUTF8Scanner;

		size_t _index = 0;
		unsigned char _byte_count = 0;
		bool _is_error = true;
		pUTF8CharacterIndex* _prev = nullptr;
		pUTF8CharacterIndex* _next = nullptr;

	public:
		pUTF8CharacterIndex(size_t index, unsigned char byte_count, bool is_error, pUTF8CharacterIndex* prev, pUTF8CharacterIndex* next = nullptr) : _index(index), _byte_count(byte_count), _is_error(is_error), _prev(prev), _next(next) {}
		~pUTF8CharacterIndex() {
			if (_next != nullptr) {
				_next->_prev = nullptr;
				_next = nullptr;
			}
			if (_prev != nullptr) {
				_prev->_next = nullptr;
				_prev = nullptr;
			}
		}
	};
	
	constexpr size_t START_SIZE = 16;

	using index_type = pUTF8CharacterIndex;
	using index_ptr = index_type*;
	using index_array = nspArray::pArray<index_ptr>;

	class pUTF8String {
		friend pUTF8Scanner;

		char* _data = nullptr;
		index_array* _offsets = nullptr;
		// buffer size
		size_t _size = START_SIZE;
		// character count
		size_t _count = 0;
		bool _error_free = false;
		bool _scanned = false;


	public:
		pUTF8String() {
			_data = new char[_size];
			_offsets = new index_array();
		}

		pUTF8String(const pUTF8String& other) {
			_size = other._size;
			_count = other._count;
			_data = new char[_size];
			for (size_t i = 0; i < _size; i++) {
				_data[i] = other._data[i];
			}
			_offsets = new index_array(_size);
			for (auto x : *(other._offsets)) {
				_offsets->push_back(x);
			}
		}

		pUTF8String(pUTF8String&& other) noexcept {
			_size = other._size;
			_count = other._count;
			_data = other._data;
			other._data = nullptr;
			_offsets = other._offsets;
			other._offsets = nullptr;
		}

		pUTF8String(const char* other) {
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

		~pUTF8String() {
			delete[] _data;
			auto offsets_size = _offsets->size();
			for (size_t i = 0; i < offsets_size; i++) {
				delete (*_offsets)[i];
			}
			delete _offsets;
		}

	};


	class pUTF8Scanner {
		pUTF8String* _str = nullptr;
		unsigned char _cur_byte = '\0';
		unsigned char _expected_continuation = 0;
		unsigned char _seq_length = 0;
		size_t _seq_index = 0;
		bool _is_error = false;
		size_t _start_index = 0;


	public:
		pUTF8Scanner(pUTF8String* str) : _str(str) {}
		~pUTF8Scanner() { _str = nullptr; }
		
		/*
		*	scan utf8 for errors, and starting byte indices
		*/
		void scan() {
			if (_str == nullptr) {
				return;
			}

			if (_str->_count == 0) {
				return;
			}


			// refer to https://en.wikipedia.org/wiki/UTF-8
			for (_seq_index = 0; _seq_index < _str->_count; _seq_index++) {
				_cur_byte = _str->_data[_seq_index];
				if (_cur_byte == '\0') {
				// end
					if (_expected_continuation != 0) {
						_is_error = true;
						_end_sequence();
					}
					break;
				}
				else if (_cur_byte <= 0x7F) {
				// 1 byte starter
					if (_expected_continuation != 0) {
						_is_error = true;
						_end_sequence();
					}
					_start_index = _seq_index;
					_seq_length = 1;
					_end_sequence();
				}
				else if (_cur_byte <= 0xBF) {
				// continuation byte
					_expected_continuation--;
					if (_expected_continuation == 0) {
						_end_sequence();
					}
				}
				else if (_cur_byte <= 0xC1) {
				// invalid byte
					_is_error = true;
				}
				else if (_cur_byte <= 0xDF) {
				// 2 byte starter
					if (_expected_continuation != 0) {
						_is_error = true;
						_end_sequence();
					}
					_start_index = _seq_index;
					_expected_continuation = 1;
					_seq_length = 2;
				}
				else if (_cur_byte <= 0xEF) {
				// 3 byte starter
					if (_expected_continuation != 0) {
						_is_error = true;
						_end_sequence();
					}
					_start_index = _seq_index;
					_expected_continuation = 2;
					_seq_length = 3;
				}
				else if (_cur_byte <= 0xF4) {
				// 4 byte starter
					if (_expected_continuation != 0) {
						_is_error = true;
						_end_sequence();
					}
					_start_index = _seq_index;
					_expected_continuation = 3;
					_seq_length = 4;
				}
				else {
				// invalid byte
					_is_error = true;
				}
			}
		}

	private:
		void _end_sequence() {
			size_t tmp_index = _str->_offsets->size() - 1;
			index_ptr prec = nullptr;
			if (tmp_index >= 0) {
				auto prec = (*_str->_offsets)[_str->_offsets->size() - 1];
			}
			auto new_node = new pUTF8CharacterIndex(_start_index, _seq_length, _is_error, prec);
			_str->_offsets->push_back(new_node);
			if (prec != nullptr) {
				prec->_next = new_node;
			}
			_seq_length = 0;
			_is_error = false;
		}
	};

}