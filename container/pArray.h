#pragma once
#include <cstdio>
#include "../utils/is_pointer.h"
#include <type_traits>
#include "../utils/exception/pOutOfRangeException.h"

/**
* 
*	Owns elements!
* 
*/

namespace nspArray {


	constexpr size_t START_SIZE = 16;
	constexpr size_t MAX_INCREASE = 1024;

	template <class T>
	class pArray
	{
	protected:
		T** _data = nullptr;
		size_t _size = START_SIZE;
		size_t _count = 0;

	public:
		using value_type = T;
		using reference = T&;
		using pointer = T*;
		using const_type = const T;
		using const_reference = const T&;


		pArray() : _data(new T*[START_SIZE]) {}

		pArray(size_t initial_size) : _size(initial_size), _data(new pointer[initial_size]) {}

		pArray(const pArray& other) : _size(other._size), _count(other._count) {

			_data = new pointer[this->_size];

			for (size_t i = 0; i < this->_count; i++) {
				this->_data[i] = new value_type{ (*other._data[i]) };
			}
		}

		pArray(pArray&& other) noexcept : _size(other._size), _count(other._count), _data(other._data) {

			other._data = nullptr;
		}

		~pArray() {
			for (size_t i = 0; i < _count; i++) {
				delete _data[i];
				_data[i] = nullptr;
			}
			delete[] _data;
			_data = nullptr;
		}
	
		void operator= (pArray& other) {
			if (this != &other) {
				this->_size = other._size;
				this->_count = other._count;

				delete[] _data;
				_data = nullptr;

				_data = new pointer[this->_size];

				for (size_t i = 0; i < this->_size; i++) {
					this->_data[i] = new value_type{ *(other._data[i]) };
				}
			}
		}

		void operator= (pArray&& other) noexcept {
			for (size_t i = 0; i < _count; i++) {
				delete _data[i];
				_data[i] = nullptr;
			}
			delete[] _data;
			_data = nullptr;

			this->_size = other._size;
			this->_count = other._count;

			other._count = 0;

			_data = other._data;
			other._data = nullptr;
		}

		void operator+= (pArray& other) noexcept {
			if (_size < _count + other._count) {
				increase_size(other._count);
			}

			auto other_start_count = other._count;				// in case other is this
			for (size_t i = 0; i < other_start_count; i++) {
				_data[_count] = new value_type{ *(other._data[i]) };
				_count++;
			}
		}

		void operator+= (pArray&& other) noexcept {
			if (_size < _count + other._count) {
				increase_size(other._count);
			}

			for (size_t i = 0; i < other._count; i++) {
				_data[_count] = new value_type{ *(other._data[i]) };
				_count++;
			}
		}

		reference operator[](const size_t index) {
			if (index >= _count || index < 0) {
				throw pOutOfRangeException();
			}
			else
				return *(_data[index]);
		}

		const size_t& size() const noexcept {
			return this->_count;
		}

		void push_back(const value_type val) {
			if (_count + 1 >= _size) {
				increase_size();
			}

			_data[_count] = new value_type{ val };
			_count++;
		}

		/*
			does not dispose elements! sets them to NULL / nullptr value instead
		*/
		constexpr void clear() {
			for (size_t i = 0; i < _count; i++) {
				_data[i] = nullptr;
			}
				
			_count = 0;
		}

		// both disposes every element and resets container
		constexpr void clear_dispose() {
			for (size_t i = 0; i < _count; i++) {
				delete _data[i];
				_data[i] = nullptr;
			}

			_count = 0;
		}

	private:
		// realocation
		void increase_size() noexcept {
			size_t new_size = (size_t)(_size * 2.5);
			pointer* new_data = new pointer[new_size];

			for (size_t i = 0; i < _count; i++) {
				new_data[i] = _data[i];
				_data[i] = nullptr;
			}

			delete[] _data;
			_data = nullptr;

			_data = new_data;
			_size = new_size;
		}

		// we know the extra size N needed
		void increase_size(size_t n) noexcept {
			size_t new_size = _size + n;
			pointer* new_data = new pointer[new_size];

			for (size_t i = 0; i < _count; i++) {
				new_data[i] = _data[i];
				_data[i] = nullptr;
			}

			delete[] _data;
			_data = nullptr;

			_data = new_data;
			_size = new_size;
		}

	public:
		// iterator
		struct Iterator {
			pointer* m_ptr;

		public:
			Iterator(pointer* ptr) {
				m_ptr = ptr;
			}

			reference operator*() const { return **m_ptr; }
			pointer operator->() { return *m_ptr; }
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

};