#pragma once
#include "../../../container/pString.h"	
#include "../../../container/pArray.h"
#include "../../../abstract/pHasId.h"
#include "pSequence.h"

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

	public:
		pRule(enum_r rule_id, String name) : _lhs(rule_id), _name(name) {}

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

		const Array<Sequence*>& get_rhs() const {
			return _rhs;
		}

		pRule& operator+=(Sequence* seq) {
			add_sequence(seq);
			return *this;
		}
	};
}