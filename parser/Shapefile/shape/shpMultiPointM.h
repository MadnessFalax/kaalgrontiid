#pragma once
#include "shpMultiPoint.h"
#include "../../../container/pArray.h"


namespace nsShapeFile {
	struct shpMultiPointM : public shpMultiPoint {
		template <class T>
		using Array = nspArray::pArray<T>;

		MRange m_range = MRange{};

		Array<double> m_array = Array<double>();


		bool load(FileHandler* fh, Header& header) override {
			index = 0;
			points = Array<double>();
			m_array = Array<double>();
			is_loaded = false;
			shape_type = fh->get_int();
			if (shape_type != 28) {
				return is_loaded;
			}
			en_shape_type = shpShapeType::MULTIPOINTM;

			box.Xmin = fh->get_double();
			box.Ymin = fh->get_double();
			box.Xmax = fh->get_double();
			box.Ymax = fh->get_double();

			num_points = fh->get_int();

			for (int i = 0; i < num_points * 2; i++) {
				points.push_back(fh->get_double());
			}

			if (header.content_length > 40 + num_points * 16) {
				m_range.min = fh->get_double();
			}

			if (header.content_length > 48 + num_points * 16) {
				m_range.max = fh->get_double();
			}

			for (int i = 56 + 16 * num_points; i < header.content_length; i += 8) {
				m_array.push_back(fh->get_double());
			}

			is_loaded = true;
			return is_loaded;
		}


		shpShapeType get_shape_type() override {
			return shpShapeType::MULTIPOINTM;
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

		~shpMultiPointM() {}
	};
}