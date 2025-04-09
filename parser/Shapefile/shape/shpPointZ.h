#pragma once
#include "../shpRecordContent.h"
#include "shpPointM.h"

namespace nsShapeFile {
	struct shpPointZ : public shpPointM {
		double Z = 0.0;

		bool load(FileHandler* fh, Header& header) override {
			index = 0;
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


		shpShapeType get_shape_type() override {
			return shpShapeType::POINTZ;
		}

		cDataShape<cNTuple>* get_item() override {
			if (index == 0 && is_loaded) {

				cNTuple* tuple = new cNTuple(&sd_3d);
				tuple->SetValue(0, X, nullptr);
				tuple->SetValue(1, Y, nullptr);
				tuple->SetValue(2, Z, nullptr);

				index++;

				auto** tuples = new cNTuple * [1];
				tuples[0] = tuple;
				auto* shape = cDataShape<cNTuple>::CreateDataShape(DataShape::DS_POINT, tuples, 1, &sd_3d);

				tuple = nullptr;
				tuples = nullptr;

				return shape;
			}

			return nullptr;
		}

		~shpPointZ() {}
	};
}