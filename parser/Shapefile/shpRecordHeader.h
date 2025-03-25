#pragma once
#include "../buffer/pFileHandler.h"


namespace nsShapeFile {
	using FileHandler = nspFile::pFileHandler;

	struct shpRecordHeader {
		int record_number = 0;
		int content_length = 0;
		bool is_loaded = false;

		bool load(FileHandler* fh) {
			is_loaded = false;
			record_number = fh->get_int(FileHandler::ByteOrder::BE);
			content_length = fh->get_int(FileHandler::ByteOrder::BE);
			is_loaded = true;
			return is_loaded;
		}
	};
}