#pragma once
#include "../shpRecordContent.h"

namespace nsShapeFile {
	struct shpPoint : protected shpRecordContent {
		double X = 0.0;
		double Y = 0.0;

		bool load(FileHandler* fh) override {
			is_loaded = false;
			shape_type = fh->get_int();
			if (shape_type != 1) {
				return is_loaded;
			}
			en_shape_type = shpShapeType::POINT;
			X = fh->get_double();
			Y = fh->get_double();
			is_loaded = true;
			return is_loaded;
		}
	};
}