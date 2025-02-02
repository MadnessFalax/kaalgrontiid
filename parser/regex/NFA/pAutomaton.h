#pragma once
#include "pState.h"
#include "../../../container/pArray.h"
#include "exception/pNFAUndefinedException.h"
#include <cstring>

namespace nspNFA {

	class pAutomaton {

		template<class key_type, class value_type, class address_type = unsigned short>
		using Map = nspMap::pMap<key_type, value_type, address_type>;

		template<class value_type>
		using Array = nspArray::pArray<value_type>;

		Array<pState*>* _all_state_closure = nullptr;
		pState* _starting_state = nullptr;
		
		void _perform_state_check() {
			if (_starting_state == nullptr || _all_state_closure == nullptr) {
				throw new pNFAUndefinedException();
			}
		}

	public:
		pAutomaton() {};

		// automaton takes ownership of passed objects
		pAutomaton(Array<pState*>* all_state_closure, pState* starting_state) : _all_state_closure(all_state_closure), _starting_state(starting_state)
		{}

		pAutomaton(pAutomaton& other) = delete;

		pAutomaton(pAutomaton&& other) noexcept : 
			_all_state_closure(other._all_state_closure), 
			_starting_state(other._starting_state) 
		{
			other._starting_state = nullptr;
			other._all_state_closure = nullptr;
		}

		~pAutomaton() {
			_starting_state = nullptr;

			if (_all_state_closure != nullptr) {
				size_t size = _all_state_closure->size();
				for (size_t i = 0; i < size; i++) {
					delete (*_all_state_closure)[i];
					(*_all_state_closure)[i] = nullptr;
				}

				delete _all_state_closure;
				_all_state_closure = nullptr;
			}
		}

		bool match(const char* input) {
			_perform_state_check();
			if (_starting_state->is_final())
				return true;

			size_t length = strlen(input);

			auto* current_states = new Map<unsigned int, pState*, unsigned char>();
			(*current_states)[_starting_state->get_id()] = _starting_state;

			for (size_t i = 0; i < length; i++) {
				auto* new_states = new Map<unsigned int, pState*, unsigned char>();

				for (auto& state : *current_states) {
					auto temp_states = state.second()->consume(input[i]);
					for (auto& temp_state : temp_states) {
						(*new_states)[temp_state.first()] = temp_state.second();
					}
				}

				if (new_states->size() == 0) {
					delete current_states;
					delete new_states;
					current_states = nullptr;
					new_states = nullptr;
					return false;
				}

				delete current_states;
				current_states = new_states;
				new_states = nullptr;
			}

			bool found_match = false;

			for (auto& state : *current_states) {
				if (state.second()->is_final()) {
					found_match = true;
				}
			}

			delete current_states;

			return found_match;
		}

		// automaton takes ownership of passed objects
		void set_states(Array<pState*>* all_states_closure, pState* starting_state) {
			if (_starting_state != nullptr) {
				_starting_state = nullptr;
			}

			if (_all_state_closure != nullptr) {
				size_t size = _all_state_closure->size();
				for (size_t i = 0; i < size; i++) {
					delete (*_all_state_closure)[i];
					(*_all_state_closure)[i] = nullptr;
				}

				delete _all_state_closure;
				_all_state_closure = nullptr;
			}
			
			_all_state_closure = all_states_closure;
			_starting_state = starting_state;
		}

		pAutomaton& operator=(pAutomaton&) = delete;
		
		pAutomaton& operator=(pAutomaton&& other) noexcept {
			if (this != &other) {
				_starting_state = other._starting_state;
				_all_state_closure = other._all_state_closure;

				other._starting_state = nullptr;
				other._all_state_closure = nullptr;
			}

			return *this;
		}

	};
}