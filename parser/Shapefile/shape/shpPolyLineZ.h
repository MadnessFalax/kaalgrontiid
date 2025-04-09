#pragma once
#include "shpPolyLineM.h"
#include "../../../container/pArray.h"


namespace nsShapeFile {
	struct shpPolyLineZ : public shpPolyLineM {
		template <class T>
		using Array = nspArray::pArray<T>;

		ZRange z_range = ZRange{};
		Array<double> z_array = Array<double>();

		bool load(FileHandler* fh, Header& header) override {
			index = 0;
			points = Array<double>();
			parts = Array<int>();
			m_array = Array<double>();
			z_array = Array<double>();
			is_loaded = false;
			shape_type = fh->get_int();
			if (shape_type != 13) {
				return is_loaded;
			}
			en_shape_type = shpShapeType::POLYLINEZ;

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

			z_range.min = fh->get_double();
			z_range.max = fh->get_double();

			for (int i = 0; i < num_points; i++) {
				z_array.push_back(fh->get_double());
			}

			if (header.content_length > 60 + num_parts * 4 + num_points * 24) {
				m_range.min = fh->get_double();
			}

			if (header.content_length > 68 + num_parts * 4 + num_points * 24) {
				m_range.max = fh->get_double();
			}

			for (int i = 76 + 4 * num_parts + 24 * num_points; i < header.content_length; i += 8) {
				m_array.push_back(fh->get_double());
			}

			is_loaded = true;
			return is_loaded;
		}


		shpShapeType get_shape_type() override {
			return shpShapeType::POLYLINEZ;
		}

		cDataShape<cNTuple>* get_item() override {
			if (index < num_parts) {
				auto part_start = parts[index++];
				auto part_end = index < parts.size() ? parts[index] : points.size() / 2;

				auto tuple_ptr_arr = Array<cNTuple*>();
				for (auto i = part_start * 2; i < part_end * 2; i += 2) {
					auto* tuple_ptr = new cNTuple(&sd_3d);
					tuple_ptr->SetValue(0, points[i], nullptr);
					tuple_ptr->SetValue(1, points[i + 1], nullptr);
					tuple_ptr->SetValue(2, z_array[i / 2], nullptr);
					tuple_ptr_arr.push_back(tuple_ptr);
				}

				auto arr_len = tuple_ptr_arr.size();
				auto** tuple_arr_raw = new cNTuple*[arr_len];
				for (size_t i = 0; i < arr_len; i++) {
					tuple_arr_raw[i] = tuple_ptr_arr[i];
				}

				auto* line = cDataShape<cNTuple>::CreateDataShape(DataShape::DS_LINESTRING, tuple_arr_raw, arr_len, &sd_3d);
				return line;
			}

			return nullptr;
		}

		~shpPolyLineZ() {}
	};
}