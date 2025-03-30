#pragma once

#include "ctime"
#include "pExporter.h"
#include "../container/pString.h"
#include "../container/pArray.h"
#include "../container/pMap.h"

#include "cPolygon.h"
#include "cLineString.h"
#include "cSphere.h"

namespace nsShapeFile {
	using String = nspString::pString;
	using FileWriter = nspFile::pFileWriter;
	template<class T>
	using Array = nspArray::pArray<T>;
	using Mapping = nspMap::pMap<size_t, Array<size_t>*>;

	class shpExporter : public nspExporter::pExporter {
		FileWriter* _poly_shx = nullptr;
		FileWriter* _ls_shx = nullptr;
		FileWriter* _point_shx = nullptr;
		FileWriter* _poly_shp = nullptr;
		FileWriter* _ls_shp = nullptr;
		FileWriter* _point_shp = nullptr;

		String _target_dir = "";
		time_t _timestamp = 0;
		String _timestamp_str = "";

		bool _write_file_header(int shape_type, FileWriter* file) {
			// code
			file->write_int(9994, FileWriter::ByteOrder::BE);
			// unuseds
			file->write_int(0, FileWriter::ByteOrder::BE);
			file->write_int(0, FileWriter::ByteOrder::BE);
			file->write_int(0, FileWriter::ByteOrder::BE);
			file->write_int(0, FileWriter::ByteOrder::BE);
			file->write_int(0, FileWriter::ByteOrder::BE);
			// file length
			file->write_int(50, FileWriter::ByteOrder::BE);
			// version
			file->write_int(1000);
			// shape type	
			file->write_int(shape_type);
			// bounding box
			file->write_double(-180.0);
			file->write_double(-90.0);
			file->write_double(180.0);
			file->write_double(90.0);
			file->write_double(-9999.0);
			file->write_double(9999.0);
			file->write_double(-9999.0);
			file->write_double(9999.0);
		}

	public:
		shpExporter(String path_to_target_directory) {
			_target_dir = path_to_target_directory;

			_timestamp = time(nullptr);
			char* buffer = new char[32];
			snprintf(buffer, 32, "%llu", _timestamp);
			_timestamp_str = buffer;
			delete[] buffer;

			_poly_shx = new FileWriter(_target_dir + "polygon_" + _timestamp_str + ".shx");
			_poly_shp = new FileWriter(_target_dir + "polygon_" + _timestamp_str + ".shp");
			_point_shx = new FileWriter(_target_dir + "point_" + _timestamp_str + ".shx");
			_point_shp = new FileWriter(_target_dir + "point_" + _timestamp_str + ".shp");
			_ls_shx = new FileWriter(_target_dir + "linestring_" + _timestamp_str + ".shx");
			_ls_shp = new FileWriter(_target_dir + "linestring_" + _timestamp_str + ".shp");
		}

		~shpExporter() {
			delete _poly_shx;
			delete _poly_shp;
			delete _point_shx;
			delete _point_shp;
			delete _ls_shx;
			delete _ls_shp;
		};
		
		bool begin() override {

		}

		bool end() override {
		
		}

		bool export_item(cNTuple* point) {

		}

		bool export_item(cLineString<cNTuple>* line) {

		}

		bool export_item(cPolygon<cNTuple>* poly) {

		}

		bool export_item(cSphere<cNTuple>* sphere) {
			return export_item(sphere->GetVertex(0));
		}
	};
}