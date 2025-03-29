#pragma once
#include "../shpRecordContent.h"
#include "shpPointM.h"

namespace nsShapeFile {
	struct shpPointZ : protected shpPointM {
		double Z = 0.0;

		bool load(FileHandler* fh, Header& header) override {
			is_loaded = false;
			shape_type = fh->get_int();
			if (shape_type != 11) {
				return is_loaded;
			}
			en_shape_type = shpShapeType::POINTZ;
			X = fh->get_double();
			Y = fh->get_double();
			Z = fh->get_double();
			M = fh->get_double();
			is_loaded = true;
			return is_loaded;
		}


		virtual shpShapeType get_shape_type() override {
			return shpShapeType::POINTZ;
		}
	};
}