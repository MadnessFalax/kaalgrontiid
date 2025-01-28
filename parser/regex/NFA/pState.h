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
		Map<char, Array<pState*>>* _transitions = nullptr;
		Array<pState*>* _epsilon_transitions = nullptr;

	public:
		pState() : 
			_transitions(new Map<char, Array<pState*>>()), 
			_epsilon_transitions(new Array<pState*>()) 
		{};

		pState(pState& other) : 
			_is_final(other._is_final), 
			_transitions(new Map<char, Array<pState*>>(*other._transitions)),
			_epsilon_transitions (new Array<pState*>(*other._epsilon_transitions))
		{}

		pState(pState&& other) noexcept : 
			_is_final(other._is_final),
			_transitions(other._transitions),
			_epsilon_transitions(other._epsilon_transitions)
		{
			other._transitions = nullptr;
			other._epsilon_transitions = nullptr;
		}

		Array<pState*>& register_transition(const char token, pState* to_state) {
			auto& transition_set = (*_transitions)[token];
			transition_set.push_back(to_state);

			return transition_set;
		}

		Array<pState*>& register_epsilon(pState* to_state) {
			auto& transition_set = (*_epsilon_transitions);
			transition_set.push_back(to_state);

			return transition_set;
		}

		Array<pState*>& consume(const char input_token) {

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
				_transitions = new Map<char, Array<pState*>>(*other._transitions);
				_epsilon_transitions = new Array<pState*>(*other._epsilon_transitions);
			}
			
			return *this;
		}

		pState& operator=(pState&& other) noexcept {
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