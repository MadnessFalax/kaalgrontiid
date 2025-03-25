#pragma once
#include "../buffer/pFileHandler.h"

namespace nsShapeFile {
	using FileHandler = nspFile::pFileHandler;

	enum class shpShapeType {
		NULLSHAPE = 0,
		POINT = 1,
		POLYLINE = 3,
		POLYGON = 5,
		MULTIPOINT = 8,
		POINTZ = 11,
		POLYLINEZ = 13,
		POLYGONZ = 15,
		MULTIPOINTZ = 18,
		POINTM = 21,
		POLYLINEM = 23,
		POLYGONM = 25,
		MULTIPOINTM = 28,
		MULTIPATCH = 31
	};

	struct shpFileHeader {
		shpShapeType en_shape_type = shpShapeType::NULLSHAPE;
		int file_code = 0;
		int unused1 = 0;
		int unused2 = 0;
		int unused3 = 0;
		int unused4 = 0;
		int unused5 = 0;
		int file_length = 0;
		int version = 0;
		int shape_type = 0;
		double x_min = 0;
		double y_min = 0;
		double x_max = 0;
		double y_max = 0;
		double z_min = 0;
		double z_max = 0;
		double m_min = 0;
		double m_max = 0;
		bool is_loaded = false;

		bool load(FileHandler* fh) {
			is_loaded = false;
			file_code = fh->get_int(FileHandler::ByteOrder::BE);
			if (file_code != 9994) {
				return false;
			}
			fh->set_position(24);
			file_length = fh->get_int(FileHandler::ByteOrder::BE);
			version = fh->get_int();
			shape_type = fh->get_int();
			en_shape_type = static_cast<shpShapeType>(shape_type);
			x_min = fh->get_double();
			y_min = fh->get_double();
			x_max = fh->get_double();
			y_max = fh->get_double();
			z_min = fh->get_double();
			z_max = fh->get_double();
			m_min = fh->get_double();
			m_max = fh->get_double();

			is_loaded = true;
			return is_loaded;
		}
	};
}