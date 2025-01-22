#pragma once

namespace nspHashable {
	
	constexpr size_t CAPACITY = 65521;

	template<typename T>
	struct _hash_max_val {
		static constexpr T value = 1;				// use defined types or define your own here
	};

	template<>
	struct _hash_max_val<unsigned char> {
		static constexpr unsigned char value = 251;
	};

	template<>
	struct _hash_max_val<unsigned short> {
		static constexpr unsigned short value = 65521;
	};

	template<>
	struct _hash_max_val<unsigned int> {
		static constexpr unsigned int value = 4294967291;
	};

	template<>
	struct _hash_max_val<unsigned long long> {
		static constexpr unsigned long long value = 18446744073709551557;
	};

	template<typename T>
	constexpr T hash_max_val() {
		return _hash_max_val<T>::value;
	}


	class pHashable
	{
	public:
		virtual unsigned long long hash() const = 0;			// used in pMap class
		
	};


	/*
		ret_T defines container size (1, 2, 3 or 4 byte bucket range), inp_T is contained type
		uses FNV-1a hash algorithm
		more on: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
	*/
	template<typename ret_T>
	static ret_T hash(const pHashable& val) {
		return val.hash() % hash_max_val<ret_T>();
	}

	template<typename ret_T, typename inp_T>
	static ret_T hash(inp_T val) {
		unsigned long long hash = 0xcbf29ce484222325;

		for (size_t i = 0; i < sizeof(inp_T); i++) {
			hash = hash ^ val;
			hash *= 0x100000001b3;
		}

		return hash % hash_max_val<ret_T>();
	}

}


