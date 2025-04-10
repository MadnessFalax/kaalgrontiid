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
#include "parser/Shapefile/shpParser.h"

#include "exporter/gjExporter.h"
#include "exporter/kmlExporter.h"
#include "exporter/osmExporter.h"
#include "exporter/shpExporter.h"

#include "cLineString.h"
#include "cPolygon.h"
#include "cSphere.h"
#include "cPoint.h"

#include "dstruct/paged/core/cNodeCache.h"
#include "dstruct/paged/core/cDStructConst.h"
#include "dstruct/paged/sequentialarray/cSequentialArray.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayContext.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayHeader.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayNodeHeader.h"
#include "common/utils/cTimer.h"

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

using Tkey = cDataShape<cNTuple>;
using SeqArray = dstruct::paged::sqarray::cSequentialArray<Tkey>;
using SeqArrayContext = dstruct::paged::sqarray::cSequentialArrayContext<Tkey>;
using SeqArrayHeader = dstruct::paged::sqarray::cSequentialArrayHeader<Tkey>;

#ifdef TEST_OSM
constexpr auto INPUT_PATH = R"(.\test\input\short.osm)";
constexpr auto OUTPUT_PATH = R"(.\test\output\output.osm)";
#endif

#ifdef TEST_GEOJS
constexpr auto INPUT_PATH = R"(.\test\input\sample_geo.json)";
constexpr auto OUTPUT_PATH = R"(.\test\output\output.json)";
#endif

#ifdef TEST_KML
constexpr auto INPUT_PATH = R"(.\test\input\KML_Samples.kml)";
constexpr auto OUTPUT_PATH = R"(.\test\output\output.kml)";
#endif

#ifdef TEST_SHP
constexpr auto INPUT_PATH = R"(.\test\input\gis_osm_natural_a_07_1)";	// should not have extension in case of Shapefile, e.g.: R"(.\test\input\gis_osm_natural_a_07_1)"
constexpr auto OUTPUT_PATH = R"(.\test\output\output)";					// should not have extension in case of Shapefile, e.g.: R"(.\test\output\shapefile)"
#endif


static void print_point_info(cNTuple* point) {
	printf("Point: ");
	auto pt_len = point->GetLength();
	for (unsigned int i = 0; i < pt_len; i++) {
		printf("%f ", point->GetDouble(i, nullptr));
	}
	printf("\n");
}

static void print_shape_info(cDataShape<cNTuple>* shape) {
	cNTuple* point = nullptr;
	cLineString<cNTuple>* ls = nullptr;
	cPolygon<cNTuple>* poly = nullptr;
	cSphere<cNTuple>* sphere = nullptr;
	cPoint<cNTuple>* pt = nullptr;

	unsigned int vt_count = 0;
	DataShape shp_type = shape->GetDataShape();
	printf("\n------------------\n");

	switch (shp_type) {
	case DataShape::DS_POINT:
		pt = static_cast<cPoint<cNTuple>*>(shape);
		vt_count = pt->GetVerticesCount();
		printf("Point: %i vertices:\n", vt_count);
		point = pt->GetVertex(0);
		print_point_info(point);
		break;
	case DataShape::DS_LINESTRING:
		ls = static_cast<cLineString<cNTuple>*>(shape);
		vt_count = ls->GetVerticesCount();
		printf("LineString: %i vertices:\n", vt_count);
		for (unsigned int i = 0; i < vt_count; i++) {
			point = ls->GetVertex(i);
			print_point_info(point);
		}
		break;
	case DataShape::DS_POLYGON:
		poly = static_cast<cPolygon<cNTuple>*>(shape);
		vt_count = poly->GetVerticesCount();
		printf("Polygon: %i vertices:\n", vt_count);
		for (unsigned int i = 0; i < vt_count; i++) {
			point = poly->GetVertex(i);
			print_point_info(point);
		}
		break;
	case DataShape::DS_SPHERE:
		sphere = static_cast<cSphere<cNTuple>*>(shape);
		vt_count = sphere->GetVerticesCount();
		printf("Sphere: %i vertices:\n", vt_count);
		for (unsigned int i = 0; i < vt_count; i++) {
			point = sphere->GetVertex(i);
			print_point_info(point);
		}
		break;
	default:
		break;
	}

	printf("------------------\n\n");
}

// used for out of scope stack disposal check so that _CrtDumpMemoryLeaks doesnt show false positive leaks on stack allocated memory
static void helper() {
	cTimer timer = cTimer();

	auto* space_desc_3d = new cSpaceDescriptor(DIMENSION_3, new cNTuple(), new cDouble());
	auto* context = new SeqArrayContext();
	auto* header = new SeqArrayHeader("seqArray", 8192, space_desc_3d, cDStructConst::DSMODE_DEFAULT);
	header->SetCodeType(ELIAS_DELTA);

	cQuickDB* db = new cQuickDB();
	db->Create("test", 1000, 50000, 8192);

	auto* seq_array = new SeqArray();
	seq_array->Create(header, db);

	unsigned int node_id, position;

#ifdef TEST_OSM
	auto* parser = nsOSM::setup_parser();
#endif

#ifdef TEST_GEOJS
	auto* parser = nsGeoJSON::setup_parser();
#endif

#ifdef TEST_KML
	auto* parser = nsKML::setup_parser();
#endif

#ifdef TEST_SHP
	auto* parser = new nsShapeFile::shpParser();
#endif

	cDataShape<cNTuple>* item = nullptr;

	// -------------- IMPORT DATA ------------------
	printf("\n\nTEST #1 - Data import:\n");

	// IMPORT SAMPLE
	
	printf("Importing sample...\n");
	
	timer.Start();
	parser->open(INPUT_PATH);
	while (item = parser->get_item()) {
		seq_array->AddItem(node_id, position, *item);
		delete item;
		item = nullptr;
	}
	delete parser;

	timer.Stop();	
	printf("Importing sample done.\n");

	auto item_count = seq_array->GetHeader()->GetItemCount();
	printf("Inserted %i items.\n", item_count);

	timer.Print(" - Import time\n");
	printf("Took %.3f seconds per imported item.\n", timer.GetRealTime() / item_count);
	
	// -------------- EXPORT DATA ------------------
	printf("\n\nTEST #2 - Data export:\n");

	// EXPORT ALL IMPORTED DATA

#ifdef TEST_OSM
	auto* exporter = new nsOSM::osmExporter(".\\test\\output\\output.osm");
#endif

#ifdef TEST_GEOJS
	auto* exporter = new nsGeoJSON::gjExporter(".\\test\\output\\output.json");
#endif

#ifdef TEST_KML
	auto* exporter = new nsKML::kmlExporter(".\\test\\output\\output.kml");
#endif

#ifndef TEST_SHP
	printf("Exporting...\n");

	timer.Reset();
	timer.Start();

	exporter->begin();

	seq_array->OpenContext(seq_array->GetHeader()->GetFirstNodeIndex(), 0, context);

	for (decltype(item_count) i = 0; i < item_count; i++) {
		if (i != 0) {
			seq_array->Advance(context);
		}
		Tkey* out_item = Tkey::CreateDataShape(context->GetItem(), space_desc_3d);
		switch (out_item->GetDataShape()) {
		case DataShape::DS_POINT:
			exporter->export_item(static_cast<cPoint<cNTuple>*>(out_item));
			break;
		case DataShape::DS_LINESTRING:
			exporter->export_item(static_cast<cLineString<cNTuple>*>(out_item));
			break;
		case DataShape::DS_POLYGON:
			exporter->export_item(static_cast<cPolygon<cNTuple>*>(out_item));
			break;
		case DataShape::DS_SPHERE:
			exporter->export_item(static_cast<cSphere<cNTuple>*>(out_item));
			break;
		default:
			break;
		}
		delete out_item;
	}

	seq_array->CloseContext(context);

	exporter->end();
	delete exporter;

	timer.Stop();
	timer.Print(" - Export time\n");
	printf("Took %.3f seconds per exported item.\n", timer.GetRealTime() / item_count);

	printf("Exporting done.\n");
#endif

#ifdef TEST_SHP
	// EXPORT ALL IMPORTED DATA INTO SHAPEFILE (SHP file only consists of one type, for that reason file for every type is created.)
	printf("Exporting to SHP...\n");

	timer.Reset();
	timer.Start();

	auto* shp_point_exporter = new nsShapeFile::shpExporter<cPoint<cNTuple>>(".\\test\\output\\output_pt");
	auto* shp_linestring_exporter = new nsShapeFile::shpExporter<cLineString<cNTuple>>(".\\test\\output\\output_ls");
	auto* shp_polygon_exporter = new nsShapeFile::shpExporter<cPolygon<cNTuple>>(".\\test\\output\\output_poly");
	shp_point_exporter->begin();
	shp_linestring_exporter->begin();
	shp_polygon_exporter->begin();

	seq_array->OpenContext(seq_array->GetHeader()->GetFirstNodeIndex(), 0, context);

	for (decltype(item_count) i = 0; i < item_count; i++) {
		if (i != 0) {
			seq_array->Advance(context);
		}
		Tkey* out_item = Tkey::CreateDataShape(context->GetItem(), space_desc_3d);
		switch (out_item->GetDataShape()) {
		case DataShape::DS_POINT:
			shp_point_exporter->export_item(static_cast<cPoint<cNTuple>*>(out_item));
			break;
		case DataShape::DS_LINESTRING:
			shp_linestring_exporter->export_item(static_cast<cLineString<cNTuple>*>(out_item));
			break;
		case DataShape::DS_POLYGON:
			shp_polygon_exporter->export_item(static_cast<cPolygon<cNTuple>*>(out_item));
			break;
		case DataShape::DS_SPHERE:
			shp_point_exporter->export_item(static_cast<cSphere<cNTuple>*>(out_item));
			break;
		default:
			break;
		}
		delete out_item;
	}

	seq_array->CloseContext(context);

	shp_point_exporter->end();
	shp_linestring_exporter->end();
	shp_polygon_exporter->end();
	delete shp_point_exporter;
	delete shp_linestring_exporter;
	delete shp_polygon_exporter;

	timer.Stop();
	timer.Print(" - Export time\n");
	printf("Took %.3f seconds per exported item.\n", timer.GetRealTime() / item_count);

	printf("Exporting to SHP done.\n");
#endif

	db->Close();

	delete seq_array;
	delete db;
	delete header;
	delete context;
	delete space_desc_3d;

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