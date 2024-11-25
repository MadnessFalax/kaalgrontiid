#pragma once

template<typename T>
struct _is_pointer {
	static constexpr bool value = false;
};

template<typename T>
struct _is_pointer<T*> {
	static constexpr bool value = true;
};

template<typename T>
constexpr bool is_pointer() {
	return _is_pointer<T>::value;
}