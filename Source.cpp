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


template <class T>
using Array = nspArray::pArray<T>;
using String = nspString::pString;
template <class T, class U>
using Map = nspMap::pMap<T, U>;

// used for out of scope stack disposal check so that _CrtDumpMemoryLeaks doesnt show false positive leaks on stack allocated memory
void helper() {
	auto* a = new Array<String>{};

	a->push_back(String{"vykur si!"});

	auto temp = nspHashable::hash<unsigned short, Array<String>>(a);
	
	printf("%u", temp);

	delete a;

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