#pragma once
#include <type_traits>

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
		virtual ~pHashable() = default;
		virtual unsigned long long hash() const = 0;			// used in pMap class	
	};

	/*
		ret_T defines container size (1, 2, 3 or 4 byte bucket range), inp_T is contained type
		uses FNV-1a hash algorithm
		more on: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
		works with inp_T being primitive type, other types cause UB
	*/
	template<typename ret_T, typename inp_T>
	static ret_T hash(typename std::enable_if<std::is_arithmetic_v<inp_T>, inp_T>::type& val) {
		unsigned long long hash = 0xcbf29ce484222325;

		unsigned char* byte_array = reinterpret_cast<unsigned char*>(&val);

		for (size_t i = 0; i < sizeof(inp_T); i++) {
			hash = hash ^ byte_array[i];
			hash *= 0x100000001b3;
		}

		return hash % hash_max_val<ret_T>();
	}

	template<typename ret_T, typename inp_T>
	static ret_T hash(const typename std::enable_if<std::is_arithmetic_v<inp_T>, inp_T>::type& val) {
		unsigned long long hash = 0xcbf29ce484222325;

		const unsigned char* byte_array = reinterpret_cast<const unsigned char*>(&val);

		for (size_t i = 0; i < sizeof(inp_T); i++) {
			hash = hash ^ byte_array[i];
			hash *= 0x100000001b3;
		}

		return hash % hash_max_val<ret_T>();
	}

	template<typename ret_T, class hashable_T>
	static ret_T hash(typename std::enable_if_t<std::is_base_of_v<pHashable, hashable_T>, hashable_T>& val) {
		auto& cast_val = static_cast<pHashable&>(val);
		return cast_val.hash() % hash_max_val<ret_T>();
	}

	template<typename ret_T, class hashable_T>
	static ret_T hash(typename std::enable_if_t<std::is_base_of_v<pHashable, hashable_T>, hashable_T>* val) {
		auto* cast_val = static_cast<pHashable*>(val);
		return cast_val->hash() % hash_max_val<ret_T>();
	}

	template<typename ret_T, class hashable_T>
	static ret_T hash(const typename std::enable_if_t<std::is_base_of_v<pHashable, hashable_T>, hashable_T>& val) {
		const auto& cast_val = static_cast<const pHashable&>(val);
		return cast_val.hash() % hash_max_val<ret_T>();
	}

	template<typename ret_T, class hashable_T>
	static ret_T hash(const typename std::enable_if_t<std::is_base_of_v<pHashable, hashable_T>, hashable_T>* val) {
		const auto* cast_val = static_cast<const pHashable*>(val);
		return cast_val->hash() % hash_max_val<ret_T>();
	}

}


