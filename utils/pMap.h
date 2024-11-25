#pragma once
#include "pString.h"
#include "is_pointer.h"
/**
* 
*	Reconsider implementing ctor triplet
* 
*/


namespace nspMap {

	constexpr size_t CAPACITY = 65521;

	template <class Tval>
	class pNode {
		bool last;

	public:
		using value_type = Tval;
		using reference = Tval&;
		using const_reference = const Tval&;
		using const_type = const Tval;

		value_type second;
		nspString::pString first;
		pNode* next;

		pNode() = delete;

		pNode(nspString::pString first) {
			this->first = first;
			second = value_type{};
			next = nullptr;
			last = true;
		}

		pNode(const pNode& other) = delete;

		pNode(pNode&& other) = delete;

		~pNode() {
			this->next = nullptr;
		}

		void operator= (const_type other) noexcept {
			second = other;
		}

		const bool& is_last() const noexcept {
			return last;
		}

		void insert_next(pNode* other) noexcept {
			if (this->last) {
				if (next != nullptr) {
					other->next = this->next;
				}
				next = other;
				this->last = false;
			}
			else {
				next->insert_next(other);
			}
		}

		pNode* get_next() const noexcept {
			return next;
		}

		// deletes all nodes pointed recursively
		void delete_list() {
			if (next != nullptr) {
				next->delete_list();
			}
			delete next;
			next = nullptr;
		}

	};

	template <class Tval>
	class pMap
	{
	public:
		using value_type = Tval;
		using reference = Tval&;
		using pointer = Tval*;
		using const_type = const Tval;
		using const_reference = const Tval&;

	private:
		pNode<value_type>** table;
		pNode<value_type>* first;
		pNode<value_type>* first_branch_end;					// for new branch insertion

	public:
		pMap() {
			table = new pNode<value_type>*[CAPACITY];
			for (size_t i = 0; i < CAPACITY; i++) {
				table[i] = nullptr;
			}
			first = nullptr;
			first_branch_end = nullptr;
		}

		// creates new 
		pMap(const pMap& other) {
			table = new pNode<value_type>*[CAPACITY];
			for (size_t i = 0; i < CAPACITY; i++) {
				table[i] = nullptr;
			}
			first = nullptr;
			first_branch_end = nullptr;

			for (pNode<value_type> other_pair : other) {
				this->set(nspString::pString(other_pair.first), other_pair.second);
			}
		}

		pMap(pMap&& other) noexcept {
			this->table = other.table;
			other.table = nullptr;
			this->first = other.first;
			other.first = nullptr;
			this->first_branch_end = other.first_branch_end;
			other.first_branch_end = nullptr;
		}

		~pMap() {
			if (first != nullptr) {
				first->delete_list();
			}
			delete first;
			first = nullptr;
			first_branch_end = nullptr;
			for (size_t i = 0; i < CAPACITY; i++) {
				table[i] = nullptr;
			}
			delete[] table;
			table = nullptr;
		}

		void operator=(const pMap& other) {
			if (this != &other) {
				table = new pNode<value_type>*[CAPACITY];
				for (size_t i = 0; i < CAPACITY; i++) {
					table[i] = nullptr;
				}

				first = nullptr;
				first_branch_end = nullptr;

				for (pNode<value_type> other_pair : other) {
					this->set(nspString::pString(other_pair.first), other_pair.second);
				}
			}
		}

		void operator=(pMap&& other) noexcept {
			this->table = other.table;
			other.table = nullptr;
			this->first = other.first;
			other.first = nullptr;
			this->first_branch_end = other.first_branch_end;
			other.first_branch_end = nullptr;
		}

		void set(nspString::pString key, const value_type value) {
			unsigned short hash = key.get_hash();

			pNode<value_type>* branch = table[hash];

			if (branch == nullptr) {
				auto tmp = new pNode<value_type>(key);
				tmp->second = value;

				if (first == nullptr) {
					first = tmp;
					first_branch_end = tmp;
				}
				else {
					tmp->next = first_branch_end->next;
					first_branch_end->next = tmp;
				}

				table[hash] = tmp;
				return;
			}
			else {
				while (branch->first != key && !branch->is_last()) {
					if (branch->get_next() == nullptr) {
						break;
					}

					branch = branch->get_next();
				}

				if (branch->first == key) {
					branch->second = value;
				}
				else {
					auto tmp = new pNode<value_type>(key);
					tmp->second = value;
					branch->insert_next(tmp);

					if (branch = first_branch_end) {
						first_branch_end = tmp;
					}
				}
			}

		}

		reference get(nspString::pString key) const {
			unsigned short hash = key.get_hash();

			pNode<value_type>* branch = table[hash];

			if (branch == nullptr) {
				// Key not found
				throw;
			}

			while (branch->first != key && !branch->is_last()) {
				if (branch->get_next() == nullptr) {
					break;
				}

				branch = branch->get_next();
			}

			if (branch->first == key) {
				return branch->second;
			}
			else {
				// Key not found
				throw;
			}
		}

		bool contains_key(nspString::pString key) const {
			int hash = key.get_hash();

			pNode<value_type>* branch = table[hash];

			while (branch->get_key() != key && !branch->is_last()) {
				branch = branch->get_next();
				if (branch == nullptr) {
					return false;
				}
			}

			if (branch->get_key() == key) {
				return true;
			}
			else
				return false;
		}

		struct Iterator {
			pNode<value_type>* m_ptr;

		public:
			Iterator(pNode<value_type>* ptr) {
				m_ptr = ptr;
			}

			pNode<value_type>& operator*() const { return *m_ptr; }
			pNode<value_type>* operator->() { return m_ptr; }
			Iterator& operator++() { m_ptr = m_ptr->get_next(); return *this; }
			Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
			friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
			friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };
		};

		Iterator begin() {
			return Iterator(first);
		}

		Iterator end() {
			return Iterator(nullptr);
		}

	};

};