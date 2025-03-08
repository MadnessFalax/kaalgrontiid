#include "common/cNumber.h"
#include "common/utils/cTimer.h"
#include "common/stream/cStream.h"
#include "common/datatype/cBasicType.h"	
#include "common/datatype/tuple/cCommonNTuple.h"			
#include "common/datatype/tuple/cHNTuple.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/cString.h"
#include "common/datatype/geo/cLineString.h"
#include "common/datatype/geo/cPolygon.h"
#include "common/datatype/geo/cSphere.h"

#include "dstruct/paged/core/cNodeCache.h"
#include "dstruct/paged/core/cDStructConst.h"
#include "dstruct/paged/sequentialarray/cSequentialArray.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayContext.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayHeader.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayNodeHeader.h"

const unsigned int TUPLE_SIZE = 2;
const unsigned int ITEM_COUNT = 2000000;
const unsigned int CACHE_SIZE = 500;
const unsigned int BLOCK_SIZE = 8192;
const unsigned int MAX_NODE_INMEM_SIZE = 10000;

using namespace dstruct::paged::core;
using namespace dstruct::paged::sqarray;
using namespace common::datatype::tuple;
using namespace common::utils;

typedef cCommonNTuple<double> tKey;
typedef cSequentialArray<tKey> tSA;

tKey* mItem[ITEM_COUNT];
tSA* mSeqArray;
cSequentialArrayHeader<tKey>* mHeader;

const char* GetObjectName(char c)
{
	const unsigned int CHARS_COUNT = 20;
	char* dataShapeName = new char[CHARS_COUNT];
	switch (c)
	{
	case 'l': strcpy(dataShapeName, "linestring\0"); break;
	case 'p': strcpy(dataShapeName, "polygon"); break;
	case 's': strcpy(dataShapeName, "sphere"); break;
	default: strcpy(dataShapeName, ""); break;
	}
	return dataShapeName;
}

void PrintShapeInfo(cDataShape<cCommonNTuple<double>>* shape, cSpaceDescriptor* sd)
{
	printf("\n Vertices overview of %s object :\n\n", GetObjectName(shape->GetCode()));
	shape->PrintAllTuples(sd);
	if (shape->GetCode() == 's')
	{
		printf("\n Center of sphere: ->");
		shape->PrintTuple(shape->GetVerticesCollection()[0], sd);
		double radius = shape->GetRadius(shape->GetVerticesCollection(), shape->GetVerticesCount(), sd);
		printf(" Radius of sphere: -> %f\n", radius);
	}

	printf("\n MBR approximation of %s object :\n\n", GetObjectName(shape->GetCode()));
	cMBRectangle<cCommonNTuple<double>>* mbr = shape->ConvertToMBR(sd);
	shape->PrintTuple(mbr->GetLoTuple(), sd);
	shape->PrintTuple(mbr->GetHiTuple(), sd);

	printf("\n\n *********************************************\n\n");
}

// create dummy data
void prepare()
{
	double dummy_data_raw[6] = {0, 1, 2, 0, 5, 10};

	// define what is tuple made of
	cSpaceDescriptor* sd = new cSpaceDescriptor(DIMENSION_2, new cCommonNTuple<double>(), new cDouble());

	// create collection of uninitialized vertex tuples
	unsigned int vertex_count = 3;
	cCommonNTuple<double>** vertex_data = NULL;
	vertex_data = new cCommonNTuple<double> * [vertex_count];

	// fill vertex collection with vertex data
	for (int i = 0; i < 3; i++) {
		// create empty point of sd shape data;
		vertex_data[i] = new cCommonNTuple<double>(sd, 0);
		for (int j = 0; j < 2 /*dim size is 2 in 2D space*/; j++) {
			vertex_data[i]->SetValue(j, (double)dummy_data_raw[j + i * 2], sd);
			printf("%f at %i row and %i col... final pos: %i, ", dummy_data_raw[j + i * 2], i, j, j + i * 2);
		}
		printf("\n");
	}

	cDataShape<cCommonNTuple<double>>* linestring = cDataShape<cCommonNTuple<double>>::CreateDataShape(DataShape::DS_LINESTRING, vertex_data, 3, sd);

	PrintShapeInfo(linestring, sd);
	printf("mission succeeded unexpectedly\n");
/*
	common::datatype::tuple::cSpaceDescriptor* sd = new common::datatype::tuple::cSpaceDescriptor(TUPLE_SIZE, new tKey(), new);
	sd->SetDimensionType(0, new cUInt());
	sd->SetDimensionType(1, new cFloat());
	 

	// preparation of the items
	for (unsigned int i = 0; i < ITEM_COUNT; i++)
	{
		mItem[i] = new tKey();
		for (unsigned int j = 0; j < TUPLE_SIZE; j++)
		{
			mItem[i]->SetValue(j, cNumber::Random(TUPLE_SIZE), sd);

		}
	}

	*/
}

bool create()
{
	bool debug = false;
	cTimer runtime;
	unsigned int nodeid, position;
	cSequentialArrayContext<tKey>* context = new cSequentialArrayContext<tKey>();

	common::datatype::tuple::cSpaceDescriptor* sd = new common::datatype::tuple::cSpaceDescriptor(TUPLE_SIZE, new tKey());
	cTuple tp(sd);

	// headers
	mHeader = new cSequentialArrayHeader<tKey>("seqarray1", BLOCK_SIZE, sd, cDStructConst::DSMODE_DEFAULT);
	mHeader->SetCodeType(ELIAS_DELTA);

	// database
	cQuickDB* quickDB = new cQuickDB();
	if (!quickDB->Create("quickDB", CACHE_SIZE, MAX_NODE_INMEM_SIZE, BLOCK_SIZE))
	{
		printf("Critical Error: Cache Data File was not created!\n");
		exit(1);
	}

	// sequential array
	mSeqArray = new tSA();
	if (!mSeqArray->Create(mHeader, quickDB))
	{
		printf("Sequential array: creation failed\n");
		return false;
	}

	runtime.Start();
	for (unsigned int i = 0; i < ITEM_COUNT; i++)
	{
		if (i % 100000 == 0)
		{
			printf("%d\r", i);
		}

		mSeqArray->AddItem(nodeid, position, *mItem[i]);
	}
	runtime.Stop();
	runtime.Print(" - Insert time\n");
	printf("Performance: %.1f Inserts/s\n", mSeqArray->GetHeader()->GetItemCount() / runtime.GetRealTime());

	unsigned int i = 0;
	mSeqArray->OpenContext(mHeader->GetFirstNodeIndex(), 0, context);
	do
	{
		if (mItem[i++]->Equal(context->GetItem(), sd) != 0)
		{
			printf("Chyba u %d prvku: ", i - 1);
			//mItem[i - 1]->Print("\n", sd);
			//tKey::Print(context->GetItem(), "\n", sd);
			return false;
		}
	} while (mSeqArray->Advance(context));
	mSeqArray->CloseContext(context);

	quickDB->Close();

	delete mSeqArray;
	delete quickDB;
	delete mHeader;

	return true;
}


bool open()
{
	bool debug = false;
	cTimer runtime;
	unsigned int nodeid, position;
	cSequentialArrayContext<tKey>* context = new cSequentialArrayContext<tKey>();

	common::datatype::tuple::cSpaceDescriptor* sd = new common::datatype::tuple::cSpaceDescriptor(TUPLE_SIZE, new tKey(), new cUInt());
	cTuple tp(sd);

	// headers
	mHeader = new cSequentialArrayHeader<tKey>("seqarray1", BLOCK_SIZE, sd, cDStructConst::DSMODE_DEFAULT);

	// database
	cQuickDB* quickDB = new cQuickDB();
	if (!quickDB->Open("quickDB", CACHE_SIZE, MAX_NODE_INMEM_SIZE, BLOCK_SIZE))
	{
		printf("Critical Error: Cache Data File was not created!\n");
		exit(1);
	}

	// sequential array
	mSeqArray = new tSA();
	if (!mSeqArray->Open(mHeader, quickDB))
	{
		printf("Sequential array: open failed!\n");
		return false;
	}

	// update test
	printf("These two tuples must be equal: ");
	mSeqArray->UpdateItem(mHeader->GetFirstNodeIndex(), mItem[0]->GetSize(sd), *mItem[0]);
	mSeqArray->OpenContext(mHeader->GetFirstNodeIndex(), 0, context);
	//tKey::Print(context->GetItem(), ", ", sd);
	mSeqArray->Advance(context);
	//tKey::Print(context->GetItem(), "\n", sd);
	mSeqArray->CloseContext(context);
	mSeqArray->UpdateItem(mHeader->GetFirstNodeIndex(), mItem[0]->GetSize(sd), *mItem[1]);

	// read of the whole array
	unsigned int i = 0;
	runtime.Start();
	mSeqArray->OpenContext(mHeader->GetFirstNodeIndex(), 0, context);
	do
	{
		if (mItem[i++]->Equal(context->GetItem(), sd) != 0)
		{
			printf("Chyba u %d prvku: ", i - 1);
			//mItem[i - 1]->Print("\n", sd);
			//tKey::Print(context->GetItem(), "\n", sd);
			return false;
		}
	} while (mSeqArray->Advance(context));
	mSeqArray->CloseContext(context);
	runtime.Stop();

	runtime.Print(" - Read time (after open)\n");

	quickDB->Close();

	delete mSeqArray;
	delete quickDB;
	delete mHeader;

	return true;
}

int main() {
	prepare();

	bool ok = create();
	ok &= open();

	printf("\n-------------------------\n");
	if (ok)
	{
		printf("cSequentialArray: Test probehl uspesne\n");
	}
	else
	{
		printf("cSequentialArray: Error during the test!\n");
	}
	
	return 0;
}

