#pragma once
#include "../shpRecordContent.h"
#include "shpPoint.h"

namespace nsShapeFile {
	struct shpPointM : public shpPoint {
		double M = 0.0;

		bool load(FileHandler* fh, Header& header) override {
			is_loaded = false;
			shape_type = fh->get_int();
			if (shape_type != 21) {
				return is_loaded;
			}
			en_shape_type = shpShapeType::POINTM;
			X = fh->get_double();
			Y = fh->get_double();
			M = fh->get_double();
			is_loaded = true;
			return is_loaded;
		}


		shpShapeType get_shape_type() override {
			return shpShapeType::POINTM;
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
	};
}