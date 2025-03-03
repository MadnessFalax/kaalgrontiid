#pragma once
#include "../../../container/pMap.h"
#include "../../../container/pArray.h"
#include "../../../abstract/pHasId.h"

namespace nspNFA {

	template<class key_type, class value_type, class address_type = unsigned short>
	using Map = nspMap::pMap<key_type, value_type, address_type>;
	template<class type>
	using Array = nspArray::pArray<type>;

	class pState : nspHasId::pHasId<unsigned int> {
		static unsigned int _id_counter;
		unsigned int _id;
		bool _is_final = false;;
		Map<char, Map<unsigned int, pState*, unsigned char>>* _transitions = nullptr;
		Map<unsigned int, pState*, unsigned char>* _epsilon_transitions = nullptr;

		unsigned int _get_id() { return _id_counter++; }

	public:
		pState() :
			_id(_get_id()),
			_transitions(new Map<char, Map<unsigned int, pState*, unsigned char>>(7)),
			_epsilon_transitions(new Map<unsigned int, pState*, unsigned char>(7))
		{};

		pState(bool is_final) :
			_id(_get_id()),
			_is_final(is_final),
			_transitions(new Map<char, Map<unsigned int, pState*, unsigned char>>(7)),
			_epsilon_transitions(new Map<unsigned int, pState*, unsigned char>(7))
		{};

		pState(pState& other) :
			_id(_get_id()),
			_is_final(other._is_final),
			_transitions(new Map<char, Map<unsigned int, pState*, unsigned char>>(*other._transitions)),
			_epsilon_transitions (new Map<unsigned int, pState*, unsigned char>(*other._epsilon_transitions))
		{}

		pState(pState&& other) noexcept :
			_id(other._id),
			_is_final(other._is_final),
			_transitions(other._transitions),
			_epsilon_transitions(other._epsilon_transitions)
		{
			other._id = 0;
			other._transitions = nullptr;
			other._epsilon_transitions = nullptr;
		}

		void set_final() { _is_final = true; }
		unsigned int get_id() const { return _id; }
		bool is_final() const { 
			if (_is_final) {
				return true;
			}
			else {
				for (auto& pair : *_epsilon_transitions) {
					if (pair.second()->is_final()) {
						return true;
					}
				}
			}

			return false;
		}

		void register_transition(const char token, pState* to_state) {
			(*_transitions)[token][to_state->get_id()] = to_state;
		}

		void register_epsilon(pState* to_state) {
			(*_epsilon_transitions)[to_state->get_id()] = to_state;
		}

		Map<unsigned int, pState*, unsigned char> consume(
			const char input_token,
			Map<unsigned int, pState*, unsigned char>* closure = new Map<unsigned int, pState*, unsigned char>(7),
			bool closure_owner = true
		) {
			auto ret_val = Map<unsigned int, pState*, unsigned char>();

			if (closure->contains(get_id())) {
				return ret_val;
			}

			(*closure)[get_id()] = this;

			for (auto pair : *_epsilon_transitions) {
				auto temp = pair.second()->consume(input_token, closure, false);
				for (auto& res : temp) {
					ret_val[res.first()] = res.second();
				}
			}

			for (auto pair : (*_transitions)[input_token]) {
				ret_val[pair.first()] = pair.second();
			}

			if (closure_owner) {
				delete closure;
			}

			return ret_val;
		}

		~pState() {
			delete _transitions;
			_transitions = nullptr;
			delete _epsilon_transitions;
			_epsilon_transitions = nullptr;
		}

		pState& operator=(pState& other) {
			if (this != &other) {
				_id = other._get_id();
				_is_final = other._is_final;
				delete _transitions;
				_transitions = new Map<char, Map<unsigned int, pState*, unsigned char>>(*other._transitions);
				_epsilon_transitions = new Map<unsigned int, pState*, unsigned char>(*other._epsilon_transitions);
			}
			
			return *this;
		}

		pState& operator=(pState&& other) noexcept {
			if (this != &other) {
				_id = other._id;
				_is_final = other._is_final;
				other._id = 0;
				delete _transitions;
				_transitions = other._transitions;
				other._transitions = nullptr;
				_epsilon_transitions = other._epsilon_transitions;
				other._epsilon_transitions = nullptr;
			}

			return *this;
		}
	};
}