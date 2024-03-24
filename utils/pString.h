#pragma once

#include "pArray.h"
#include "cstdlib"
#include "cstring"

#ifndef CAPACITY
#define CAPACITY 65535
#endif


#define START_SIZE 16
#define LOW_RESIZE 10		// higher = less chance that a reallocation will be needed after initial alloc, lower = less unused space after initial allocation
#define INCR_FACTOR 2		// higher = reallocating less often, lower = less unused space after reallocation

class pString
{
	char* data;
	int size;				// includes trailing '\0'
	int count;

public:
	pString() {
		this->data = new char[START_SIZE];
		this->size = START_SIZE;
		this->count = 0;

		memset(this->data, '\0', this->size);
	}

	pString(const pString& other) {
		this->size = other.size;
		this->count = other.count;
		this->data = new char[this->size];

		memset(this->data, '\0', this->size);

		memcpy(this->data, other.data, this->count);
	}

	pString(pString&& other) noexcept {
		this->size = other.size;
		this->count = other.count;
		this->data = other.data;

		other.data = nullptr;
	}

	pString(const char* other) {
		int tmp_count = strlen(other);
		int tmp_size = tmp_count + START_SIZE;
		count = tmp_count;
		size = tmp_size;
		data = new char[size];

		memset(this->data, '\0', this->size);

		memcpy(this->data, other, this->count);

	}

	~pString() {
		delete[] data;
		data = nullptr;
	}

	void operator= (pString& other) {
		if (this != &other) {
			int tmp_count = other.count;

			if (tmp_count > this->size - 1) {
				int tmp_size = tmp_count + START_SIZE;
				count = tmp_count;
				size = tmp_size;
				delete[] data;
				data = new char[size];
			}

			memset(this->data, '\0', this->size);

			memcpy(this->data, other.data, tmp_count);

			count = other.count;
		}
	}

	// move resource from anonymous pString
	void operator= (pString&& other) noexcept {
		delete[] data;

		count = other.count;
		size = other.size;
		data = other.data;

		other.data = nullptr;
	}

	void operator= (const char* other) {
		int tmp_len = strlen(other);

		if (tmp_len > this->size - 1) {
			delete[] data;
			data = nullptr;

			this->size = tmp_len + START_SIZE;
			this->count = tmp_len;

			data = new char[this->size];
		}

		memset(this->data, '\0', this->size);
		memcpy(this->data, other, tmp_len);
	}

	void operator= (char other) {
		memset(this->data, '\0', this->size);
		this->data[0] = other;
	}

	void operator+= (pString& other) {
		int tmp_count = other.count;
		if (count + tmp_count >= size - 1) {
			increase_size(tmp_count);
		}

		memcpy(&(data[count]), other.data, tmp_count);
		count += tmp_count;
	}

	void operator+= (const char* other) {
		int tmp_count = strlen(other);
		if (count + tmp_count >= size - 1) {
			increase_size(tmp_count);
		}

		memcpy(&(data[count]), other, tmp_count);
		count += tmp_count;
	}

	void operator+= (char other) {
		if (count + 1 >= size - 1) {
			increase_size(1);
		}

		data[count] = other;
		count++;
	}

	bool operator== (const char* other) {
		if (strcmp(this->data, other) == 0) {
			return true;
		}
		return false;
	}

	bool operator== (pString& other) {
		if (this == &other)
			return true;
		if (strcmp(this->data, other.data) == 0) {
			return true;
		}
		return false;
	}

	bool operator== (pString&& other) {
		if (strcmp(this->data, other.data) == 0) {
			return true;
		}
		return false;
	}

	bool operator!= (pString& other) {
		return !(this->operator==(other));
	}

	bool operator< (const pString& other) const {
		return strcmp(this->data, other.data) < 0;
	}

	char operator[](int index) {
		if (index >= count + 1 || index < 0)
			// out of bounds
			throw;
		else
			return data[index];
	}

	int length() {
		return this->count + 1;
	}

	char* c_str() {
		return this->data;
	}

	void clear() {
		this->count = 0;
	}

	virtual int get_hash() {
		int hash_value = 0;
		for (int i = 0; i < count; i++) {
			hash_value = 37 * hash_value + data[i];
		}
		
		unsigned short result = hash_value % CAPACITY;
		
		return result;
	}


private:

	// prefered if we know new size
	void increase_size(int incr) {
		if (incr < LOW_RESIZE)
			incr = LOW_RESIZE;
		this->size += incr;
		char* tmp_buf = new char[this->size];

		memset(tmp_buf, '\0', this->size);
		memcpy(tmp_buf, this->data, this->count);

		delete[] this->data;
		this->data = tmp_buf;
	}

	// suggested for usage in loop
	int increase_size() {
		this->size *= INCR_FACTOR;
		char* tmp_buf = new char[this->size];

		memset(tmp_buf, '\0', this->size);
		memcpy(tmp_buf, this->data, this->count);

		delete[] this->data;
		this->data = tmp_buf;

		return this->size;
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
		return Iterator(&data[0]);
	}

	Iterator end() {
		return Iterator(&data[count]);
	}
};

struct pStringCmp
{
	bool operator()(const pString& lhs, const pString& rhs) const
	{
		return lhs < rhs;
	}
};

