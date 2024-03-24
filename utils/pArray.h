#pragma once
#include <cstdio>

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

template <class T>
class pArray
{
protected:
	T* data;
	int _size;
	int count;


public:

	pArray() {
		data = new T[START_SIZE];
		_size = START_SIZE;
		count = 0;
	}

	// copy constr
	pArray(pArray& other) {
		this->_size = other._size;
		this->count = other.count;

		data = new T[this->_size];

		for (int i = 0; i < this->_size; i++) {
			this->data[i] = other.data[i];
		}
	}

	// move constr
	pArray(pArray&& other) noexcept {
		this->_size = other._size;
		this->count = other.count;

		data = new T[this->_size];

		for (int i = 0; i < this->_size; i++) {
			this->data[i] = other.data[i];
		}

		other.clear();
	}


	~pArray() {
		delete[] data;
		data = nullptr;
	}

	// move assignment (copy is forbiden for this array)
	void operator= (pArray& other) {
		if (this != &other) {
			this->_size = other._size;
			this->count = other.count;

			delete[] data;
			data = nullptr;

			data = new T[this->_size];

			for (int i = 0; i < this->_size; i++) {
				this->data[i] = other.data[i];
			}

			other.clear();
		}
	}

	// move assignment
	void operator= (pArray&& other) noexcept {
		this->_size = other._size;
		this->count = other.count;

		delete[] data;
		data = nullptr;

		data = new T[this->_size];

		for (int i = 0; i < this->_size; i++) {
			this->data[i] = other.data[i];
		}

		other.clear();

	}

	T& operator[](int index) {
		if (index >= count || index < 0) {
			// index out of bounds
			throw;
		}
		else 
			return data[index];	
	}

	int size() {
		return this->count;
	}

	void push_back(T val) {
		if (count + 1 >= _size) {
			increase_size();
		}

		data[count] = val;
		count++;
	}

	void clear() {
		for (int i = 0; i < count; i++) {
			// ???
			data[i] = T{};
		}
		count = 0;
	}

private:
	// realocation
	void increase_size() {
		int new_size = (int)(_size * 2.5);
		T* new_data = new T[new_size];

		for (int i = 0; i < count; i++) {
			new_data[i] = data[i];
		}

		delete[] data;
		data = nullptr;

		data = new_data;
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
		return Iterator(&data[0]);
	}

	Iterator end() {
		return Iterator(&data[count]);
	}
};

