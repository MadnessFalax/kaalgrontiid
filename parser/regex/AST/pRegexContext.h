#pragma once
#include "../NFA/pState.h"
#include "../../../container/pArray.h"

namespace nspRegexAST {

	class pRegexContext {
		using State = nspNFA::pState;
		template<class value_type>
		using Array = nspArray::pArray<value_type>;


	};
}