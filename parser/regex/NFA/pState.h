#pragma once
#include "../../../container/pMap.h"


namespace nspNFA {

	template<class key_type, class value_type, class address_type = unsigned short>
	using Map = nspMap::pMap<key_type, value_type, address_type>;

	class pState {
		bool _is_final = false;
		Map<char, pState*>* _transitions = new Map<char, pState*>();

		pState() {};
		pState(pState& other) : _is_final(other._is_final) {
			delete _transitions;
			_transitions = new Map<char, pState*>(*other._transitions);
		}

		pState(pState&& other) : _is_final(other._is_final) {
			delete _transitions;
			_transitions = other._transitions;
			other._transitions = nullptr;
		}

		~pState() {
			delete _transitions;
			_transitions = nullptr;
		}

		pState& operator=(pState& other) {
			if (this != &other) {
				_is_final = other._is_final;
				delete _transitions;
				_transitions = new Map<char, pState*>(*other._transitions);
			}
			
			return *this;
		}

		pState& operator=(pState&& other) {
			if (this != &other) {
				_is_final = other._is_final;
				delete _transitions;
				_transitions = other._transitions;
				other._transitions = nullptr;
			}

			return *this;
		}
	};
}