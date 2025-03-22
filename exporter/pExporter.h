#pragma once
#include "buffer/pFileWriter.h"


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