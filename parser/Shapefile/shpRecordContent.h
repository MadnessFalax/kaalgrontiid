#pragma once
#include "../buffer/pFileHandler.h"
#include "shpFileHeader.h"

namespace nsShapeFile {
	using FileHandler = nspFile::pFileHandler;

	struct shpRecordContent {
		shpShapeType en_shape_type = shpShapeType::NULLSHAPE;
		int shape_type = 0;
		bool is_loaded = false;

		virtual bool load(FileHandler* fh) = 0;
	};
}