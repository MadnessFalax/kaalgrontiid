#pragma once

#include <ctime>
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
		enum class ExportType {
			POLYGON,
			LINESTRING,
			POINT
		};

		FileWriter* _poly_shx = nullptr;
		FileWriter* _ls_shx = nullptr;
		FileWriter* _point_shx = nullptr;

		FileWriter* _poly_shp = nullptr;
		FileWriter* _ls_shp = nullptr;
		FileWriter* _point_shp = nullptr;

		FileWriter* _poly_dbf = nullptr;
		FileWriter* _ls_dbf = nullptr;
		FileWriter* _point_dbf = nullptr;

		FileWriter* _poly_z_shx = nullptr;
		FileWriter* _ls_z_shx = nullptr;
		FileWriter* _point_z_shx = nullptr;

		FileWriter* _poly_z_shp = nullptr;
		FileWriter* _ls_z_shp = nullptr;
		FileWriter* _point_z_shp = nullptr;

		FileWriter* _poly_z_dbf = nullptr;
		FileWriter* _ls_z_dbf = nullptr;
		FileWriter* _point_z_dbf = nullptr;

		String _target_dir = "";
		time_t _timestamp = 0;
		String _timestamp_str = "";

		int _poly_len = 50;
		int _ls_len = 50;
		int _point_len = 50;
		int _poly_z_len = 50;
		int _ls_z_len = 50;
		int _point_z_len = 50;

		int _poly_number = 1;
		int _ls_number = 1;
		int _point_number = 1;
		int _poly_z_number = 1;
		int _ls_z_number = 1;
		int _point_z_number = 1;

		ExportType _export_type = ExportType{};

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
			return true;
		}

		bool _write_dbf_header(FileWriter* file) {
			// dBASE 3
			file->write_char(0x03);
			// last update
			time_t t = time(nullptr);
			tm* time = localtime(&t);
			file->write_char(time->tm_year);
			file->write_char(time->tm_mon);
			file->write_char(time->tm_mday);
			// number of records
			file->write_int(0);
			// header length
			file->write_short(0);
			// record length
			file->write_short(1);
			// reserved
			file->write_int(0);
			file->write_int(0);
			file->write_int(0);
			file->write_int(0);
			file->write_int(0);

			// dummy field descriptor - shapefile states that at least one field is required and there should be one entry for each shape
			file->write_char('d');
			file->write_char('u');
			file->write_char('m');
			file->write_char('m');
			file->write_char('y');
			file->write_int(0);
			file->write_int(0);
			// type
			file->write_char('L');
			// data address
			file->write_int(0);
			// length
			file->write_char(1);
			// decimal count
			file->write_char(0);
			// reserved
			file->write_short(0);
			// work area id
			file->write_char(0);
			// reserved
			file->write_char(0);
			// set fields
			file->write_char(0);
			// reserved
			file->write_int(0);
			file->write_int(0);
			// descriptors end
			file->write_char(0x0d);

			return true;
		}

		bool _write_dbf_record(FileWriter* file) {
			// deletion marker
			file->write_char(' ');
			// write True
			file->write_char('T');
			return true;
		}

	public:
		shpExporter(String path_to_target_directory, ExportType exported_type) {
			_export_type = exported_type;
			_target_dir = path_to_target_directory;

			_timestamp = time(nullptr);
			char* buffer = new char[32];
			snprintf(buffer, 32, "%llu", _timestamp);
			_timestamp_str = buffer;
			delete[] buffer;

			_poly_shx = new FileWriter(_target_dir + "_polygon_" + _timestamp_str + ".shx", true);
			_poly_shp = new FileWriter(_target_dir + "_polygon_" + _timestamp_str + ".shp", true);
			_point_shx = new FileWriter(_target_dir + "_point_" + _timestamp_str + ".shx", true);
			_point_shp = new FileWriter(_target_dir + "_point_" + _timestamp_str + ".shp", true);
			_ls_shx = new FileWriter(_target_dir + "_linestring_" + _timestamp_str + ".shx", true);
			_ls_shp = new FileWriter(_target_dir + "_linestring_" + _timestamp_str + ".shp", true);
			_poly_z_shx = new FileWriter(_target_dir + "_polygonz_" + _timestamp_str + ".shx", true);
			_poly_z_shp = new FileWriter(_target_dir + "_polygonz_" + _timestamp_str + ".shp", true);
			_point_z_shx = new FileWriter(_target_dir + "_pointz_" + _timestamp_str + ".shx", true);
			_point_z_shp = new FileWriter(_target_dir + "_pointz_" + _timestamp_str + ".shp", true);
			_ls_z_shx = new FileWriter(_target_dir + "_linestringz_" + _timestamp_str + ".shx", true);
			_ls_z_shp = new FileWriter(_target_dir + "_linestringz_" + _timestamp_str + ".shp", true);
		}

		~shpExporter() {
			delete _poly_shx;
			delete _poly_shp;
			delete _point_shx;
			delete _point_shp;
			delete _ls_shx;
			delete _ls_shp;
			delete _poly_z_shx;
			delete _poly_z_shp;
			delete _point_z_shx;
			delete _point_z_shp;
			delete _ls_z_shx;
			delete _ls_z_shp;
		};
		
		bool begin() override {
			_write_file_header(5, _poly_shx);
			_write_file_header(5, _poly_shp);
			_write_file_header(3, _ls_shx);
			_write_file_header(3, _ls_shp);
			_write_file_header(1, _point_shx);
			_write_file_header(1, _point_shp);
			_write_file_header(15, _poly_z_shx);
			_write_file_header(15, _poly_z_shp);
			_write_file_header(13, _ls_z_shx);
			_write_file_header(13, _ls_z_shp);
			_write_file_header(11, _point_z_shx);
			_write_file_header(11, _point_z_shp);
			return true;
		}

		bool end() override {
			_poly_shx->set_position(24);
			_poly_shx->write_int(_poly_len, FileWriter::ByteOrder::BE);
			_poly_shp->set_position(24);
			_poly_shp->write_int(_poly_len, FileWriter::ByteOrder::BE);
			
			_point_shx->set_position(24);
			_point_shx->write_int(_point_len, FileWriter::ByteOrder::BE);
			_point_shp->set_position(24);
			_point_shp->write_int(_point_len, FileWriter::ByteOrder::BE);
			
			_ls_shx->set_position(24);
			_ls_shx->write_int(_ls_len, FileWriter::ByteOrder::BE);
			_ls_shp->set_position(24);
			_ls_shp->write_int(_ls_len, FileWriter::ByteOrder::BE);
			
			_poly_z_shx->set_position(24);
			_poly_z_shx->write_int(_poly_z_len, FileWriter::ByteOrder::BE);
			_poly_z_shp->set_position(24);
			_poly_z_shp->write_int(_poly_z_len, FileWriter::ByteOrder::BE);
			
			_point_z_shx->set_position(24);
			_point_z_shx->write_int(_point_z_len, FileWriter::ByteOrder::BE);
			_point_z_shp->set_position(24);
			_point_z_shp->write_int(_point_z_len, FileWriter::ByteOrder::BE);
			
			_ls_z_shx->set_position(24);
			_ls_z_shx->write_int(_ls_z_len, FileWriter::ByteOrder::BE);
			_ls_z_shp->set_position(24);
			_ls_z_shp->write_int(_ls_z_len, FileWriter::ByteOrder::BE);

			return true;
		}

		bool export_item(cNTuple* point) {
			auto p_size = point->GetLength();
			if (p_size == 2) {
				_point_shp->write_int(_point_number++, FileWriter::ByteOrder::BE);
				_point_shp->write_int(10, FileWriter::ByteOrder::BE);
				_point_shp->write_int(1);
				_point_shp->write_double(point->GetDouble(0, nullptr));
				_point_shp->write_double(point->GetDouble(1, nullptr));

				_point_shx->write_int(_point_len, FileWriter::ByteOrder::BE);
				_point_len += 14;
				_point_shx->write_int(10, FileWriter::ByteOrder::BE);
			}
			else {
				_point_z_shp->write_int(_point_z_number++, FileWriter::ByteOrder::BE);
				_point_z_shp->write_int(14, FileWriter::ByteOrder::BE);
				_point_z_shp->write_int(11);
				_point_z_shp->write_double(point->GetDouble(0, nullptr));
				_point_z_shp->write_double(point->GetDouble(1, nullptr));
				_point_z_shp->write_double(point->GetDouble(2, nullptr));

				_point_z_shx->write_int(_point_z_len, FileWriter::ByteOrder::BE);
				_point_z_len += 18;
				_point_z_shx->write_int(14, FileWriter::ByteOrder::BE);
			}
			return true;
		}

		bool export_item(cLineString<cNTuple>* line) {
			auto max_dim = 2;
			auto** vtx_col = line->GetVerticesCollection();
			auto vtx_count = line->GetVerticesCount();
			int x_min = 0;
			int x_max = 0;
			int y_min = 0;
			int y_max = 0;
			for (decltype(vtx_count) i = 0; i < vtx_count; i++) {
				auto* vtx = vtx_col[i];
				auto p_size = vtx->GetLength();
				if (p_size > max_dim) {
					max_dim = p_size;
				}
				auto x = vtx->GetDouble(0, nullptr);
				auto y = vtx->GetDouble(1, nullptr);

				if (x < x_min) {
					x_min = x;
				}
				if (x > x_max) {
					x_max = x;
				}
				if (y < y_min) {
					y_min = y;
				}
				if (y > y_max) {
					y_max = y;
				}
			}
			if (max_dim == 2) {
				_ls_shp->write_int(_ls_number++, FileWriter::ByteOrder::BE);
				_ls_shp->write_int(24 + 8 * vtx_count, FileWriter::ByteOrder::BE);
				_ls_shp->write_int(3);
				_ls_shp->write_double(x_min);
				_ls_shp->write_double(y_min);
				_ls_shp->write_double(x_max);
				_ls_shp->write_double(y_max);
				_ls_shp->write_int(1);
				_ls_shp->write_int(vtx_count);
				_ls_shp->write_int(0);
				for (decltype(vtx_count) i = 0; i < vtx_count; i++) {
					auto* vtx = vtx_col[i];
					_ls_shp->write_double(vtx->GetDouble(0, nullptr));
					_ls_shp->write_double(vtx->GetDouble(1, nullptr));
				}

				_ls_shx->write_int(_ls_len, FileWriter::ByteOrder::BE);
				_ls_len += 28 + 8 * vtx_count;
				_ls_shx->write_int(24 + 8 * vtx_count, FileWriter::ByteOrder::BE);
			}
			else {
				int z_min = 0;
				int z_max = 0;
				for (decltype(vtx_count) i = 0; i < vtx_count; i++) {
					auto* vtx = vtx_col[i];
					auto z = vtx->GetDouble(2, nullptr);
					if (z < z_min) {
						z_min = z;
					}
					if (z > z_max) {
						z_max = z;
					}
				}

				_ls_z_shp->write_int(_ls_z_number++, FileWriter::ByteOrder::BE);
				_ls_z_shp->write_int(32 + 12 * vtx_count, FileWriter::ByteOrder::BE);
				_ls_z_shp->write_int(13);
				_ls_z_shp->write_double(x_min);
				_ls_z_shp->write_double(y_min);
				_ls_z_shp->write_double(x_max);
				_ls_z_shp->write_double(y_max);
				_ls_z_shp->write_int(1);
				_ls_z_shp->write_int(vtx_count);
				_ls_z_shp->write_int(0);
				for (decltype(vtx_count) i = 0; i < vtx_count; i++) {
					auto* vtx = vtx_col[i];
					_ls_z_shp->write_double(vtx->GetDouble(0, nullptr));
					_ls_z_shp->write_double(vtx->GetDouble(1, nullptr));
				}
				_ls_z_shp->write_double(z_min);
				_ls_z_shp->write_double(z_max);
				for (decltype(vtx_count) i = 0; i < vtx_count; i++) {
					auto* vtx = vtx_col[i];
					_ls_z_shp->write_double(vtx->GetLength() == 3 ? vtx->GetDouble(2, nullptr) : 0.0);
				}

				_ls_z_shx->write_int(_ls_z_len, FileWriter::ByteOrder::BE);
				_ls_z_len += 36 + 12 * vtx_count;
				_ls_z_shx->write_int(32 + 12 * vtx_count, FileWriter::ByteOrder::BE);
			}
			return true;
		}

		bool export_item(cPolygon<cNTuple>* poly) {
			auto max_dim = 2;
			auto** vtx_col = poly->GetVerticesCollection();
			auto vtx_count = poly->GetVerticesCount();
			int x_min = 0;
			int x_max = 0;
			int y_min = 0;
			int y_max = 0;
			for (decltype(vtx_count) i = 0; i < vtx_count; i++) {
				auto* vtx = vtx_col[i];
				auto p_size = vtx->GetLength();
				if (p_size > max_dim) {
					max_dim = p_size;
				}
				auto x = vtx->GetDouble(0, nullptr);
				auto y = vtx->GetDouble(1, nullptr);

				if (x < x_min) {
					x_min = x;
				}
				if (x > x_max) {
					x_max = x;
				}
				if (y < y_min) {
					y_min = y;
				}
				if (y > y_max) {
					y_max = y;
				}
			}
			if (max_dim == 2) {
				_poly_shp->write_int(_poly_number++, FileWriter::ByteOrder::BE);
				_poly_shp->write_int(24 + 8 * vtx_count, FileWriter::ByteOrder::BE);
				_poly_shp->write_int(5);
				_poly_shp->write_double(x_min);
				_poly_shp->write_double(y_min);
				_poly_shp->write_double(x_max);
				_poly_shp->write_double(y_max);
				_poly_shp->write_int(1);
				_poly_shp->write_int(vtx_count);
				_poly_shp->write_int(0);
				for (decltype(vtx_count) i = 0; i < vtx_count; i++) {
					auto* vtx = vtx_col[i];
					_poly_shp->write_double(vtx->GetDouble(0, nullptr));
					_poly_shp->write_double(vtx->GetDouble(1, nullptr));
				}

				_poly_shx->write_int(_poly_len, FileWriter::ByteOrder::BE);
				_poly_len += 28 + 8 * vtx_count;
				_poly_shx->write_int(24 + 8 * vtx_count, FileWriter::ByteOrder::BE);
			}
			else {
				int z_min = 0;
				int z_max = 0;
				for (decltype(vtx_count) i = 0; i < vtx_count; i++) {
					auto* vtx = vtx_col[i];
					auto z = vtx->GetDouble(2, nullptr);
					if (z < z_min) {
						z_min = z;
					}
					if (z > z_max) {
						z_max = z;
					}
				}

				_poly_z_shp->write_int(_poly_z_number++, FileWriter::ByteOrder::BE);
				_poly_z_shp->write_int(32 + 12 * vtx_count, FileWriter::ByteOrder::BE);
				_poly_z_shp->write_int(15);
				_poly_z_shp->write_double(x_min);
				_poly_z_shp->write_double(y_min);
				_poly_z_shp->write_double(x_max);
				_poly_z_shp->write_double(y_max);
				_poly_z_shp->write_int(1);
				_poly_z_shp->write_int(vtx_count);
				_poly_z_shp->write_int(0);
				for (decltype(vtx_count) i = 0; i < vtx_count; i++) {
					auto* vtx = vtx_col[i];
					_poly_z_shp->write_double(vtx->GetDouble(0, nullptr));
					_poly_z_shp->write_double(vtx->GetDouble(1, nullptr));
				}
				_poly_z_shp->write_double(z_min);
				_poly_z_shp->write_double(z_max);
				for (decltype(vtx_count) i = 0; i < vtx_count; i++) {
					auto* vtx = vtx_col[i];
					_poly_z_shp->write_double(vtx->GetLength() == 3 ? vtx->GetDouble(2, nullptr) : 0.0);
				}

				_poly_z_shx->write_int(_poly_z_len, FileWriter::ByteOrder::BE);
				_poly_z_len += 36 + 12 * vtx_count;
				_poly_z_shx->write_int(32 + 12 * vtx_count, FileWriter::ByteOrder::BE);
			}
			return true;
		}

		bool export_item(cSphere<cNTuple>* sphere) {
			return export_item(sphere->GetVertex(0));
		}
	};
}