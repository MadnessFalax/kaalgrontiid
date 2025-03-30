//#include "parser/JSON/GeoJSONConverter.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "container/pArray.h"
#include "container/pString.h"
#include "container/pMap.h"
#include "container/pPair.h"

#include "parser/buffer/pFileHandler.h"
#include "parser/Shapefile/shpFileHeader.h"
#include "parser/Shapefile/shpRecordIndex.h"

template <class T>
using Array = nspArray::pArray<T>;
using String = nspString::pString;
template <class T, class U, class V = unsigned char>
using Map = nspMap::pMap<T, U, V>;
using FileHandler = nspFile::pFileHandler;
using Header = nsShapeFile::shpFileHeader;
using Index = nsShapeFile::shpRecordIndex;

// used for out of scope stack disposal check so that _CrtDumpMemoryLeaks doesnt show false positive leaks on stack allocated memory
static void helper() {

#ifndef WORK
    String path = "C:\\Users\\Petr\\Downloads\\src\\test\\kaalgrontiid\\test\\gis_osm_water_a_07_1.shx";
#endif

#ifdef WORK
    String path = "C:\\Users\\uiv56391\\source\\repos\\framework-back-up\\test\\kaalgrontiid\\test\\gis_osm_water_a_07_1.shx";
#endif

    FileHandler* fh = new FileHandler(path, true);
	Header* header = new Header();
	header->load(fh);

	Index index = Index();
    while (index.load(fh)) {
        if (index.offset == 0) {
            break;
        }
        else {
            printf("%i\n", index.offset);
            printf("%i\n", index.content_length);
        }
    }

	delete header;
    delete fh;

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