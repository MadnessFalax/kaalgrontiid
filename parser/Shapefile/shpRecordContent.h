#pragma once
#include "../buffer/pFileHandler.h"
#include "shpRecordHeader.h"
#include "shpFileHeader.h"

#include "cSphere.h"
#include "cLineString.h"
#include "cPolygon.h"

namespace nsShapeFile {
	using FileHandler = nspFile::pFileHandler;
	using Header = nsShapeFile::shpRecordHeader;

	struct shpRecordContent {
		int index = 0;

		cSpaceDescriptor sd_2d = cSpaceDescriptor(2, new cNTuple(), new cDouble());
		cSpaceDescriptor sd_3d = cSpaceDescriptor(3, new cNTuple(), new cDouble());

		struct Box {
			double Xmin = 0.0;
			double Ymin = 0.0;
			double Xmax = 0.0;
			double Ymax = 0.0;
		};

		struct MRange {
			double min = 0.0;
			double max = 0.0;
		};

		struct ZRange {
			double min = 0.0;
			double max = 0.0;
		};

		shpShapeType en_shape_type = shpShapeType::NULLSHAPE;
		int shape_type = 0;
		bool is_loaded = false;

		virtual bool load(FileHandler* fh, Header& header) = 0;
		virtual shpShapeType get_shape_type() {
			return shpShapeType::NULLSHAPE;
		}
		virtual cDataShape<cNTuple>* get_item() = 0;
		virtual ~shpRecordContent() {};
	};
}