//#include "parser/JSON/GeoJSONConverter.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "container/pArray.h"
#include "container/pString.h"
#include "container/pMap.h"
#include "container/pPair.h"

#include "exporter/buffer/pFileWriter.h"

template <class T>
using Array = nspArray::pArray<T>;
using String = nspString::pString;
template <class T, class U, class V = unsigned char>
using Map = nspMap::pMap<T, U, V>;
using FileWriter = nspFile::pFileWriter;

// used for out of scope stack disposal check so that _CrtDumpMemoryLeaks doesnt show false positive leaks on stack allocated memory
static void helper() {

	String path = ".\\fajny_soubor.bat";

	auto* fw = new FileWriter(path);
	fw->write("pause\r\n");
	fw->write("echo Hello world, but in indian accent.\r\n");

	delete fw;
	fw = nullptr;

	return;
}

int main() {
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif

	helper();

#ifdef _DEBUG
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}