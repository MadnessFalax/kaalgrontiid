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


constexpr auto OSM_PATH = R"(.\test\input\short.osm)";
constexpr auto GEOJS_PATH = R"(.\test\input\sample_geo.json)";
constexpr auto KML_PATH = R"(.\test\input\KML_Samples.kml)";
constexpr auto SHP = R"(.\test\input\gis_osm_natural_a_07_1)";

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
	printf("------------------\n");

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
		printf("\n");
		break;
	case DataShape::DS_POLYGON:
		poly = static_cast<cPolygon<cNTuple>*>(shape);
		vt_count = poly->GetVerticesCount();
		printf("Polygon: %i vertices:\n", vt_count);
		for (unsigned int i = 0; i < vt_count; i++) {
			point = poly->GetVertex(i);
			print_point_info(point);
		}
		printf("\n");
		break;
	case DataShape::DS_SPHERE:
		sphere = static_cast<cSphere<cNTuple>*>(shape);
		vt_count = sphere->GetVerticesCount();
		printf("Sphere: %i vertices:\n", vt_count);
		for (unsigned int i = 0; i < vt_count; i++) {
			point = sphere->GetVertex(i);
			print_point_info(point);
		}
		printf("\n");
		break;
	default:
		break;
	}

	printf("------------------\n");
}

// used for out of scope stack disposal check so that _CrtDumpMemoryLeaks doesnt show false positive leaks on stack allocated memory
static void helper() {

	auto* space_desc_3d = new cSpaceDescriptor(DIMENSION_3, new cNTuple(), new cDouble());
	auto* context = new SeqArrayContext();
	auto* header = new SeqArrayHeader("seqArray", 8192, space_desc_3d, cDStructConst::DSMODE_DEFAULT);
	header->SetCodeType(ELIAS_DELTA);

	cQuickDB* db = new cQuickDB();
	db->Create("test", 1000, 50000, 8192);

	auto* seq_array = new SeqArray();
	seq_array->Create(header, db);

	unsigned int node_id, position;

	auto* osm_parser = nsOSM::setup_parser();
	auto* geojs_parser = nsGeoJSON::setup_parser();
	auto* kml_parser = nsKML::setup_parser();
	auto* shp_parser = new nsShapeFile::shpParser();

	cDataShape<cNTuple>* item = nullptr;

	// -------------- IMPORT DATA ------------------
	printf("TEST #1 - Data import:\n");

	// IMPORT OSM SAMPLE
	printf("Importing OSM sample...\n");
	osm_parser->open(OSM_PATH);
	while (item = osm_parser->get_item()) {
		seq_array->AddItem(node_id, position, *item);
		delete item;
		item = nullptr;
	}
	delete osm_parser;
	printf("Importing OSM sample done.\n");

	// IMPORT GEOJSON SAMPLE
	printf("Importing GeoJSON sample...\n");
	geojs_parser->open(GEOJS_PATH);
	while (item = geojs_parser->get_item()) {
		seq_array->AddItem(node_id, position, *item);
		delete item;
		item = nullptr;
	}
	delete geojs_parser;
	printf("Importing GeoJSON sample done.\n");

	// IMPORT KML SAMPLE
	printf("Importing KML sample...\n");
	kml_parser->open(KML_PATH);
	while (item = kml_parser->get_item()) {
		seq_array->AddItem(node_id, position, *item);
		delete item;
		item = nullptr;
	}
	delete kml_parser;
	printf("Importing KML sample done.\n");

	// IMPORT SHAPEFILE SAMPLE
	printf("Importing SHP sample...\n");
	shp_parser->open(SHP);
	while (item = shp_parser->get_item()) {
		seq_array->AddItem(node_id, position, *item);
		delete item;
		item = nullptr;
	}
	delete shp_parser;
	printf("Importing SHP sample done.\n");

	auto item_count = seq_array->GetHeader()->GetItemCount();
	printf("Inserted %i items.\n", item_count);

	// -------------- EXPORT DATA ------------------
	printf("TEST #2 - Data export:\n");

	// EXPORT ALL IMPORTED DATA INTO KML
	printf("Exporting to KML...\n");
	auto* kml_exporter = new nsKML::kmlExporter(".\\test\\output\\output.kml");
	kml_exporter->begin();

	seq_array->OpenContext(seq_array->GetHeader()->GetFirstNodeIndex(), 0, context);

	for (decltype(item_count) i = 0; i < item_count; i++) {
		if (i != 0) {
			seq_array->Advance(context);
		}
		Tkey* out_item = Tkey::CreateDataShape(context->GetItem(), space_desc_3d);
		switch (out_item->GetDataShape()) {
		case DataShape::DS_POINT:
			kml_exporter->export_item(static_cast<cPoint<cNTuple>*>(out_item));
			break;
		case DataShape::DS_LINESTRING:
			kml_exporter->export_item(static_cast<cLineString<cNTuple>*>(out_item));
			break;
		case DataShape::DS_POLYGON:
			kml_exporter->export_item(static_cast<cPolygon<cNTuple>*>(out_item));
			break;
		case DataShape::DS_SPHERE:
			kml_exporter->export_item(static_cast<cSphere<cNTuple>*>(out_item));
			break;
		default:
			break;
		}
		delete out_item;
	}

	seq_array->CloseContext(context);

	kml_exporter->end();
	delete kml_exporter;
	printf("Exporting to KML done.\n");

	// EXPORT ALL IMPORTED DATA INTO OSM
	printf("Exporting to OSM...\n");
	auto* osm_exporter = new nsOSM::osmExporter(".\\test\\output\\output.osm");
	osm_exporter->begin();

	seq_array->OpenContext(seq_array->GetHeader()->GetFirstNodeIndex(), 0, context);

	for (decltype(item_count) i = 0; i < item_count; i++) {
		if (i != 0) {
			seq_array->Advance(context);
		}
		Tkey* out_item = Tkey::CreateDataShape(context->GetItem(), space_desc_3d);
		switch (out_item->GetDataShape()) {
		case DataShape::DS_POINT:
			osm_exporter->export_item(static_cast<cPoint<cNTuple>*>(out_item));
			break;
		case DataShape::DS_LINESTRING:
			osm_exporter->export_item(static_cast<cLineString<cNTuple>*>(out_item));
			break;
		case DataShape::DS_POLYGON:
			osm_exporter->export_item(static_cast<cPolygon<cNTuple>*>(out_item));
			break;
		case DataShape::DS_SPHERE:
			osm_exporter->export_item(static_cast<cSphere<cNTuple>*>(out_item));
			break;
		default:
			break;
		}
		delete out_item;
	}

	seq_array->CloseContext(context);

	osm_exporter->end();
	delete osm_exporter;
	printf("Exporting to OSM done.\n");

	// EXPORT ALL IMPORTED DATA INTO GEOJSON
	printf("Exporting to GeoJSON...\n");
	auto* gj_exporter = new nsGeoJSON::gjExporter(".\\test\\output\\output.json");
	gj_exporter->begin();

	seq_array->OpenContext(seq_array->GetHeader()->GetFirstNodeIndex(), 0, context);

	for (decltype(item_count) i = 0; i < item_count; i++) {
		if (i != 0) {
			seq_array->Advance(context);
		}
		Tkey* out_item = Tkey::CreateDataShape(context->GetItem(), space_desc_3d);
		switch (out_item->GetDataShape()) {
		case DataShape::DS_POINT:
			gj_exporter->export_item(static_cast<cPoint<cNTuple>*>(out_item));
			break;
		case DataShape::DS_LINESTRING:
			gj_exporter->export_item(static_cast<cLineString<cNTuple>*>(out_item));
			break;
		case DataShape::DS_POLYGON:
			gj_exporter->export_item(static_cast<cPolygon<cNTuple>*>(out_item));
			break;
		case DataShape::DS_SPHERE:
			gj_exporter->export_item(static_cast<cSphere<cNTuple>*>(out_item));
			break;
		default:
			break;
		}
		delete out_item;
	}

	seq_array->CloseContext(context);

	gj_exporter->end();
	delete gj_exporter;
	printf("Exporting to GeoJSON done.\n");

	// EXPORT ALL IMPORTED DATA INTO SHAPEFILE (SHP file only consists of one type, for that reason file for every type is created.)
	printf("Exporting to SHP...\n");
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
	printf("Exporting to SHP done.\n");

	db->Close();

	delete seq_array;
	delete db;
	delete header;
	delete context;
	delete space_desc_3d;

	// -------------- EXPORTED DATA IMPORT ------------------
	printf("TEST #3 - Import of exported data:\n");

	space_desc_3d = new cSpaceDescriptor(DIMENSION_3, new cNTuple(), new cDouble());
	context = new SeqArrayContext();
	header = new SeqArrayHeader("seqArray", 8192, space_desc_3d, cDStructConst::DSMODE_DEFAULT);
	header->SetCodeType(ELIAS_DELTA);

	db = new cQuickDB();
	db->Create("test", 1000, 50000, 8192);

	seq_array = new SeqArray();
	seq_array->Create(header, db);

	node_id = position = 0;

	osm_parser = nsOSM::setup_parser();
	geojs_parser = nsGeoJSON::setup_parser();
	kml_parser = nsKML::setup_parser();

	item = nullptr;

	// IMPORT OSM SAMPLE
	printf("Importing OSM sample...\n");
	osm_parser->open(".\\test\\output\\output.osm");
	while (item = osm_parser->get_item()) {
		seq_array->AddItem(node_id, position, *item);
		delete item;
		item = nullptr;
	}
	delete osm_parser;
	printf("Importing OSM sample done.\n");

	// IMPORT GEOJSON SAMPLE
	printf("Importing GeoJSON sample...\n");
	geojs_parser->open(".\\test\\output\\output.json");
	while (item = geojs_parser->get_item()) {
		seq_array->AddItem(node_id, position, *item);
		delete item;
		item = nullptr;
	}
	delete geojs_parser;
	printf("Importing GeoJSON sample done.\n");

	// IMPORT KML SAMPLE
	printf("Importing KML sample...\n");
	kml_parser->open(".\\test\\output\\output.kml");
	while (item = kml_parser->get_item()) {
		seq_array->AddItem(node_id, position, *item);
		delete item;
		item = nullptr;
	}
	delete kml_parser;
	printf("Importing KML sample done.\n");

	item_count = seq_array->GetHeader()->GetItemCount();
	printf("Inserted %i items. Which should be three times as much as in TEST #1.\n", item_count);

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