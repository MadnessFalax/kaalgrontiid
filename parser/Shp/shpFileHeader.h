#pragma once
#include "../buffer/pFileHandler.h"

namespace nsShapeFile {
	using FileHandler = nspFile::pFileHandler;

	struct shpHeader {
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

		shpHeader() = default;

		bool load(FileHandler* fh) {
			file_code = fh->get_int();
			if (file_code != 9994) {
				return false;
			}
			fh->set_position(24);
			file_length = fh->get_int();
			version = fh->get_int();
			shape_type = fh->get_int();
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