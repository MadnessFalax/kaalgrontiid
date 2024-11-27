#pragma once

#include "pArray.h"
#include "cstdlib"
#include "cstring"

/**
* 
*		The class could have been implemented using nspArray::pArray
*		but for optimization possbilities and for keeping it low-level
*		a char array is being used instead. This should also minimize 
*		acquired resources.
* 
*		_offsets array holds index elements that point to first 
*		byte of each variable size character. This is to ensure access
*		to each byte with O(n). Only a simple index computation is needed
*		and an extra byte of storage per UTF8 character stored.
* 
*		To keep an offset minimal, we utilize each byte to hold one index.
*		This gives us index range of 0 - 255. What we do here is that after
*		index 255 the value for next index is real_index % 255. We know that
*		max size of UTF8 character is 4 bytes, so we can create chunks 
*		of 256 / 4 = 64 characters where we are sure, that their real_index % 255 
*		will fit one byte value range. So now that we have this constant 64, 
*		we can now compute the address of starting byte as raw_index... let me finish the implementation because I've got myself mindfucked at this point :D
*		64 is number of indexes!!!!!!!!!!
* 
*/


namespace nspUTF8String {
	
	constexpr size_t START_SIZE = 10;
	constexpr size_t BYTES_PER_CHUNK = 64;

	using Bytes = nspArray::pArray<char>;

	class pUTF8String {
		char* _data;
		char* _offsets;
		size_t _offsets_size;
		size_t _offsets_count;
		size_t _size;
		size_t _count;

	public:
		pUTF8String() {
			_offsets_size = START_SIZE;
			_size = START_SIZE;
			_count = 0;
			_offsets_count = 0;
			_data = new char[_size];
			_offsets = new char[_offsets_count];
		}

		pUTF8String(const pUTF8String& other) noexcept {
			_offsets_size = other._offsets_size;
			_size = other._size;
			_count = other._count;
			_offsets_count = other._offsets_count;

			_data = new char[_size];
			for (size_t i = 0; i < _count; i++) {
				_data[i] = other._data[i];
			}

			_offsets = new char[_offsets_size];
			for (size_t i = 0; i < _offsets_count; i++) {
				_offsets[i] = other._offsets[i];
			}


		}


	
	};

}