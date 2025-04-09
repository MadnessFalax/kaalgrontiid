#pragma once
#include "../shpRecordContent.h"

namespace nsShapeFile {
	struct shpNullShape : public shpRecordContent {

		bool load(FileHandler* fh, Header& header) override {
			is_loaded = false;
			shape_type = fh->get_int();
			if (shape_type != 0) {
				return is_loaded;
			}
			en_shape_type = shpShapeType::NULLSHAPE;
			is_loaded = true;
			return is_loaded;
		}


		shpShapeType get_shape_type() override {
			return shpShapeType::NULLSHAPE;
		}

		cDataShape<cNTuple>* get_item() override {
			return nullptr;
		}

		~shpNullShape() {}
	};
}