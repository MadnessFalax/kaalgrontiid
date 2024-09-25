#pragma once
#include <cstdio>
#include "is_pointer.h"

/**
* 
*	Minimalistic vector-like array used in parser implementations.
* 
*	note: since this array's main purpose is to be used in parsers, 
*	you should not modify current contents for other purposes, 
*	yet adding new functionality or using this array structure 
*	for other purposes is not prohibited, but might be overriden 
*	for parser purposes
* 
*/


#define START_SIZE 16
#define MAX_INCREASE 1024

template <class T>
class pArray
{
protected:
	T* _data;
	int _size;
	int _count;

public:

	pArray() {
		_data = new T[START_SIZE];
		_size = START_SIZE;
		_count = 0;
	}

	pArray(pArray& other) {
		this->_size = other._size;
		this->_count = other._count;

		_data = new T[this->_size];

		for (int i = 0; i < this->_size; i++) {
			this->_data[i] = other._data[i];
		}
	}

	pArray(pArray&& other) noexcept {
		this->_size = other._size;
		this->_count = other._count;

		_data = new T[this->_size];

		for (int i = 0; i < this->_size; i++) {
			this->_data[i] = T(other._data[i]);
		}

		other.clear();
	}


	~pArray() {
		delete[] _data;
		_data = nullptr;
	}

	void operator= (pArray& other) {
		if (this != &other) {
			this->_size = other._size;
			this->_count = other._count;

			delete[] _data;
			_data = nullptr;

			_data = new T[this->_size];

			for (int i = 0; i < this->_size; i++) {
				this->_data[i] = other._data[i];
			}

			other.clear();
		}
	}

	void operator= (pArray&& other) noexcept {
		this->_size = other._size;
		this->_count = other._count;

		delete[] _data;
		_data = nullptr;

		_data = new T[this->_size];

		for (int i = 0; i < this->_size; i++) {
			this->_data[i] = other._data[i];
		}

		other.clear();

	}

	T& operator[](int index) {
		if (index >= _count || index < 0) {
			// index out of bounds
			throw;
		}
		else 
			return _data[index];
	}

	int size() {
		return this->_count;
	}

	void push_back(T val) {
		if (_count + 1 >= _size) {
			increase_size();
		}

		_data[_count] = val;
		_count++;
	}

	void clear() {
		for (int i = 0; i < _count; i++) {
			// ???
			_data[i] = T{};
		}
		_count = 0;
	}

private:
	// realocation
	void increase_size() {
		int new_size = (int)(_size * 2.5);
		T* new_data = new T[new_size];

		for (int i = 0; i < _count; i++) {
			new_data[i] = _data[i];
		}

		delete[] _data;
		_data = nullptr;

		_data = new_data;
		_size = new_size;
	}

public:
	// iterator
	struct Iterator {
		T* m_ptr;

	public:
		Iterator(T* ptr) {
			m_ptr = ptr;
		}

		T& operator*() const { return *m_ptr; }
		T* operator->() { return m_ptr; }
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

