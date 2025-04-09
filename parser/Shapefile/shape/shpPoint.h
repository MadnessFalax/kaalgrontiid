#pragma once
#include "../shpRecordContent.h"

namespace nsShapeFile {
	struct shpPoint : public shpRecordContent {
		double X = 0.0;
		double Y = 0.0;

		bool load(FileHandler* fh, Header& header) override {
			index = 0;
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

		cDataShape<cNTuple>* get_item() override {
			if (index == 0 && is_loaded) {

				cNTuple* tuple = new cNTuple(&sd_3d);
				tuple->SetValue(0, X, nullptr);
				tuple->SetValue(1, Y, nullptr);
				tuple->SetValue(2, 0, nullptr);

				index++;

				auto** tuples = new cNTuple * [1];
				tuples[0] = tuple;
				auto* shape = cDataShape<cNTuple>::CreateDataShape(DataShape::DS_POINT, tuples, 1, &sd_2d);

				tuple = nullptr;
				tuples = nullptr;

				return shape;
			}

			return nullptr;
		}

		~shpPoint() {}
	};
}