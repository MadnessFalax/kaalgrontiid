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
		virtual unsigned short hash() = 0;			// used in pMap class
	};


	/*
		ret_T defines container size (1, 2, 3 or 4 byte bucket range), inp_T is contained type
	*/
	template<class ret_T, class inp_T>
	static ret_T hash(inp_T val) {
		ret_T value = 1;




		return value % hash_max_val<ret_T>();
	}
}


