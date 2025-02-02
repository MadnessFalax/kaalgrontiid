//#include "parser/JSON/GeoJSONConverter.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "container/pArray.h"
#include "container/pString.h"
#include "utils/is_pointer.h"
#include "container/pMap.h"
#include "container/pPair.h"
#include "abstract/pHashable.h"
#include "parser/regex/pMatch.h"
#include "parser/regex/NFA/pState.h"
#include "parser/regex/pRegex.h"
#include "parser/regex/NFA/pAutomaton.h"

template <class T>
using Array = nspArray::pArray<T>;
using String = nspString::pString;
template <class T, class U, class V = unsigned short>
using Map = nspMap::pMap<T, U, V>;

using State = nspNFA::pState;

// used for out of scope stack disposal check so that _CrtDumpMemoryLeaks doesnt show false positive leaks on stack allocated memory
static void helper() {
	auto tmp = nspRegex::pRegex::compile(R"(^-?(0|[1-9]\d*)(\.\d+)?([eE][+-]?\d+)?$)");			// json number

	auto* start_state = new State();
	auto* a_state = new State();
	auto* at_state = new State();
	auto* b_state = new State();
	auto* dot_state = new State();
	auto* c_state = new State();
	auto* z_state = new State(true);

	start_state->register_transition('a', a_state);
	a_state->register_epsilon(start_state);
	a_state->register_transition('@', at_state);
	at_state->register_transition('b', b_state);
	b_state->register_epsilon(at_state);
	b_state->register_transition('.', dot_state);
	dot_state->register_transition('c', c_state);
	c_state->register_transition('z', z_state);

	auto* aut = new nspNFA::pAutomaton();

	Array<State*>* arr = new Array<State*>();

	arr->push_back(start_state);
	arr->push_back(a_state);
	arr->push_back(at_state);
	arr->push_back(b_state);
	arr->push_back(dot_state);
	arr->push_back(c_state);
	arr->push_back(z_state);

	aut->set_states(arr, start_state);
	
	printf("%i\n", aut->match("aaaa@bbbbbbb.cz"));

	delete aut;

	return;
}

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	
	helper();

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtDumpMemoryLeaks();
	return 0;
}