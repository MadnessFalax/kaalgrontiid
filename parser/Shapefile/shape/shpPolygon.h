#pragma once
#include "../shpRecordContent.h"
#include "../../../container/pArray.h"


namespace nsShapeFile {
	struct shpPolygon : protected shpRecordContent {
		template <class T>
		using Array = nspArray::pArray<T>;

		Box box = Box{};

		int num_parts = 0;
		int num_points = 0;

		Array<int> parts;
		Array<double> points;


		bool load(FileHandler* fh, Header& header) override {
			is_loaded = false;
			shape_type = fh->get_int();
			if (shape_type != 5) {
				return is_loaded;
			}
			en_shape_type = shpShapeType::POLYGON;

			box.Xmin = fh->get_double();
			box.Ymin = fh->get_double();
			box.Xmax = fh->get_double();
			box.Ymax = fh->get_double();

			num_parts = fh->get_int();
			num_points = fh->get_int();

			for (int i = 0; i < num_parts; i++) {
				parts.push_back(fh->get_int());
			}

			for (int i = 0; i < num_points * 2; i++) {
				points.push_back(fh->get_double());
			}

			is_loaded = true;
			return is_loaded;
		}


		virtual shpShapeType get_shape_type() override {
			return shpShapeType::POLYGON;
		}
	};
}