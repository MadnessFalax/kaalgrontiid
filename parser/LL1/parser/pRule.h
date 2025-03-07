#pragma once
#include "../../../container/pString.h"	
#include "../../../container/pArray.h"
#include "pSequence.h"

namespace nspParser {
	using String = nspString::pString;
	template<class T>
	using Array = nspArray::pArray<T>;
	using Sequence = nspParser::pSequence;

	class pRule {
		String _lhs = "";
		Array<Sequence*> _rhs = Array<Sequence*>();

		pRule(String name) : _lhs(name) {
			
		}
	};
}