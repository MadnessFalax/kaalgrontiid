#define _CRTDBG_MAP_ALLOC
#define DATASHAPE_POINT
#include <stdlib.h>
#include <crtdbg.h>

#include "container/pArray.h"
#include "container/pString.h"
#include "container/pMap.h"
#include "container/pPair.h"

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

using Tkey = cDataShape<cNTuple>;
using SeqArray = dstruct::paged::sqarray::cSequentialArray<Tkey>;
using SeqArrayContext = dstruct::paged::sqarray::cSequentialArrayContext<Tkey>;
using SeqArrayHeader = dstruct::paged::sqarray::cSequentialArrayHeader<Tkey>;

static void print_point_info(cNTuple* point) {
    printf("Point: ");
    auto pt_len = point->GetLength();
    for (unsigned int i = 0; i < pt_len; i++) {
        printf("%f ", point->GetDouble(i, nullptr));
    }
    printf("\n");
}

static void print_shape_info(cDataType* shape, DataShape shp_type) {
    cNTuple* point = nullptr;
    cLineString<cNTuple>* ls = nullptr;
    cPolygon<cNTuple>* poly = nullptr;
    cSphere<cNTuple>* sphere = nullptr;
    cPoint<cNTuple>* pt = nullptr;

    unsigned int vt_count = 0;
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

    // DEFINE SHAPE DATA
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

#ifdef POLYGON
    cNTuple* point5_2 = new cNTuple(space);
    point5_2->SetValue(0, (double)12.0f, nullptr);
    point5_2->SetValue(1, (double)13.0f, nullptr);
    point5_2->SetValue(2, (double)14.0f, nullptr);
#endif

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

#ifdef POLYGON
    cNTuple* point10_2 = new cNTuple(space);
    point10_2->SetValue(0, (double)27.0f, nullptr);
    point10_2->SetValue(1, (double)28.0f, nullptr);
    point10_2->SetValue(2, (double)29.0f, nullptr);
#endif

#ifdef DATASHAPE_POINT
	cNTuple** pt1_arr = new cNTuple * [1];
	pt1_arr[0] = point;
    cPoint<cNTuple>* pt1 = new cPoint<cNTuple>(pt1_arr, 1, space);

    cNTuple** pt2_arr = new cNTuple * [1];
    pt2_arr[0] = point2;
	cPoint<cNTuple>* pt2 = new cPoint<cNTuple>(pt2_arr, 1, space);

    cNTuple** pt3_arr = new cNTuple * [1];
    pt3_arr[0] = point3;
	cPoint<cNTuple>* pt3 = new cPoint<cNTuple>(pt3_arr, 1, space);

    cNTuple** pt4_arr = new cNTuple * [1];
    pt4_arr[0] = point4;
	cPoint<cNTuple>* pt4 = new cPoint<cNTuple>(pt4_arr, 1, space);

    cNTuple** pt5_arr = new cNTuple * [1];
    pt5_arr[0] = point5;
	cPoint<cNTuple>* pt5 = new cPoint<cNTuple>(pt5_arr, 1, space);

    cNTuple** pt6_arr = new cNTuple * [1];
    pt6_arr[0] = point6;
	cPoint<cNTuple>* pt6 = new cPoint<cNTuple>(pt6_arr, 1, space);

    cNTuple** pt7_arr = new cNTuple * [1];
    pt7_arr[0] = point7;
	cPoint<cNTuple>* pt7 = new cPoint<cNTuple>(pt7_arr, 1, space);

    cNTuple** pt8_arr = new cNTuple * [1];
    pt8_arr[0] = point8;
	cPoint<cNTuple>* pt8 = new cPoint<cNTuple>(pt8_arr, 1, space);

    cNTuple** pt9_arr = new cNTuple * [1];
    pt9_arr[0] = point9;
	cPoint<cNTuple>* pt9 = new cPoint<cNTuple>(pt9_arr, 1, space);

    cNTuple** pt10_arr = new cNTuple * [1];
    pt10_arr[0] = point10;
	cPoint<cNTuple>* pt10 = new cPoint<cNTuple>(pt10_arr, 1, space);
#endif

#ifdef LINESTRING
    cNTuple** ls_points = new cNTuple * [4];
    ls_points[0] = point;
    ls_points[1] = point2;
    ls_points[2] = point3;
    ls_points[3] = point4;
    cLineString<cNTuple>* line1 = new cLineString<cNTuple>(ls_points, 4, space);

	ls_points = new cNTuple * [3];
	ls_points[0] = point5;
	ls_points[1] = point6;
	ls_points[2] = point7;
    cLineString<cNTuple>* line2 = new cLineString<cNTuple>(ls_points, 3, space);

	ls_points = new cNTuple * [3];
	ls_points[0] = point8;
	ls_points[1] = point9;
	ls_points[2] = point10;
    cLineString<cNTuple>* line3 = new cLineString<cNTuple>(ls_points, 3, space);
#endif

#ifdef POLYGON
    cNTuple** poly_points = new cNTuple * [6];
    poly_points[0] = point5;
    poly_points[1] = point;
    poly_points[2] = point2;
    poly_points[3] = point3;
    poly_points[4] = point4;
    poly_points[5] = point5_2;
    cPolygon<cNTuple>* poly1 = new cPolygon<cNTuple>(poly_points, 6, space);

    poly_points = new cNTuple * [6];
    poly_points[0] = point10;
    poly_points[1] = point6;
    poly_points[2] = point7;
    poly_points[3] = point8;
    poly_points[4] = point9;
    poly_points[5] = point10_2;
    cPolygon<cNTuple>* poly2 = new cPolygon<cNTuple>(poly_points, 6, space);
#endif

#ifdef SPHERE
    cNTuple** sphere_points = new cNTuple * [2];
    sphere_points[0] = point;
    sphere_points[1] = point2;
    cSphere<cNTuple>* sphere1 = new cSphere<cNTuple>(sphere_points, 2, space);

	sphere_points = new cNTuple * [2];
    sphere_points[0] = point3;
	sphere_points[1] = point4;
    cSphere<cNTuple>* sphere2 = new cSphere<cNTuple>(sphere_points, 2, space);

	sphere_points = new cNTuple * [2];
	sphere_points[0] = point5;
	sphere_points[1] = point6;
    cSphere<cNTuple>* sphere3 = new cSphere<cNTuple>(sphere_points, 2, space);

	sphere_points = new cNTuple * [2];
	sphere_points[0] = point7;
	sphere_points[1] = point8;
    cSphere<cNTuple>* sphere4 = new cSphere<cNTuple>(sphere_points, 2, space);
	
    sphere_points = new cNTuple * [2];
	sphere_points[0] = point9;
	sphere_points[1] = point10;
    cSphere<cNTuple>* sphere5 = new cSphere<cNTuple>(sphere_points, 2, space);
#endif

    auto* context = new SeqArrayContext();
    auto* header = new SeqArrayHeader("seqArray", 8192, space, cDStructConst::DSMODE_DEFAULT);
    header->SetCodeType(ELIAS_DELTA);

    cQuickDB* db = new cQuickDB();
    db->Create("test", 500, 10000, 8192);

    auto* seq_array = new SeqArray();
    seq_array->Create(header, db);

    unsigned int node_id, position;

#ifdef LINESTRING

    //auto s = line1->GetSize(space);
	seq_array->AddItem(node_id, position, *line1);
	seq_array->AddItem(node_id, position, *line2);
	seq_array->AddItem(node_id, position, *line3);
#endif

#ifdef POLYGON
	seq_array->AddItem(node_id, position, *poly1);
	seq_array->AddItem(node_id, position, *poly2);
#endif

#ifdef SPHERE
	seq_array->AddItem(node_id, position, *sphere1);
	seq_array->AddItem(node_id, position, *sphere2);
	seq_array->AddItem(node_id, position, *sphere3);
	seq_array->AddItem(node_id, position, *sphere4);
	seq_array->AddItem(node_id, position, *sphere5);
#endif

#ifdef DATASHAPE_POINT
	seq_array->AddItem(node_id, position, *pt1);
	seq_array->AddItem(node_id, position, *pt2);
	seq_array->AddItem(node_id, position, *pt3);
	seq_array->AddItem(node_id, position, *pt4);
	seq_array->AddItem(node_id, position, *pt5);
	seq_array->AddItem(node_id, position, *pt6);
	seq_array->AddItem(node_id, position, *pt7);
	seq_array->AddItem(node_id, position, *pt8);
	seq_array->AddItem(node_id, position, *pt9);
	seq_array->AddItem(node_id, position, *pt10);
#endif

	auto item_count = seq_array->GetHeader()->GetItemCount();
    seq_array->OpenContext(seq_array->GetHeader()->GetFirstNodeIndex(), 0, context);

	for (decltype(item_count) i = 0; i < item_count; i++) {
		if (i != 0) {
			seq_array->Advance(context);
		}
		Tkey* item = Tkey::CreateDataShape(context->GetItem(), space);
		print_shape_info(item, item->GetDataShape());
        delete item;
	}

	seq_array->CloseContext(context);

#ifdef LINESTRING
    delete line1;
    delete line2;
    delete line3;
#endif

#ifdef POLYGON
    delete poly1;
    delete poly2;
#endif

#ifdef SPHERE
    delete sphere1;
    delete sphere2;
    delete sphere3;
    delete sphere4;
    delete sphere5;
#endif

#ifdef DATASHAPE_POINT
	delete pt1;
	delete pt2;
	delete pt3;
	delete pt4;
	delete pt5;
	delete pt6;
	delete pt7;
	delete pt8;
	delete pt9;
	delete pt10;
#endif

    db->Close();

	delete seq_array;
    delete db;
    delete header;
	delete context;
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