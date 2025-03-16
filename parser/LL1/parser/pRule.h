#pragma once
#include "../../../container/pString.h"	
#include "../../../container/pArray.h"
#include "../../../abstract/pHasId.h"
#include "pSequence.h"
#include "pEpsilonException.h"

namespace nspParser {
	using String = nspString::pString;
	template<class T>
	using Array = nspArray::pArray<T>;

	template<typename enum_t, typename enum_r>
	class pRule : public nspHasId::pHasId<enum_r> {
		using Sequence = nspParser::pSequence<enum_t, enum_r>;
		enum_r _lhs = "";
		Array<Sequence*> _rhs = Array<Sequence*>();
		String _name = "";					// for printing purposes
		bool _has_epsilon = false;

	public:
		pRule(enum_r rule_id, String name) : _lhs(rule_id), _name(name) {
			size_t empty_count = 0;
			auto rhs_size = _rhs.size();
			for (size_t i = 0; i < rhs_size; i++) {
				if (_rhs[i]->is_empty()) {
					if (i == 0) {
						// it is not allowed to pass empty sequence as the first sequence
						throw pEpsilonException();
					}

					empty_count++;
				}
			}
			if (empty_count > 0) {
				_has_epsilon = true;
				if (empty_count > 1) {
					// it is not allowed to pass more than one empty sequence
					throw pEpsilonException();
				}
			}
		}

		~pRule() {
			auto rhs_size = _rhs.size();
			for (size_t i = 0; i < rhs_size; i++) {
				delete _rhs[i];
				_rhs[i] = nullptr;
			}
		}

		enum_r get_id() const override {
			return _lhs;
		}

		// takes ownership of seq
		void add_sequence(Sequence* seq) {
			_rhs.push_back(seq);
		}

		Array<Sequence*>& get_rhs() {
			return _rhs;
		}

		pRule& operator+=(Sequence* seq) {
			add_sequence(seq);
			return *this;
		}

		String get_name() const {
			return _name;
		}

		bool has_epsilon() const {
			return _has_epsilon;
		}
	};
}