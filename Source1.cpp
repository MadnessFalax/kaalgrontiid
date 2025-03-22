//#include "parser/JSON/GeoJSONConverter.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "container/pArray.h"
#include "container/pString.h"
#include "container/pMap.h"
#include "container/pPair.h"

#include "exporter/gjExporter.h"
#include "exporter/kmlExporter.h"
#include "exporter/osmExporter.h"

template <class T>
using Array = nspArray::pArray<T>;
using String = nspString::pString;
template <class T, class U, class V = unsigned char>
using Map = nspMap::pMap<T, U, V>;
using FileWriter = nspFile::pFileWriter;

// used for out of scope stack disposal check so that _CrtDumpMemoryLeaks doesnt show false positive leaks on stack allocated memory
static void helper() {

	cSpaceDescriptor* space = new cSpaceDescriptor((unsigned int)3, new cNTuple(), new cDouble());
    cNTuple* point = new cNTuple(space);
    point->SetValue(0, (double)0.0f, nullptr);
    point->SetValue(1, (double)1.0f, nullptr);
    point->SetValue(2, (double)2.0f, nullptr);

    cNTuple* point2 = new cNTuple(space);
    point2->SetValue(0, (double)3.0f, nullptr);
    point2->SetValue(1, (double)4.0f, nullptr);
    point2->SetValue(2, (double)5.0f, nullptr);

    cNTuple* point3 = new cNTuple(space);
    point3->SetValue(0, (double)7.0f, nullptr);
    point3->SetValue(1, (double)6.0f, nullptr);
    point3->SetValue(2, (double)8.0f, nullptr);

    cNTuple* point4 = new cNTuple(space);
    point4->SetValue(0, (double)9.0f, nullptr);
    point4->SetValue(1, (double)10.0f, nullptr);
    point4->SetValue(2, (double)11.0f, nullptr);

    cNTuple* point5 = new cNTuple(space);
    point5->SetValue(0, (double)12.0f, nullptr);
    point5->SetValue(1, (double)13.0f, nullptr);
    point5->SetValue(2, (double)14.0f, nullptr);

    cNTuple* point5_2 = new cNTuple(space);
    point5_2->SetValue(0, (double)12.0f, nullptr);
    point5_2->SetValue(1, (double)13.0f, nullptr);
    point5_2->SetValue(2, (double)14.0f, nullptr);

    cNTuple* point6 = new cNTuple(space);
    point6->SetValue(0, (double)15.0f, nullptr);
    point6->SetValue(1, (double)16.0f, nullptr);
    point6->SetValue(2, (double)17.0f, nullptr);

    cNTuple* point7 = new cNTuple(space);
    point7->SetValue(0, (double)19.0f, nullptr);
    point7->SetValue(1, (double)18.0f, nullptr);
    point7->SetValue(2, (double)20.0f, nullptr);

    cNTuple* point8 = new cNTuple(space);
    point8->SetValue(0, (double)22.0f, nullptr);
    point8->SetValue(1, (double)21.0f, nullptr);
    point8->SetValue(2, (double)23.0f, nullptr);

    cNTuple* point9 = new cNTuple(space);
    point9->SetValue(0, (double)24.0f, nullptr);
    point9->SetValue(1, (double)25.0f, nullptr);
    point9->SetValue(2, (double)26.0f, nullptr);

    cNTuple* point10 = new cNTuple(space);
    point10->SetValue(0, (double)27.0f, nullptr);
    point10->SetValue(1, (double)28.0f, nullptr);
    point10->SetValue(2, (double)29.0f, nullptr);

	cNTuple** ls_points = new cNTuple * [4];
	ls_points[0] = point2;
	ls_points[1] = point3;
	ls_points[2] = point4;
    ls_points[3] = point7;

	cLineString<cNTuple>* line = new cLineString<cNTuple>(ls_points, 4, space);

	cNTuple** poly_points = new cNTuple * [4];
	poly_points[0] = point5;
	poly_points[1] = point6;
	poly_points[2] = point8;
	poly_points[3] = point5_2;

	cPolygon<cNTuple>* poly = new cPolygon<cNTuple>(poly_points, 4, space);

	cNTuple** sphere_points = new cNTuple * [2];
	sphere_points[0] = point9;
	sphere_points[1] = point10;

	cSphere<cNTuple>* sphere = new cSphere<cNTuple>(sphere_points, 2, space);
#ifdef _GEOJS
	auto* exporter = new nsGeoJSON::gjExporter(".\\test\\output.json");
#endif
#ifdef _KML
    auto* exporter = new nsKML::kmlExporter(".\\test\\output.kml");
#endif
#ifdef _OSM
    auto* exporter = new nsOSM::osmExporter(".\\test\\output.osm");
#endif
    exporter->begin();
	exporter->export_item(point);
	exporter->export_item(line);
	exporter->export_item(poly);
	exporter->export_item(sphere);
	exporter->end();
	delete exporter;

    delete point;
    delete line;
    delete poly;
	delete sphere;

    delete space;

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