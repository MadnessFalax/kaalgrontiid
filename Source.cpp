//#include "parser/JSON/GeoJSONConverter.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "container/pArray.h"
#include "container/pString.h"
#include "container/pMap.h"
#include "container/pPair.h"
#include "parser/GeoJSON/gjParser.h"
#include "parser/KML/kmlParser.h"
#include "parser/OSM/osmParser.h"

#include "cLineString.h"
#include "cPolygon.h"
#include "cSphere.h"

#include "dstruct/paged/core/cNodeCache.h"
#include "dstruct/paged/core/cDStructConst.h"
#include "dstruct/paged/sequentialarray/cSequentialArray.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayContext.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayHeader.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayNodeHeader.h"

template <class T>
using Array = nspArray::pArray<T>;
using String = nspString::pString;
template <class T, class U, class V = unsigned char>
using Map = nspMap::pMap<T, U, V>;
template<class enum_t>
using Lexer = nspLexer::pLexer<enum_t>;

using Regex = nspRegex::pRegex;
template<class enum_t, nspLexer::PrototypeKind t = nspLexer::PrototypeKind::DEFAULT>
using Token = nspLexer::pTokenPrototype<enum_t, t>;

using Tkey = cNTuple;
using SeqArray = dstruct::paged::sqarray::cSequentialArray<Tkey>;
using SeqArrayContext = dstruct::paged::sqarray::cSequentialArrayContext<Tkey>;
using SeqArrayHeader = dstruct::paged::sqarray::cSequentialArrayHeader<Tkey>;

#ifndef _WORK
constexpr auto PATH = R"(C:\Users\Petr\Downloads\src\test\kaalgrontiid\test\example.kml)";
#endif

#ifdef _WORK
constexpr auto PATH = R"(C:\Users\uiv56391\source\repos\framework-back-up\test\kaalgrontiid\test\frenstat.osm)";
#endif


// used for out of scope stack disposal check so that _CrtDumpMemoryLeaks doesnt show false positive leaks on stack allocated memory
static void helper() {
	//const double vals[3] = {3.1235, 2.8644, 15.4655};
	//const double vals2[3] = {1.65465, 2.6465, 10.465165};
	//const double vals3[3] = {12.465, .6646, 5.486};

	//cSpaceDescriptor* space_desc_2d = new cSpaceDescriptor(DIMENSION_2, new cTuple(), new cInt());
	//cSpaceDescriptor* space_desc_3d = new cSpaceDescriptor(DIMENSION_3, new cTuple(), new cInt());

	//auto** line_vtcs = new cTuple * [3];

	//for (size_t i = 0; i < 3; i++) {
	//	line_vtcs[i] = new cTuple(space_desc_3d);
	//	line_vtcs[i]->SetValue(0, (int) vals[i], space_desc_3d);
	//	line_vtcs[i]->SetValue(1, (int) vals2[i], space_desc_3d);
	//	line_vtcs[i]->SetValue(2, (int) vals3[i], space_desc_3d);
	//}

	//cDataShape<cTuple>* linestring = cDataShape<cTuple>::CreateDataShape(DataShape::DS_LINESTRING, line_vtcs, 3, space_desc_3d);

	//linestring->PrintAllTuples(space_desc_3d);

	//delete space_desc_2d;
	//space_desc_2d = nullptr;
	//delete space_desc_3d;
	//space_desc_3d = nullptr;


	// KML Parser test ----------------
	String path = PATH;
	auto* p = nsOSM::setup_parser();
	p->open(path);
	while (p->get_item()) {
		// do nothing
		printf("got value \n");
	}

	delete p;
	// ------------------------------------

	// KML Parser test ----------------
	//String path = PATH;
	//auto* p = nsKML::setup_parser();
	//p->open(path);
	//while (p->get_item()) {
	//	// do nothing
	//	printf("got value \n");
	//}

	//delete p;
	// ------------------------------------

	// GeoJSON Parser test ----------------
	//String path = PATH;
	//auto* p = nsGeoJSON::setup_parser();
	//p->open(path);
	//while (p->get_item()) {
	//	// do nothing
	//	printf("got value \n");
	//}

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