#pragma once
#include "shpMultiPointM.h"
#include "../../../container/pArray.h"


namespace nsShapeFile {
	struct shpMultiPointZ : public shpMultiPointM {
		template <class T>
		using Array = nspArray::pArray<T>;

		ZRange z_range = ZRange{};
		Array<double> z_array = Array<double>();

		bool load(FileHandler* fh, Header& header) override {
			index = 0;
			points = Array<double>();
			m_array = Array<double>();
			z_array = Array<double>();
			is_loaded = false;
			shape_type = fh->get_int();
			if (shape_type != 18) {
				return is_loaded;
			}
			en_shape_type = shpShapeType::MULTIPOINTZ;

			box.Xmin = fh->get_double();
			box.Ymin = fh->get_double();
			box.Xmax = fh->get_double();
			box.Ymax = fh->get_double();

			num_points = fh->get_int();

			for (int i = 0; i < num_points * 2; i++) {
				points.push_back(fh->get_double());
			}

			z_range.min = fh->get_double();
			z_range.max = fh->get_double();

			for (int i = 0; i < num_points; i++) {
				z_array.push_back(fh->get_double());
			}

			if (header.content_length > 56 + num_points * 24) {
				m_range.min = fh->get_double();
			}

			if (header.content_length > 64 + num_points * 24) {
				m_range.max = fh->get_double();
			}

			for (int i = 72 + 24 * num_points; i < header.content_length; i += 8) {
				m_array.push_back(fh->get_double());
			}

			is_loaded = true;
			return is_loaded;
		}


		shpShapeType get_shape_type() override {
			return shpShapeType::MULTIPOINTZ;
		}

		cDataShape<cNTuple>* get_item() override {
			if (index < num_points) {

				cNTuple* tuple = new cNTuple(&sd_3d);
				tuple->SetValue(0, points[index * 2], nullptr);
				tuple->SetValue(1, points[index * 2 + 1], nullptr);
				tuple->SetValue(2, z_array[index], nullptr);

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

		~shpMultiPointZ() {}
	};
}