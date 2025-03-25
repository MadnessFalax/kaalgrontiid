#pragma once
#include "../buffer/pFileHandler.h"
#include "shpFileHeader.h"
#include "shpRecordIndex.h"
#include "shpRecordHeader.h"
#include "shpRecordContent.h"

namespace nsShapeFile {

	using FileHandler = nspFile::pFileHandler;

	class shpParser {
	private:
		FileHandler* _fh = nullptr;
		shpFileHeader _file_header;
		shpRecordIndex _record_index;
		shpRecordHeader _record_header;
		shpRecordContent _record_content;

		bool _is_loaded = false;

	public:
		shpParser(FileHandler* fh) : _fh(fh) {}

		bool load() {
			_is_loaded = false;
			_file_header.load(_fh);
			_record_index.load(_fh);
			_record_header.load(_fh);
			_record_content.load(_fh);
			_is_loaded = true;
			return _is_loaded;
		}

		shpFileHeader* get_file_header() {
			return &_file_header;
		}

		shpRecordIndex* get_record_index() {
			return &_record_index;
		}

		shpRecordHeader* get_record_header() {
			return &_record_header;
		}

		shpRecordContent* get_record_content() {
			return &_record_content;
		}
	};

}