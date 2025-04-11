#pragma once
#include "buffer/pFileWriter.h"

#include "cPolygon.h"
#include "cLineString.h"
#include "cSphere.h"
#include "cPoint.h"


namespace nspExporter {

	class pExporter {


	public:
		virtual ~pExporter() {};
		
		// dump initial data
		virtual bool begin() = 0;

		// dump final data
		virtual bool end() = 0;

	};
}