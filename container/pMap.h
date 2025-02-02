#pragma once
#include "pPair.h"
#include "../abstract/pHashable.h"


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

			Node() {};

			Node(Pair<key_type, value_type>* pair) : _pair(pair) {}

			// dtor only destructs this node, delete chained by calling delete_bucket()
			~Node() {
				delete _pair;
				_pair = nullptr;
			};

			const bool& is_last() const noexcept {
				return _last;
			}

			Node* get_next() const noexcept {
				return _next;
			}

			Node* get_bucket_last() const noexcept {
				Node* tmp = this;
				while (!tmp->is_last()) {
					tmp = tmp->get_next();
				}

				return tmp;
			}

			Node* get_map_last() const noexcept {
				Node* tmp = this;
				while (tmp->_next != nullptr) {
					tmp = tmp->get_next();
				}

				return tmp;
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
				if (_last) {
					_next = nullptr;
				}
				else {
					_next->delete_bucket();
					delete _next;
					_next = nullptr;
				}
				delete _pair;
				_pair = nullptr;
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
			_table = new Node * [_capacity];
			for (Pair<key_type, value_type> cur : other) {
				(*this)[cur.first()] = cur.second();
			}
		}

		pMap(pMap&& other) noexcept {
			_table = other._table;
			other._table = nullptr;

			_first = other._first;
			other._first = nullptr;

			_first_end = other._first_end;
			other._first_end = nullptr;
		}

		~pMap() {
			// pMap owns both Nodes, Pairs are owned by Nodes
			for (size_t i = 0; i < _capacity; i++) {
				if (_table[i] != nullptr) {
					_table[i]->delete_bucket();
					delete _table[i];
					_table[i] = nullptr;
				}
			}

			delete[] _table;
			_first = nullptr;
			_first_end = nullptr;

		}

		pMap& operator=(const pMap& other) {
			if (this != &other) {
				for (size_t i = 0; i < _capacity; i++) {
					if (_table[i] != nullptr) {
						_table[i]->delete_bucket();
						delete _table[i];
						_table[i] = nullptr;
					}
				}

				delete[] _table;
				_first = nullptr;
				_first_end = nullptr;

				_table = new Node * [_capacity];
				for (auto cur : other) {
					(*this)[cur.first()] = cur.second();
				}
			}

			return *this;
		}

		pMap& operator=(pMap&& other) {
			if (this != &other) {
				for (size_t i = 0; i < _capacity; i++) {
					if (_table[i] != nullptr) {
						_table[i]->delete_bucket();
						delete _table[i];
						_table[i] = nullptr;
					}
				}

				delete[] _table;
				_first = nullptr;
				_first_end = nullptr;

				_table = other._table;
				other._table = nullptr;

				_first = other._first;
				other._first = nullptr;

				_first_end = other._first_end;
				other._first_end = nullptr;
			}

			return *this;
		}
			 
		value_type& operator[](const key_type& key) {
			address_type bucket_id = nspHashable::hash<address_type, key_type>(key);

			// head
			Node* cur = _table[bucket_id];
			if (cur == nullptr) {
				// bucket empty -> become head of bucket
				Pair<key_type, value_type>* pair = new Pair<key_type, value_type>{ key };
				Node* node = new Node(pair);
				_table[bucket_id] = node;

				if (_first == nullptr) {
					// map empty -> become first bucket
					_first = _first_end = node;
				}
				else {
					// map not empty -> assign after first bucket
					node->_next = _first_end->_next;
					_first_end->_next = node;
				}

				return *(node->_pair->_second);
			}

			if (*(cur->_pair->_first) == key) {
				return *(cur->_pair->_second);
			}

			// others
			while (!cur->is_last()) {
				cur = cur->get_next();
				if (*(cur->_pair->_first) == key) {
					return *(cur->_pair->_second);
				}
			}

			Pair<key_type, value_type>* pair = new Pair<key_type, value_type>(key);
			Node* node = new Node(pair);
			node->_next = cur->_next;
			cur->_next = node;
			cur->_last = false;
			if (_table[bucket_id] == _first) {
				_first_end = node;
			}

			return *(node->_pair->_second);
		}

		bool empty() const noexcept {
			return begin() == end();
		}

		size_t size() const {						// mark noexcept after test

			size_t count = 0;
			for (auto x : (*this)) {
				count++;
			}
		}

		bool contains(const key_type& key) const {
			address_type bucket_id = nspHashable::hash<address_type, key_type>(key);

			// head
			Node* cur = _table[bucket_id];
			if (cur == nullptr) {
				return false;
			}

			if (cur->_pair->_first == key) {
				return true;
			}

			// others
			while (!cur->is_last()) {
				cur = cur->get_next();
				if (cur == nullptr) {
					return false;
				}
				if (cur->_pair->_first == key) {
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

			Pair<key_type, value_type>& operator*() { return *(_ptr->_pair); }
			Pair<key_type, value_type>* operator->() { return _ptr->_pair; }
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


