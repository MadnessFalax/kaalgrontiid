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
template <class T, class U, class V = unsigned short>
using Map = nspMap::pMap<T, U, V>;

// used for out of scope stack disposal check so that _CrtDumpMemoryLeaks doesnt show false positive leaks on stack allocated memory
Map<int, int, unsigned char>* helper() {
	auto* a = new Map<int, int, unsigned char>();

	for (int i = 0; i < 300; i++) {
		(*a)[i] = i;
	}


	return a;
}

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	
	auto res = helper();

	for (auto x : *res) {
		printf("%i: %i with key: %c\n", x.first(), x.second(), nspHashable::hash<unsigned char, int>(x.first()));
	}

	delete res;

	printf("disposed");

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtDumpMemoryLeaks();
	return 0;
}