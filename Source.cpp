//#include "parser/JSON/GeoJSONConverter.h"
#include "utils/pArray.h"
#include "utils/pString.h"
#include "utils/is_pointer.h"
#include "utils/pMap.h"
#include "utils/pUTF8String.h"


template <class T>
using Array = nspArray::pArray<T>;
using String = nspString::pString;
template <class T>
using Map = nspMap::pMap<T>;

int main() {



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

	auto m = new Map<int>();
	m->set(String("a"), 1);

	auto out = m->get(String("a"));

	printf("%i\n", out);

	delete m;

	return 0;
}