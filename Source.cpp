//#include "parser/JSON/GeoJSONConverter.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "container/pArray.h"
#include "container/pString.h"
#include "utils/is_pointer.h"
#include "container/pMap.h"
#include "container/pPair.h"


template <class T>
using Array = nspArray::pArray<T>;
using String = nspString::pString;
template <class T, class U>
using Map = nspMap::pMap<T, U>;

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	
	auto map = new Map<String, int>();

	(*map)["aaa"] = 10;

	printf("%i\n", (*map)["aaa"]);

	delete map;

	printf("ok");

	/*
	pArray<GJFeature*> a;
	GeoJSONConverter* conv = new GeoJSONConverter("test/utf8_test.json");
	conv->convert();
	if (!conv->made_error()) {
		a = conv->vectorize();
	}
	delete conv;

	auto value = a[0]->get_properties()->get().get("description");
	auto tmp = dynamic_cast<JSONString*>(value)->get().c_str();

	printf("%s\n", tmp);

	for (auto x : a) {
		x->dispose_deep();
		delete x;
		x = nullptr;
	}*/

	/*pArray<int> a = pArray<int>();

	for (int i = 0; i < 100; i += 3) {
		a.push_back(i);
	}

	int size = a.size();
	 
	for (auto x : a) {
		printf("%i\n", x);
	}*/

	// a.clear();

	/*pMap<int> a = pMap<int>();
	a.set(pString("halabala"), 10);
	a.set(pString("brblal"), 69);
	a.set(pString("Ganjaweasel"), 69);
	a.set(pString("420butastring"), 420);
	a.set(pString("halabala"), 5);

	int tmp = NULL;
	a.get(pString("halabala"), tmp);
	printf("%i\n", tmp);


	for (pNode<int> x : a) {
		printf("%i\n", x.second);
	}*/

	//auto m = new Map<int>();
	//m->set(String("a"), 1);
	//m->set(String("b"), 2);
	//m->set(String("c"), 3);

	//int out = int{};
	//String out_s = String{};

	//for (auto x : *m) {
	//	out_s = x.first;
	//	printf("%s\n", out_s.c_str());
	//	out = x.second;
	//	printf("%i\n", out);
	//}


	//delete m;
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtDumpMemoryLeaks();
	return 0;
}