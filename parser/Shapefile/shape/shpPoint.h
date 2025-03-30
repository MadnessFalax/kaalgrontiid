#pragma once
#include "../shpRecordContent.h"

namespace nsShapeFile {
	struct shpPoint : public shpRecordContent {
		double X = 0.0;
		double Y = 0.0;

		bool load(FileHandler* fh, Header& header) override {
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


		shpShapeType get_shape_type() override {
			return shpShapeType::POINT;
		}

		cDataType* get_item() override {
			if (index == 0) {

				cNTuple* tuple = new cNTuple(&sd_2d);
				tuple->SetValue(0, X, nullptr);
				tuple->SetValue(1, Y, nullptr);

				index++;

				return tuple;
			}

			return nullptr;
		}

		~shpPoint() {}
	};
}