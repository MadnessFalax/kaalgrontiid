//#include "parser/JSON/GeoJSONConverter.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "container/pArray.h"
#include "container/pString.h"
#include "container/pMap.h"
#include "container/pPair.h"
#include "abstract/pHashable.h"
#include "parser/regex/pRegex.h"
#include "utils/match.h"
#include "parser/LL1/lexer/pLexer.h"
#include "parser/GeoJSON/gjParser.h"
#include "parser/LL1/parser/node/pConsumeNode.h"
#include "parser/KML/kmlParser.h"

template <class T>
using Array = nspArray::pArray<T>;
using String = nspString::pString;
template <class T, class U, class V = unsigned short>
using Map = nspMap::pMap<T, U, V>;
template<class enum_t>
using Lexer = nspLexer::pLexer<enum_t>;

using Regex = nspRegex::pRegex;
template<class enum_t, nspLexer::PrototypeKind t = nspLexer::PrototypeKind::DEFAULT>
using Token = nspLexer::pTokenPrototype<enum_t, t>;


// used for out of scope stack disposal check so that _CrtDumpMemoryLeaks doesnt show false positive leaks on stack allocated memory
static void helper() {
	
	// KML Parser test ----------------
	//String path = R"(C:\Users\Petr\Downloads\src\test\kaalgrontiid\test\example.kml)";
	//auto* p = nsKML::setup_parser();
	//p->open(path);
	//p->parse();

	//delete p;
	// ------------------------------------


	// GeoJSON Parser test ----------------
	//String path = R"(C:\Users\Petr\Downloads\src\test\kaalgrontiid\test\sample_geo.json)";
	//auto* p = nsGeoJSON::setup_parser();
	//p->open(path);
	//p->parse();

	//delete p;
	// ------------------------------------

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