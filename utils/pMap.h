#pragma once
#include "pPair.h"


namespace nspMap {

	constexpr size_t CAPACITY = 65521;

	template <class Tkey, class Tval>
	using Pair = nspPair::pPair<Tkey, Tval>;

	template <class Tkey, class Tval>
	class pMap
	{
		using key_type = Tkey;
		using value_type = Tval;

		struct Node {
			Pair<key_type, value_type>* _pair = nullptr;
			bool _last = true;
			Node* _next = nullptr;

			const bool& is_last() const noexcept {
				return _last;
			}

			Node* get_next() const noexcept {
				return _next;
			}

			void insert_next(Node* other) noexcept {
				if (this->_last) {
					if (_next != nullptr) {
						other->_next = _next;
					}
					_next = other;
					_last = false;

				}
				else {
					_next->insert_next(other);
				}
			}

			void delete_bucket() {
				if (_next != nullptr) {
					_next->delete_bucket();
				}
				delete _next;
				_next = nullptr;
			}
		};

		Node** _table = nullptr;
		Node* _first = nullptr;						//	first element of first bucket
		Node* _first_end = nullptr;					//	last element of first bucket

	public:
		pMap() {
			_table = new Node * [CAPACITY];
			for (size_t i = 0; i < CAPACITY; i++) {
				_table[i] = nullptr;
			}
		};

		value_type& operator[](const key_type key) {

		}


	};


}


