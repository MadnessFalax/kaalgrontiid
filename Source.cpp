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
constexpr auto PATH = R"(C:\Users\Petr\Downloads\src\test\kaalgrontiid\test\short.osm)";
#endif

#ifdef _WORK
constexpr auto PATH = R"(C:\Users\uiv56391\source\repos\framework-back-up\test\kaalgrontiid\test\example.kml)";
#endif

static void print_shape_info() {

}

// used for out of scope stack disposal check so that _CrtDumpMemoryLeaks doesnt show false positive leaks on stack allocated memory
static void helper() {

	auto* space_desc_3d = new cSpaceDescriptor(DIMENSION_3, new cNTuple(), new cDouble());
	auto* context = new SeqArrayContext();
	auto* header = new SeqArrayHeader("seqArray", 8192, space_desc_3d, cDStructConst::DSMODE_DEFAULT);
	header->SetCodeType(ELIAS_DELTA);

	cQuickDB* db = new cQuickDB();
	db->Create("test", 500, 10000, 8192);

	auto* seq_array = new SeqArray();
	seq_array->Create(header, db);

	unsigned int node_id, position;

	cNTuple** tuple_arr = nullptr;
	unsigned int tuple_size = 0;

	String path = PATH;

#ifdef _OSM
	auto* p = nsOSM::setup_parser();
#endif

#ifdef _GEOJS
	auto* p = nsGeoJSON::setup_parser();
#endif

#ifdef _KML
	auto* p = nsKML::setup_parser();
#endif

	cLineString<cNTuple>* ls = nullptr;
	cPolygon<cNTuple>* poly = nullptr;
	cSphere<cNTuple>* sphere = nullptr;
	cNTuple* point = nullptr;

	p->open(path);
	cDataType* item = nullptr;
	while (item = p->get_item()) {
		// do nothing

		switch (p->get_shape_type()) {
		case DataShape::DS_POINT:
			point = static_cast<cNTuple*>(item);
			seq_array->AddItem(node_id, position, *point);

			delete point;
			point = nullptr;
			item = nullptr;
			break;
		case DataShape::DS_LINESTRING:
			ls = static_cast<cLineString<cNTuple>*>(item);
			tuple_arr = ls->GetVerticesCollection();
			tuple_size = ls->GetVerticesCount();

			for (unsigned int i = 0; i < tuple_size; i++) {
				seq_array->AddItem(node_id, position, *(tuple_arr[i]));
			}

			delete ls;
			ls = nullptr;
			item = nullptr;
			break;
		case DataShape::DS_POLYGON:
			poly = static_cast<cPolygon<cNTuple>*>(item);
			tuple_arr = poly->GetVerticesCollection();
			tuple_size = poly->GetVerticesCount();

			for (unsigned int i = 0; i < tuple_size; i++) {
				seq_array->AddItem(node_id, position, *(tuple_arr[i]));
			}

			delete poly;
			poly = nullptr;
			item = nullptr;
			break;
		case DataShape::DS_SPHERE:
			sphere = static_cast<cSphere<cNTuple>*>(item);
			tuple_arr = sphere->GetVerticesCollection();
			tuple_size = sphere->GetVerticesCount();

			for (unsigned int i = 0; i < tuple_size; i++) {
				seq_array->AddItem(node_id, position, *(tuple_arr[i]));
			}

			delete sphere;
			sphere = nullptr;
			item = nullptr;
			break;
		default:
			break;
		}


		// impossible to delete through base class, dtor not virtual!!!
		//delete item;
		//item = nullptr;
	}

	delete p;

	printf("Inserted %i items.", seq_array->GetHeader()->GetItemCount());

	db->Close();

	delete seq_array;
	delete db;
	delete header;
	delete context;

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