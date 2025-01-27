#pragma once
#include "../../../container/pMap.h"
#include "../../../container/pArray.h"

namespace nspNFA {

	template<class key_type, class value_type, class address_type = unsigned short>
	using Map = nspMap::pMap<key_type, value_type, address_type>;
	template<class type>
	using Array = nspArray::pArray<type>;

	class pState {
		bool _is_final = false;
		Map<char, pState*>* _transitions = new Map<char, pState*>();
		Array<pState*>* _epsilon_transitions = new Array<pState*>();

		pState() {};
		pState(pState& other) : _is_final(other._is_final) {
			delete _transitions;
			_transitions = new Map<char, pState*>(*other._transitions);
			_epsilon_transitions = new Array<pState*>(*other._epsilon_transitions);
		}

		pState(pState&& other) : _is_final(other._is_final) {
			delete _transitions;
			_transitions = other._transitions;
			other._transitions = nullptr;
			_epsilon_transitions = other._epsilon_transitions;
			other._epsilon_transitions = nullptr;
		}

		~pState() {
			delete _transitions;
			_transitions = nullptr;
			delete _epsilon_transitions;
			_epsilon_transitions = nullptr;
		}

		pState& operator=(pState& other) {
			if (this != &other) {
				_is_final = other._is_final;
				delete _transitions;
				_transitions = new Map<char, pState*>(*other._transitions);
				_epsilon_transitions = new Array<pState*>(*other._epsilon_transitions);
			}
			
			return *this;
		}

		pState& operator=(pState&& other) {
			if (this != &other) {
				_is_final = other._is_final;
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