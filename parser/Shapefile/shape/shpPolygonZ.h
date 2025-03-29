#pragma once
#include "shpPolygonM.h"
#include "../../../container/pArray.h"


namespace nsShapeFile {
	struct shpPolygonZ : protected shpPolygonM {
		template <class T>
		using Array = nspArray::pArray<T>;

		ZRange z_range = ZRange{};
		Array<double> z_array = Array<double>();

		bool load(FileHandler* fh, Header& header) override {
			is_loaded = false;
			shape_type = fh->get_int();
			if (shape_type != 15) {
				return is_loaded;
			}
			en_shape_type = shpShapeType::POLYGONZ;

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


		virtual shpShapeType get_shape_type() override {
			return shpShapeType::POLYGONZ;
		}
	};
}