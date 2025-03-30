#pragma once
#include "../shpRecordContent.h"
#include "../../../container/pArray.h"


namespace nsShapeFile {
	struct shpPolygon : public shpRecordContent {
		template <class T>
		using Array = nspArray::pArray<T>;

		Box box = Box{};

		int num_parts = 0;
		int num_points = 0;

		Array<int> parts;
		Array<double> points;


		bool load(FileHandler* fh, Header& header) override {
			index = 0;
			points = Array<double>();
			parts = Array<int>();
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


		shpShapeType get_shape_type() override {
			return shpShapeType::POLYGON;
		}

		cDataType* get_item() override {
			if (index < num_parts) {
				auto part_start = parts[index++];
				auto part_end = index < parts.size() ? parts[index] : points.size() / 2;

				auto tuple_ptr_arr = Array<cNTuple*>();
				for (auto i = part_start * 2; i < part_end * 2; i += 2) {
					auto* tuple_ptr = new cNTuple(&sd_2d);
					tuple_ptr->SetValue(0, points[i], nullptr);
					tuple_ptr->SetValue(1, points[i + 1], nullptr);
					tuple_ptr_arr.push_back(tuple_ptr);
				}

				auto arr_len = tuple_ptr_arr.size();
				auto** tuple_arr_raw = new cNTuple * [arr_len];
				for (size_t i = 0; i < arr_len; i++) {
					tuple_arr_raw[i] = tuple_ptr_arr[i];
				}

				auto* poly = cDataShape<cNTuple>::CreateDataShape(DataShape::DS_POLYGON, tuple_arr_raw, arr_len, &sd_2d);
				return poly;
			}

			return nullptr;
		}

		~shpPolygon() {};
	};
}