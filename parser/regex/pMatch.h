#pragma once
#include "../../container/pString.h"
#include "../../container/pArray.h"

namespace nspRegex {

	template<class arr_type>
	using Array = nspArray::pArray<arr_type>;
	using String = nspString::pString;


	class pMatch {
		bool _is_match = false;
		Array<String>* _groups = new Array<String>();

	public:
		pMatch() {};

		pMatch(pMatch& other) : _is_match(other._is_match) {
			for (String group : *(other._groups)) {
				add_group(group);
			}
		};

		pMatch(pMatch&& other) noexcept : _is_match(other._is_match), _groups(other._groups) {
			other._groups = nullptr;
		};

		~pMatch() {
			delete _groups;
			_groups = nullptr;
		};
		

		bool is_match() const { return _is_match; };
		bool set_match() { return (_is_match = true); };
		
		String& add_group(String& group) {
			_groups->push_back(group);
			return (*_groups)[_groups->size() - 1];
		};

		pMatch& operator=(pMatch& other) {
			if (this != &other) {
				_is_match = other._is_match;
				_groups = new Array<String>();
				for (String group : *(other._groups)) {
					add_group(group);
				}
			}

			return *this;
		}

		pMatch& operator=(pMatch&& other) noexcept {
			if (this != &other) {
				_is_match = other._is_match;
				_groups = other._groups;
				other._groups = nullptr;
			}

			return *this;
		}

		String& operator[](size_t index) {
			return (*_groups)[index];
		}
	};

}