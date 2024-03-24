#pragma once

#include "pArray.h"
#include "cstdlib"
#include "cstring"

#ifndef CAPACITY
#define CAPACITY 65535		// pMap CAPACITY used in get_hash function
#endif


#define START_SIZE 16
#define LOW_RESIZE 10		// higher = less chance that a reallocation will be needed after initial alloc, lower = less unused space after initial allocation
#define INCR_FACTOR 2		// higher = reallocating less often, lower = less unused space after reallocation

// accepts n as byte length which is in range <1;4>
class pCodePoint {
	char* data;
	char size;

public:
	// array of bytes
	pCodePoint(char* in, char n) {
		if (n < 1 || n > 4)
			throw;

		this->size = n;
		data = new char[size];

		for (int i = 0; i < size; i++) {
			data[i] = in[i];
		}
	}

	pCodePoint(pArray<char> in, char n) {
		if (n < 1 || n > 4)
			throw;

		if (in.size() < n) {
			n = in.size();
		}

		this->size = n;
		data = new char[size];

		for (int i = 0; i < size; i++) {
			data[i] = in[i];
		}
	}

	pCodePoint(unsigned char cd1) {
		this->size = 1;
		this->data = new char[1];
		data[0] = cd1;
	}

	pCodePoint(unsigned char cd1, unsigned char cd2) {
		this->size = 2;
		this->data = new char[2];
		data[0] = cd1;
		data[1] = cd2;
	}

	pCodePoint(unsigned char cd1, unsigned char cd2, unsigned char cd3) {
		this->size = 3;
		this->data = new char[3];
		data[0] = cd1;
		data[1] = cd2;
		data[2] = cd3;
	}

	pCodePoint(unsigned char cd1, unsigned char cd2, unsigned char cd3, unsigned char cd4) {
		this->size = 4;
		this->data = new char[4];
		data[0] = cd1;
		data[1] = cd2;
		data[2] = cd3;
		data[3] = cd4;
	}

	pCodePoint() {
		this->size = 1;
		data = new char(1);

		data[0] = '\0';
	}

	pCodePoint(pCodePoint& other) {
		this->size = other.size;
		this->data = new char[size];
		for (int i = 0; i < size; i++)
			data[i] = other.data[i];
	}

	pCodePoint(pCodePoint&& other) {
		this->size = other.size;
		delete[] this->data;
		this->data = other.data;
		other.data = nullptr;
	}

	~pCodePoint() {
		delete[] data;
		data = nullptr;
	}

	void operator=(pCodePoint&& other) {
		this->size = other.size;
		this->data = other.data;
		other.data = nullptr;
	}

	bool operator==(pCodePoint& other) {
		if (this->size != other.size) {
			return false;
		}

		for (int i = 0; i < size; i++) {
			if ((unsigned char)data[i] != (unsigned char)other.data[i])
				return false;
		}

		return true;
	}

	bool operator==(pCodePoint&& other) {
		if (this->size != other.size) {
			return false;
		}

		for (int i = 0; i < size; i++) {
			if ((unsigned char)data[i] != (unsigned char)other.data[i])
				return false;
		}

		return true;
	}

	bool operator!=(pCodePoint& other) {
		return !operator==(other);
	}

	bool operator!=(pCodePoint&& other) {
		return !operator==(other);
	}

	int get_size() {
		return (int)size;
	}

	char* get_all_bytes() {
		return this->data;
	}

	char get_byte(int n) {
		if (n >= size || n < 0)
			// index out of bounds
			throw;

		return this->data[n];
	}

	virtual int get_hash() {
		int hash_value = 0;
		for (int i = 0; i < size; i++) {
			hash_value = 37 * hash_value + data[i];
		}

		unsigned short result = hash_value % CAPACITY;

		return result;
	}
};

class pUTF8String
{
	pCodePoint* data;
	int size;				// includes trailing '\0'
	int count;

public:
	pUTF8String() {
		this->data = new pCodePoint[START_SIZE];
		this->size = START_SIZE;
		this->count = 0;

		for (int i = 0; i < this->size; i++) {
			data[i] = pCodePoint();
		}
	}

	pUTF8String(const pUTF8String& other) {
		this->size = other.size;
		this->count = other.count;
		this->data = new pCodePoint[this->size];

		for (int i = 0; i < this->size; i++) {
			this->data[i] = pCodePoint();
		}

		for (int i = 0; i < this->count; i++) {
			this->data[i] = pCodePoint(other.data[i]);
		}
	}

	pUTF8String(pUTF8String&& other) noexcept {
		this->size = other.size;
		this->count = other.count;
		this->data = other.data;

		other.data = nullptr;
	}

	pUTF8String(pArray<pCodePoint> other) {
		int tmp_count = other.size();
		int tmp_size = tmp_count + START_SIZE;
		count = tmp_count;
		size = tmp_size;
		data = new pCodePoint[size];

		for (int i = 0; i < size; i++) {
			data[i] = pCodePoint();
		}

		for (int i = 0; i < count; i++) {
			data[i] = pCodePoint(other[i]);
		}
	}

	pUTF8String(unsigned char* other) {
		int cd_count = 0;

		int other_len = strlen((char*)other);

		for (int i = 0; i < other_len; i++) {
			if (((other[i] >> 7) & 0b0) || ((other[i] >> 5) & 0b110) || ((other[i] >> 4) & 0b1110) || ((other[i] >> 3) & 0b11110)) {
				cd_count++;
			}
		}

		this->size = cd_count + START_SIZE + 1;
		this->data = new pCodePoint[size];
		this->count = cd_count + 1;

		for (int i = 0; i < size; i++) {
			data[i] = pCodePoint();
		}

		int index = 0;

		for (int i = 0; i < cd_count; i++) {
			int cd_size = 0;

			unsigned char cur_byte = other[index];

			if ((cur_byte >> 7) & 0b0) {
				cd_size = 1;
				data[i] = pCodePoint(other[index]);
			}
			else if ((cur_byte >> 5) & 0b110) {
				cd_size = 2;
				data[i] = pCodePoint(other[index], other[index + 1]);
			}
			else if ((cur_byte >> 4) & 0b1110) {
				cd_size = 3;
				data[i] = pCodePoint(other[index], other[index + 1], other[index + 2]);
			}
			else if ((cur_byte >> 3) & 0b11110) {
				cd_size = 4;
				data[i] = pCodePoint(other[index], other[index + 1], other[index + 2], other[index + 3]);
			}

			index += cd_size;
		}
	}

	~pUTF8String() {
		delete[] data;
		data = nullptr;
	}

	void operator= (pUTF8String& other) {
		if (this != &other) {
			int tmp_count = other.count;

			if (tmp_count > this->size - 1) {
				int tmp_size = tmp_count + START_SIZE;
				count = tmp_count;
				size = tmp_size;
				delete[] data;
				data = new pCodePoint[size];
			}

			for (int i = 0; i < size; i++) {
				data[i] = pCodePoint();
			}

			count = other.count;

			for (int i = 0; i < count; i++) {
				data[i] = pCodePoint(other.data[i]);
			}
		}
	}

	// move resource from anonymous pUTF8String
	void operator= (pUTF8String&& other) noexcept {
		delete[] data;

		count = other.count;
		size = other.size;
		data = other.data;

		other.data = nullptr;
	}

	void operator+= (pUTF8String& other) {
		int tmp_count = other.count;
		if (count + tmp_count >= size - 1) {
			increase_size(tmp_count);
		}

		for (int i = count; i < count + other.count; i++) {
			this->data[i] = pCodePoint(other.data[i]);
		}
		count += tmp_count;
	}

	void operator+= (pCodePoint& other) {
		if (count + 1 >= size - 1) {
			increase_size(1);
		}

		data[count] = pCodePoint(other);
		count++;
	}

	void operator+= (const char* other) {
		int cd_count = 0;

		int other_len = strlen((char*)other);

		for (int i = 0; i < other_len; i++) {
			if (((other[i] >> 7) & 0b0) || ((other[i] >> 5) & 0b110) || ((other[i] >> 4) & 0b1110) || ((other[i] >> 3) & 0b11110)) {
				cd_count++;
			}
		}

		if (count + cd_count >= size - 1) {
			increase_size(cd_count);
		}

		for (int i = count; i < count + cd_count; i++) {
			data[i] = pCodePoint();
		}

		int index = 0;

		for (int i = count; i < count + cd_count; i++) {
			int cd_size = 0;

			unsigned char cur_byte = other[index];

			if ((cur_byte >> 7) & 0b0) {
				cd_size = 1;
				data[i] = pCodePoint(other[index]);
			}
			else if ((cur_byte >> 5) & 0b110) {
				cd_size = 2;
				data[i] = pCodePoint(other[index], other[index + 1]);
			}
			else if ((cur_byte >> 4) & 0b1110) {
				cd_size = 3;
				data[i] = pCodePoint(other[index], other[index + 1], other[index + 2]);
			}
			else if ((cur_byte >> 3) & 0b11110) {
				cd_size = 4;
				data[i] = pCodePoint(other[index], other[index + 1], other[index + 2], other[index + 3]);
			}

			index += cd_size;
		}

		count += cd_count;
	}

	void operator+= (char other) {
		if (count + 1 >= size - 1) {
			increase_size(1);
		}

		data[count] = pCodePoint(other);
		count++;
	}

	/*
	bool operator== (const char* other) {
		for () {
			return false;
		}
		return true;
	}
	*/

	bool operator== (pUTF8String& other) {
		if (this == &other)
			return true;

		if (this->count != other.count) {
			return false;
		}

		for (size_t i = 0; i < this->count; i++)
		{
			if (data[i] != other.data[i]) {
				return false;
			}
		}

		return true;
	}

	bool operator== (pUTF8String&& other) {
		if (this->count != other.count) {
			return false;
		}

		for (size_t i = 0; i < count; i++)
		{
			if (data[i] != other.data[i]) {
				return false;
			}
		}

		return true;
	}

	bool operator!= (pUTF8String& other) {
		return !(this->operator==(other));
	}

	bool operator!= (pUTF8String&& other) {
		return !(this->operator==(other));
	}

	/*
	bool operator< (const pUTF8String& other) const {
		if (this->count < other.count)
			return true;
		if (this->count > other.count)
			return false;

		for (size_t i = 0; i < count; i++)
		{
			if ((unsigned char)this->data[i])
		}
	}
	*/

	pCodePoint operator[](int index) {
		if (index >= count + 1 || index < 0)
			// out of bounds
			throw;
		else
			return data[index];
	}

	int length() {
		return this->count + 1;
	}

	void c_str(unsigned char* buffer, int buf_len /* includes '\n' */) {
		for (int i = 0; i < buf_len; i++) {
			buffer[i] = '\0';
		}

		int index = 0;
		int code_point_index = 0;
		int byte_index = 0;

		while (index < buf_len) {
			if (byte_index >= data[code_point_index].get_size()) {
				code_point_index++;
				byte_index = 0;
			}
			if (code_point_index >= count) {
				break;
			}

			buffer[index] = data[code_point_index].get_byte(byte_index);

			byte_index++;
			index++;
		}
	}

	void clear() {
		this->count = 0;
	}

	virtual int get_hash() {
		int hash_value = 0;
		for (int i = 0; i < count; i++) {
			hash_value = 37 * hash_value + data[i].get_hash();
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
		pCodePoint* tmp_buf = new pCodePoint[this->size];

		for (int i = 0; i < size; i++) {
			tmp_buf[i] = pCodePoint();
		}
		for (int i = 0; i < count; i++) {
			tmp_buf[i] = pCodePoint(data[i]);
		}

		delete[] this->data;
		this->data = tmp_buf;
	}

	// suggested for usage in loop
	int increase_size() {
		this->size *= INCR_FACTOR;
		pCodePoint* tmp_buf = new pCodePoint[this->size];

		for (size_t i = 0; i < size; i++)
		{
			tmp_buf[i] = pCodePoint();
		}
		for (size_t i = 0; i < count; i++)
		{
			tmp_buf[i] = pCodePoint(data[i]);
		}

		delete[] this->data;
		this->data = tmp_buf;

		return this->size;
	}

public:
	struct Iterator {
		pCodePoint* m_ptr;

	public:
		Iterator(pCodePoint* ptr) {
			m_ptr = ptr;
		}

		pCodePoint& operator*() const { return *m_ptr; }
		pCodePoint* operator->() { return m_ptr; }
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

/*
struct pUTF8StringCmp
{
	bool operator()(const pUTF8String& lhs, const pUTF8String& rhs) const
	{
		return lhs < rhs;
	}
};

*/

