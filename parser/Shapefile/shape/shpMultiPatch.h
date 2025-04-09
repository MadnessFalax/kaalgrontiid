#pragma once
#include "../shpRecordContent.h"
#include "../../../container/pArray.h"


namespace nsShapeFile {
	struct shpMultiPatch : public shpRecordContent {
		template <class T>
		using Array = nspArray::pArray<T>;

		enum class PartType {
			TRIANGLESTRIP,
			TRIANGLEFAN,
			OUTERRING,
			INNERRING,
			FIRSTRING,
			RING
		};

		Box box = Box{};

		int num_parts = 0;
		int num_points = 0;

		Array<int> parts;
		Array<int> part_types;
		Array<PartType> en_part_types;
		Array<double> points;

		ZRange z_range = ZRange{};
		Array<double> z_array = Array<double>();
		MRange m_range = MRange{};
		Array<double> m_array = Array<double>();

	private:
		bool _check_points_are_equal(double a_x, double a_y, double a_z, double b_x, double b_y, double b_z) {
			return a_x == b_x && a_y == b_y && a_z == b_z;
		}
	public:
		bool load(FileHandler* fh, Header& header) override {
			index = 0;
			points = Array<double>();
			parts = Array<int>();
			z_array = Array<double>();
			m_array = Array<double>();
			part_types = Array<int>();
			en_part_types = Array<PartType>();
			is_loaded = false;
			shape_type = fh->get_int();
			if (shape_type != 31) {
				return is_loaded;
			}
			en_shape_type = shpShapeType::MULTIPATCH;

			box.Xmin = fh->get_double();
			box.Ymin = fh->get_double();
			box.Xmax = fh->get_double();
			box.Ymax = fh->get_double();

			num_parts = fh->get_int();
			num_points = fh->get_int();

			for (int i = 0; i < num_parts; i++) {
				parts.push_back(fh->get_int());
			}

			for (int i = 0; i < num_parts; i++) {
				part_types.push_back(fh->get_int());
				en_part_types.push_back(PartType(part_types[i]));
			}

			for (int i = 0; i < num_points * 2; i++) {
				points.push_back(fh->get_double());
			}

			z_range.min = fh->get_double();
			z_range.max = fh->get_double();

			for (int i = 0; i < num_points; i++) {
				z_array.push_back(fh->get_double());
			}

			if (header.content_length > 60 + num_parts * 8 + num_points * 24) {
				m_range.min = fh->get_double();
			}

			if (header.content_length > 68 + num_parts * 8 + num_points * 24) {
				m_range.max = fh->get_double();
			}

			for (int i = 76 + 8 * num_parts + 24 * num_points; i < header.content_length; i += 8) {
				m_array.push_back(fh->get_double());
			}

			is_loaded = true;
			return is_loaded;
		}


		shpShapeType get_shape_type() override {
			return shpShapeType::MULTIPATCH;
		}

		cDataShape<cNTuple>* get_item() override {
			if (index < num_parts) {
				size_t V1 = 0;
				size_t Vn = 0;
				auto tuple_ptr_arr = Array<cNTuple*>();
				int part_start = 0;
				int part_end = 0;
				size_t arr_len = 0;
				cNTuple** tuple_arr_raw = nullptr;
				cDataShape<cNTuple>* poly = nullptr;
				cNTuple* tuple_ptr = nullptr;

				switch (en_part_types[index]) {
				case PartType::TRIANGLESTRIP:
					// polygon vertex order is: V0, V1, V3, V5 ... V4, V2, V0
					part_start = parts[index++];
					part_end = index < parts.size() ? parts[index] : points.size() / 2;

					tuple_ptr = new cNTuple(&sd_3d);
					tuple_ptr->SetValue(0, points[part_start * 2], nullptr);
					tuple_ptr->SetValue(1, points[part_start * 2 + 1], nullptr);
					tuple_ptr->SetValue(2, z_array[part_start * 2 / 2], nullptr);
					tuple_ptr_arr.push_back(tuple_ptr);

					for (auto i = part_start * 2 + 2; i < part_end * 2; i += 4) {
						tuple_ptr = new cNTuple(&sd_3d);
						tuple_ptr->SetValue(0, points[i], nullptr);
						tuple_ptr->SetValue(1, points[i + 1], nullptr);
						tuple_ptr->SetValue(2, z_array[i / 2], nullptr);
						tuple_ptr_arr.push_back(tuple_ptr);
					}

					for (auto i = (part_end - 1 - part_start % 2 == 0) ? part_end * 2 - 2 : part_end * 2 - 4; i >= part_start * 2; i -= 4) {
						tuple_ptr = new cNTuple(&sd_3d);
						tuple_ptr->SetValue(0, points[i], nullptr);
						tuple_ptr->SetValue(1, points[i + 1], nullptr);
						tuple_ptr->SetValue(2, z_array[i / 2], nullptr);
						tuple_ptr_arr.push_back(tuple_ptr);
					}

					arr_len = tuple_ptr_arr.size();
					tuple_arr_raw = new cNTuple * [arr_len];
					for (size_t i = 0; i < arr_len; i++) {
						tuple_arr_raw[i] = tuple_ptr_arr[i];
					}

					poly = cDataShape<cNTuple>::CreateDataShape(DataShape::DS_POLYGON, tuple_arr_raw, arr_len, &sd_3d);
					return poly;
				case PartType::TRIANGLEFAN:
					// if V1 == Vn, V0 is inside the polygon (it should be omitted), else V0 is part of the polygon boundary (should be included)
					part_start = parts[index++];
					part_end = index < parts.size() ? parts[index] : points.size() / 2;
					V1 = part_start * 2 + 2;
					Vn = part_end * 2 - 2;

					if (_check_points_are_equal(points[V1], points[V1 + 1], z_array[V1 / 2], points[Vn], points[Vn + 1], z_array[Vn / 2])) {
						part_start += 1;
					}

					for (auto i = part_start * 2; i < part_end * 2; i += 2) {
						tuple_ptr = new cNTuple(&sd_3d);
						tuple_ptr->SetValue(0, points[i], nullptr);
						tuple_ptr->SetValue(1, points[i + 1], nullptr);
						tuple_ptr->SetValue(2, z_array[i / 2], nullptr);
						tuple_ptr_arr.push_back(tuple_ptr);
					}

					arr_len = tuple_ptr_arr.size();
					tuple_arr_raw = new cNTuple * [arr_len];
					for (size_t i = 0; i < arr_len; i++) {
						tuple_arr_raw[i] = tuple_ptr_arr[i];
					}

					poly = cDataShape<cNTuple>::CreateDataShape(DataShape::DS_POLYGON, tuple_arr_raw, arr_len, &sd_3d);
					return poly;
				default:
					part_start = parts[index++];
					part_end = index < parts.size() ? parts[index] : points.size() / 2;

					for (auto i = part_start * 2; i < part_end * 2; i += 2) {
						tuple_ptr = new cNTuple(&sd_3d);
						tuple_ptr->SetValue(0, points[i], nullptr);
						tuple_ptr->SetValue(1, points[i + 1], nullptr);
						tuple_ptr->SetValue(2, z_array[i / 2], nullptr);
						tuple_ptr_arr.push_back(tuple_ptr);
					}

					arr_len = tuple_ptr_arr.size();
					tuple_arr_raw = new cNTuple * [arr_len];
					for (size_t i = 0; i < arr_len; i++) {
						tuple_arr_raw[i] = tuple_ptr_arr[i];
					}

					poly = cDataShape<cNTuple>::CreateDataShape(DataShape::DS_POLYGON, tuple_arr_raw, arr_len, &sd_3d);
					return poly;
				}
			}

			return nullptr;
		}

		~shpMultiPatch() {};
	};
}