#pragma once
#include "../shpRecordContent.h"
#include "../../../container/pArray.h"


namespace nsShapeFile {
	struct shpMultiPoint : public shpRecordContent {
		template <class T>
		using Array = nspArray::pArray<T>;

		Box box = Box{};

		int num_points = 0;

		Array<double> points;


		bool load(FileHandler* fh, Header& header) override {
			index = 0;
			is_loaded = false;
			shape_type = fh->get_int();
			if (shape_type != 8) {
				return is_loaded;
			}
			en_shape_type = shpShapeType::MULTIPOINT;

			box.Xmin = fh->get_double();
			box.Ymin = fh->get_double();
			box.Xmax = fh->get_double();
			box.Ymax = fh->get_double();

			num_points = fh->get_int();

			for (int i = 0; i < num_points * 2; i++) {
				points.push_back(fh->get_double());
			}

			is_loaded = true;
			return is_loaded;
		}


		shpShapeType get_shape_type() override {
			return shpShapeType::MULTIPOINT;
		}

		cDataType* get_item() override {
			if (index < num_points) {

				cNTuple* tuple = new cNTuple(&sd_2d);
				tuple->SetValue(0, points[index * 2], nullptr);
				tuple->SetValue(1, points[index * 2 + 1], nullptr);

				index++;

				return tuple;
			}

			return nullptr;
		}
	};
}