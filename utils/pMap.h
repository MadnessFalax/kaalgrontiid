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
			Pair<key_type, value_type>* _pair;
			bool _last = true;
			Node* _next;

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

		Node** _table;
		Node* _first;						//	first element of first bucket
		Node* _first_end;					//	last element of first bucket

	public:

	};


}


