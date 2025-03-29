#pragma once
#include "../shpRecordContent.h"
#include "shpPolygon.h"
#include "../../../container/pArray.h"


namespace nsShapeFile {
	struct shpPolygonM : protected shpPolygon {
		template <class T>
		using Array = nspArray::pArray<T>;

		MRange m_range = MRange{};
		Array<double> m_array = Array<double>();

		bool load(FileHandler* fh, Header& header) override {
			is_loaded = false;
			shape_type = fh->get_int();
			if (shape_type != 25) {
				return is_loaded;
			}
			en_shape_type = shpShapeType::POLYGONM;

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

			if (header.content_length > 44 + num_parts * 4 + num_points * 16) {
				m_range.min = fh->get_double();
			}

			if (header.content_length > 52 + num_parts * 4 + num_points * 16) {
				m_range.max = fh->get_double();
			}

			for (int i = 60 + 4 * num_parts + 16 * num_points; i < header.content_length; i += 8) {
				m_array.push_back(fh->get_double());
			}

			is_loaded = true;
			return is_loaded;
		}


		virtual shpShapeType get_shape_type() override {
			return shpShapeType::POLYGONM;
		}
	};
}