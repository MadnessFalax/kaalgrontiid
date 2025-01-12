#pragma once
#include "pPair.h"
#include "abstract/pHashable.h"


namespace nspMap {


	template <class Tkey, class Tval>
	using Pair = nspPair::pPair<Tkey, Tval>;

	template <class Tkey, class Tval, class Taddress = unsigned short>			// Taddress defines address space for buckets: unsigned char, unsigned short, unsigned int, unsigned long long for 1, 2, 3, 4 bytes respectivelly
	class pMap
	{
		using key_type = Tkey;
		using value_type = Tval;
		using address_type = Taddress;

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
		address_type _capacity = nspHashable::hash_max_val<address_type>();

	public:
		pMap() {
			_table = new Node * [_capacity];
			for (size_t i = 0; i < _capacity; i++) {
				_table[i] = nullptr;
			}
		};

		pMap(pMap& other) {

		}

		pMap(pMap&& other) {

		}

		~pMap() {

		}

		pMap& operator=(const pMap& other) {

		}

		pMap& operator=(pMap&& other) {

		}
			 
		value_type& operator[](const key_type key) {
			address_type bucket_id = nspHashable::hash<address_type, key_type>(key);

			//alt workflow for first node

			// head
			Node* cur = _table[bucket_id];
			if (cur == nullptr) {
				// create new node, create new pair for node, assign node to container
				cur->_pair = new Pair<key_type, value_type>{key};
				return cur->_pair->second();
			}

			if (cur->_pair->first() == key) {
				return cur->_pair->second();
			}

			// others
			while (!cur->is_last()) {
				cur = cur->get_next();
				if (cur == nullptr) {
					cur->_pair = new Pair<key_type, value_type>{ key };
					return cur->_pair->second();
				}
				if (cur->_pair->first() == key) {
					return cur->_pair->second();
				}
			}

			cur->_pair = new Pair<key_type, value_type>{ key };
			return cur->_pair->second();
		}

		bool empty() const noexcept {
			return begin() == end();
		}

		size_t size() const {						// mark noexcept after test
			size_t count = 0;
			for (auto x : (*this)) {
				count++;
			}
			return count;
		}

		bool contains(const key_type& key) const {
			address_type bucket_id = nspHashable::hash<address_type, key_type>(key);

			// head
			Node* cur = _table[bucket_id];
			if (cur == nullptr) {
				return false;
			}

			if (cur->_pair->first() == key) {
				return true;
			}

			// others
			while (!cur->is_last()) {
				cur = cur->get_next();
				if (cur == nullptr) {
					return false;
				}
				if (cur->_pair->first() == key) {
					return true;
				}
			}

			return false;
		}

		struct Iterator {
			Node* _ptr;

		public:
			Iterator(Node* ptr) {
				_ptr = ptr;
			}

			Node& operator*() { return *(_ptr->_pair); }
			Node* operator->() { return _ptr->_pair; }
			Iterator& operator++() { _ptr = _ptr->get_next(); return *this; }
			Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
			friend bool operator== (const Iterator& a, const Iterator& b) { return a._ptr == b._ptr; };
			friend bool operator!= (const Iterator& a, const Iterator& b) { return a._ptr != b._ptr; };
		};

		Iterator begin() const noexcept {
			return Iterator(_first);
		}

		Iterator end() const noexcept {
			return Iterator(nullptr);
		}

	};


}


