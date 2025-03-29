#pragma once
#include "shpMultiPointM.h"
#include "../../../container/pArray.h"


namespace nsShapeFile {
	struct shpMultiPointZ : protected shpMultiPointM {
		template <class T>
		using Array = nspArray::pArray<T>;

		ZRange z_range = ZRange{};
		Array<double> z_array = Array<double>();

		bool load(FileHandler* fh, Header& header) override {
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


		virtual shpShapeType get_shape_type() override {
			return shpShapeType::MULTIPOINTZ;
		}
	};
}