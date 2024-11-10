#pragma once
#include "pString.h"

/**
* 
*	Reconsider setting limits for buckets!
* 
*/


namespace nspMap {

	constexpr size_t CAPACITY = 65535;

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
			this->second = NULL;
			next = nullptr;
			last = true;
		}

		pNode(const pNode& other) = delete;

		pNode(pNode&& other) = delete;

		~pNode() {
			this->next = nullptr;
		}

		void operator= (const_type other) {
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

		const pNode* get_next() const noexcept {
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
		pNode<value_type>* first_branch_end;
		bool table_moved;							// is this necesarry????????????

	public:
		pMap() {
			table = new pNode<value_type>*[CAPACITY];
			for (size_t i = 0; i < CAPACITY; i++) {
				table[i] = nullptr;
			}

			table_moved = false;
			first = nullptr;
			first_branch_end = nullptr;
		}

		// creates new 
		pMap(const pMap& other) {
			table = new pNode<value_type>*[CAPACITY];
			for (size_t i = 0; i < CAPACITY; i++) {
				table[i] = nullptr;
			}

			table_moved = false;
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
			table_moved = false;						// ??
			other.table_moved = true;					// ??

		}

		~pMap() {
			if (!table_moved) {
				clear();
			}
			delete[] table;
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
			other.table_moved = true;
		}


		// TODO: check this out
		void set(nspString::pString key, value_type value) {
			int hash = key.get_hash();

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

				auto current = branch;

				while (current->first != key && !current->is_last()) {
					if (current->get_next() == nullptr) {
						break;
					}

					current = current->get_next();
				}

				if (current->first == key) {
					current->second = value;
				}
				else {

					auto tmp = new pNode<Tval>(key);
					tmp->second = value;
					current->insert_next(tmp);

					if (current = first_branch_end) {
						first_branch_end = tmp;
					}
				}
			}

		}

		Tval& get(pString key) {
			int hash = key.get_hash();

			pNode<Tval>* branch = table[hash];

			if (branch == nullptr) {
				// Key not found
				throw;
			}

			auto current = branch;

			while (current->first != key && !current->is_last()) {
				if (current->get_next() == nullptr) {
					break;
				}

				current = current->get_next();
			}

			if (current->first == key) {
				return current->second;
			}
			else {
				// Key not found
				throw;
			}
		}

		// IMPORTANT TO CHECK THIS OUT AGAIN!!!!!!!!!
		void clear() {
			if (first != nullptr) {
				first->delete_list();
				first_branch_end = nullptr;
				delete first;
				first = nullptr;
			}
			for (int i = 0; i < CAPACITY; i++) {
				table[i] = nullptr;
			}
		}

		bool contains_key(pString key) {
			int hash = key.get_hash();

			pNode<Tval>* branch = table[hash];

			auto current = branch;

			while (current->get_key() != key && !current->is_last()) {
				current = current->get_next();
				if (current == nullptr) {
					return false;
				}
			}

			if (current->get_key() == key) {
				return true;
			}
			else
				return false;
		}

		struct Iterator {
			pNode<Tval>* m_ptr;

		public:
			Iterator(pNode<Tval>* ptr) {
				m_ptr = ptr;
			}

			pNode<Tval>& operator*() const { return *m_ptr; }
			pNode<Tval>* operator->() { return m_ptr; }
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