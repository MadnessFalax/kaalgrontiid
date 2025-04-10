#pragma once
#include <type_traits>
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

	template <typename U>
	struct get_shape_code;

	template <>
	struct get_shape_code<cNTuple> {
		static constexpr int value = 1;
	};

	template <>
	struct get_shape_code<cPoint<cNTuple>> {
		static constexpr int value = 1;
	};

	template <>
	struct get_shape_code<cLineString<cNTuple>> {
		static constexpr int value = 3;
	};

	template <>
	struct get_shape_code<cPolygon<cNTuple>> {
		static constexpr int value = 5;
	};

	template <>
	struct get_shape_code<cSphere<cNTuple>> {
		static constexpr int value = 1;
	};

	template <typename U>
	struct get_z_shape_code;

	template <>
	struct get_z_shape_code<cNTuple> {
		static constexpr int value = 11;
	};

	template <>
	struct get_z_shape_code<cPoint<cNTuple>> {
		static constexpr int value = 11;
	};

	template <>
	struct get_z_shape_code<cLineString<cNTuple>> {
		static constexpr int value = 13;
	};

	template <>
	struct get_z_shape_code<cPolygon<cNTuple>> {
		static constexpr int value = 15;
	};

	template <>
	struct get_z_shape_code<cSphere<cNTuple>> {
		static constexpr int value = 11;
	};

	template <typename T>
	class shpExporter : public nspExporter::pExporter {
		FileWriter* _target_shx = nullptr;
		FileWriter* _target_shp = nullptr;
		FileWriter* _target_dbf = nullptr;

		FileWriter* _target_z_shx = nullptr;
		FileWriter* _target_z_shp = nullptr;
		FileWriter* _target_z_dbf = nullptr;

		String _target_dir = "";
		time_t _timestamp = 0;
		String _timestamp_str = "";
		String _last_target_path = "";
		String _last_z_target_path = "";

		int _target_len = 50;
		int _target_z_len = 50;

		int _target_number = 1;
		int _target_z_number = 1;

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
			file->write_char(time->tm_mon + 1);
			file->write_char(time->tm_mday);
			// number of records
			file->write_int(0);
			// header length
			file->write_short(65);
			// record length
			file->write_short(2);
			// reserved
			file->write_int(0);
			file->write_int(0);
			file->write_int(0);
			file->write_int(0);
			file->write_int(0);

			// dummy field descriptor - shapefile docs state that at least one field is required and there should be one entry for each shape
			file->write_char('d');
			file->write_char('u');
			file->write_char('m');
			file->write_char('m');
			file->write_char('y');
			file->write_int(0);
			file->write_short(0);
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
			file->write_int(0);
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

		bool _write_dbf_size(FileWriter* file, unsigned int record_count) {
			file->set_position(4);
			file->write_int(reinterpret_cast<int&>(record_count));
			return true;
		}

		bool _start_new_export() {
			bool ret_val = true;
			ret_val &= end();
			ret_val &= begin();
			return ret_val;
		}

		bool _reset_timestamp() {
			_timestamp = time(nullptr);
			char* buffer = new char[32];
			snprintf(buffer, 32, "%llu", _timestamp);
			_timestamp_str = buffer;
			delete[] buffer;

			return true;
		}

	public:
		shpExporter() = delete;

		template <
			typename U = T, 
			typename = std::enable_if_t <
				std::is_same_v<U, cNTuple> || 
				std::is_same_v<U, cLineString<cNTuple>> || 
				std::is_same_v<U, cPolygon<cNTuple>> ||
				std::is_same_v<U, cSphere<cNTuple>> || 
				std::is_same_v<U, cPoint<cNTuple>>
			>
		>
		shpExporter(String path_to_target_directory) {
			_target_dir = path_to_target_directory;
		}

		~shpExporter() {
			delete _target_shx;
			delete _target_shp;
			delete _target_z_shx;
			delete _target_z_shp;
			delete _target_dbf;
			delete _target_z_dbf;
		};

		String get_last_target_path() {
			return _last_target_path;
		}

		String get_last_z_target_path() {
			return _last_z_target_path;
		}
		
		bool begin() override {
			_reset_timestamp();
			_target_len = 50;
			_target_z_len = 50;

			_target_number = 1;
			_target_z_number = 1;

			_last_target_path = _target_dir + "_target_" + _timestamp_str;
			_last_z_target_path = _target_dir + "_z_target_" + _timestamp_str;

			_target_shx = new FileWriter(_last_target_path + ".shx", true);
			_target_shp = new FileWriter(_last_target_path + ".shp", true);
			_target_dbf = new FileWriter(_last_target_path + ".dbf", true);

			_target_z_shx = new FileWriter(_last_z_target_path + ".shx", true);
			_target_z_shp = new FileWriter(_last_z_target_path + ".shp", true);
			_target_z_dbf = new FileWriter(_last_z_target_path + ".dbf", true);

			_write_file_header(get_shape_code<T>::value, _target_shx);
			_write_file_header(get_shape_code<T>::value, _target_shp);
			_write_dbf_header(_target_dbf);

			_write_file_header(get_z_shape_code<T>::value, _target_z_shx);
			_write_file_header(get_z_shape_code<T>::value, _target_z_shp);
			_write_dbf_header(_target_z_dbf);
			
			return true;
		}

		bool end() override {
			_target_shx->set_position(24);
			_target_shx->write_int(_target_len, FileWriter::ByteOrder::BE);
			_target_shp->set_position(24);
			_target_shp->write_int(_target_len, FileWriter::ByteOrder::BE);

			_target_z_shx->set_position(24);
			_target_z_shx->write_int(_target_z_len, FileWriter::ByteOrder::BE);
			_target_z_shp->set_position(24);
			_target_z_shp->write_int(_target_z_len, FileWriter::ByteOrder::BE);

			_write_dbf_size(_target_dbf, _target_number - 1);
			_write_dbf_size(_target_z_dbf, _target_z_number - 1);

			delete _target_shx;
			_target_shx = nullptr;
			delete _target_shp;
			_target_shp = nullptr;
			delete _target_z_shx;
			_target_z_shx = nullptr;
			delete _target_z_shp;
			_target_z_shp = nullptr;
			delete _target_dbf;
			_target_dbf = nullptr;
			delete _target_z_dbf;
			_target_z_dbf = nullptr;

			return true;
		}

		template <typename U = T, typename = std::enable_if_t<std::is_same_v<U, cNTuple> || std::is_same_v<U, cSphere<cNTuple>> || std::is_same_v<U, cPoint<cNTuple>>>>
		bool export_item(cNTuple* point) {
			auto p_size = point->GetLength();
			if (p_size == 2) {
				auto record_len = 14;
				if (record_len + _target_len >= 0x7FFFFFFF || record_len + _target_len < 0) {
					_start_new_export();
				}
				_target_shp->write_int(_target_number++, FileWriter::ByteOrder::BE);
				_target_shp->write_int(10, FileWriter::ByteOrder::BE);
				_target_shp->write_int(1);
				_target_shp->write_double(point->GetDouble(0, nullptr));
				_target_shp->write_double(point->GetDouble(1, nullptr));

				_target_shx->write_int(_target_len, FileWriter::ByteOrder::BE);
				_target_len += record_len;
				_target_shx->write_int(10, FileWriter::ByteOrder::BE);
				_write_dbf_record(_target_dbf);	
			}
			else {
				auto record_len = 18;
				if (record_len + _target_z_len >= 0x7FFFFFFF || record_len + _target_z_len < 0) {
					_start_new_export();
				}
				_target_z_shp->write_int(_target_z_number++, FileWriter::ByteOrder::BE);
				_target_z_shp->write_int(14, FileWriter::ByteOrder::BE);
				_target_z_shp->write_int(11);
				_target_z_shp->write_double(point->GetDouble(0, nullptr));
				_target_z_shp->write_double(point->GetDouble(1, nullptr));
				_target_z_shp->write_double(point->GetDouble(2, nullptr));

				_target_z_shx->write_int(_target_z_len, FileWriter::ByteOrder::BE);
				_target_z_len += record_len;
				_target_z_shx->write_int(14, FileWriter::ByteOrder::BE);
				_write_dbf_record(_target_z_dbf);
			}
			return true;
		}

		template <typename U = T, typename = std::enable_if_t<std::is_same_v<U, cNTuple> || std::is_same_v<U, cSphere<cNTuple>> || std::is_same_v<U, cPoint<cNTuple>>>>
		bool export_item(cPoint<cNTuple>* point) {
			return export_item(point->GetVertex(0));
		}

		template <typename U = T, typename = std::enable_if_t<std::is_same_v<U, cLineString<cNTuple>>>>
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
				auto record_len = 28 + 8 * vtx_count;
				if (record_len + _target_len >= 0x7FFFFFFF || record_len + _target_len < 0) {
					_start_new_export();
				}
				_target_shp->write_int(_target_number++, FileWriter::ByteOrder::BE);
				_target_shp->write_int(24 + 8 * vtx_count, FileWriter::ByteOrder::BE);
				_target_shp->write_int(3);
				_target_shp->write_double(x_min);
				_target_shp->write_double(y_min);
				_target_shp->write_double(x_max);
				_target_shp->write_double(y_max);
				_target_shp->write_int(1);
				_target_shp->write_int(vtx_count);
				_target_shp->write_int(0);
				for (decltype(vtx_count) i = 0; i < vtx_count; i++) {
					auto* vtx = vtx_col[i];
					_target_shp->write_double(vtx->GetDouble(0, nullptr));
					_target_shp->write_double(vtx->GetDouble(1, nullptr));
				}

				_target_shx->write_int(_target_len, FileWriter::ByteOrder::BE);
				_target_len += record_len;
				_target_shx->write_int(24 + 8 * vtx_count, FileWriter::ByteOrder::BE);
				_write_dbf_record(_target_dbf);
			}
			else {
				auto record_len = 36 + 12 * vtx_count;
				if (record_len + _target_z_len >= 0x7FFFFFFF || record_len + _target_z_len < 0) {
					_start_new_export();
				}
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

				_target_z_shp->write_int(_target_z_number++, FileWriter::ByteOrder::BE);
				_target_z_shp->write_int(32 + 12 * vtx_count, FileWriter::ByteOrder::BE);
				_target_z_shp->write_int(13);
				_target_z_shp->write_double(x_min);
				_target_z_shp->write_double(y_min);
				_target_z_shp->write_double(x_max);
				_target_z_shp->write_double(y_max);
				_target_z_shp->write_int(1);
				_target_z_shp->write_int(vtx_count);
				_target_z_shp->write_int(0);
				for (decltype(vtx_count) i = 0; i < vtx_count; i++) {
					auto* vtx = vtx_col[i];
					_target_z_shp->write_double(vtx->GetDouble(0, nullptr));
					_target_z_shp->write_double(vtx->GetDouble(1, nullptr));
				}
				_target_z_shp->write_double(z_min);
				_target_z_shp->write_double(z_max);
				for (decltype(vtx_count) i = 0; i < vtx_count; i++) {
					auto* vtx = vtx_col[i];
					_target_z_shp->write_double(vtx->GetLength() == 3 ? vtx->GetDouble(2, nullptr) : 0.0);
				}

				_target_z_shx->write_int(_target_z_len, FileWriter::ByteOrder::BE);
				_target_z_len += record_len;
				_target_z_shx->write_int(32 + 12 * vtx_count, FileWriter::ByteOrder::BE);
				_write_dbf_record(_target_z_dbf);
			}
			return true;
		}

		template <typename U = T, typename = std::enable_if_t<std::is_same_v<U, cPolygon<cNTuple>>>>
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
				auto record_len = 28 + 8 * vtx_count;
				if (record_len + _target_len >= 0x7FFFFFFF || record_len + _target_len < 0) {
					_start_new_export();
				}
				_target_shp->write_int(_target_number++, FileWriter::ByteOrder::BE);
				_target_shp->write_int(24 + 8 * vtx_count, FileWriter::ByteOrder::BE);
				_target_shp->write_int(5);
				_target_shp->write_double(x_min);
				_target_shp->write_double(y_min);
				_target_shp->write_double(x_max);
				_target_shp->write_double(y_max);
				_target_shp->write_int(1);
				_target_shp->write_int(vtx_count);
				_target_shp->write_int(0);
				for (decltype(vtx_count) i = 0; i < vtx_count; i++) {
					auto* vtx = vtx_col[i];
					_target_shp->write_double(vtx->GetDouble(0, nullptr));
					_target_shp->write_double(vtx->GetDouble(1, nullptr));
				}

				_target_shx->write_int(_target_len, FileWriter::ByteOrder::BE);
				_target_len += record_len;
				_target_shx->write_int(24 + 8 * vtx_count, FileWriter::ByteOrder::BE);
				_write_dbf_record(_target_dbf);
			}
			else {
				auto record_len = 36 + 12 * vtx_count;
				if (record_len + _target_z_len >= 0x7FFFFFFF || record_len + _target_z_len < 0) {
					_start_new_export();
				}
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

				_target_z_shp->write_int(_target_z_number++, FileWriter::ByteOrder::BE);
				_target_z_shp->write_int(32 + 12 * vtx_count, FileWriter::ByteOrder::BE);
				_target_z_shp->write_int(15);
				_target_z_shp->write_double(x_min);
				_target_z_shp->write_double(y_min);
				_target_z_shp->write_double(x_max);
				_target_z_shp->write_double(y_max);
				_target_z_shp->write_int(1);
				_target_z_shp->write_int(vtx_count);
				_target_z_shp->write_int(0);
				for (decltype(vtx_count) i = 0; i < vtx_count; i++) {
					auto* vtx = vtx_col[i];
					_target_z_shp->write_double(vtx->GetDouble(0, nullptr));
					_target_z_shp->write_double(vtx->GetDouble(1, nullptr));
				}
				_target_z_shp->write_double(z_min);
				_target_z_shp->write_double(z_max);
				for (decltype(vtx_count) i = 0; i < vtx_count; i++) {
					auto* vtx = vtx_col[i];
					_target_z_shp->write_double(vtx->GetLength() == 3 ? vtx->GetDouble(2, nullptr) : 0.0);
				}

				_target_z_shx->write_int(_target_z_len, FileWriter::ByteOrder::BE);
				_target_z_len += record_len;
				_target_z_shx->write_int(32 + 12 * vtx_count, FileWriter::ByteOrder::BE);
				_write_dbf_record(_target_z_dbf);
			}
			return true;
		}

		template <typename U = T, typename = std::enable_if_t<std::is_same_v<U, cSphere<cNTuple>> || std::is_same_v<U, cNTuple> || std::is_same_v<U, cPoint<cNTuple>>>>
		bool export_item(cSphere<cNTuple>* sphere) {
			return export_item(sphere->GetVertex(0));
		}
	};
}