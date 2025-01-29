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

template <class T>
using Array = nspArray::pArray<T>;
using String = nspString::pString;
template <class T, class U, class V = unsigned short>
using Map = nspMap::pMap<T, U, V>;

// used for out of scope stack disposal check so that _CrtDumpMemoryLeaks doesnt show false positive leaks on stack allocated memory
static void helper() {
	auto tmp = nspRegex::pRegex::compile(R"(^-?(0|[1-9]\d*)(\.\d+)?([eE][+-]?\d+)?$)");			// json number

	auto state = nspNFA::pState();

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