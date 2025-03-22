#pragma once
#include "cstdio"

#include "pExporter.h"
#include "../container/pString.h"
#include "../container/pArray.h"

#include "cPolygon.h"
#include "cLineString.h"
#include "cSphere.h"

namespace nsGeoJSON {
	using String = nspString::pString;
	using FileWriter = nspFile::pFileWriter;

	class gjExporter : public nspExporter::pExporter {
		FileWriter* _file = nullptr;
		bool _first = true;

		void _begin_geometry_collection() {
			_file->write("{");
			_file->write("\"type\": \"GeometryCollection\",");
			_file->write("\"geometries\": [");
		}

		void _end_geometry_collection() {
			_file->write("]");
			_file->write("}");
		}


	public:
		gjExporter(String path) {
			_file = new FileWriter(path);
		}

		~gjExporter() {
			delete _file;
			_file = nullptr;
		};

		// dump initial data
		bool begin() override {
			_begin_geometry_collection();
			return true;
		}

		// dump final data
		bool end() override {
			_end_geometry_collection();
			return true;
		}

		bool export_item(cNTuple* point) {
			auto status = false;
			if (point != nullptr) {
				String output = "";
				if (!_first) {
					output += ",";
				}
				output += "{";
				output += "\"type\": \"Point\",";
				output += "\"coordinates\": [";
				auto pt_size = point->GetLength();
				for (unsigned char i = 0; i < pt_size; i++) {
					char* num = new char[32];
					if (i > 0) {
						output += ",";
					}
					snprintf(num, 32, "%f", point->GetDouble(i, nullptr));
					output += num;
					delete[] num;
				}
				output += "]";
				output += "}";

				status &= _file->write(output);
			}

			_first = false;
			return status;
		}

		bool export_item(cLineString<cNTuple>* line) {
			auto status = false;
			if (line != nullptr) {
				String output = "";
				if (!_first) {
					output += ",";
				}
				output += "{";
				output += "\"type\": \"LineString\",";
				output += "\"coordinates\": [";
				auto pt_size = line->GetVerticesCount();
				auto* vertices = line->GetVerticesCollection();
				for (unsigned int i = 0; i < pt_size; i++) {
					if (i > 0) {
						output += ",";
					}
					output += "[";
					auto* point = vertices[i];
					auto pt_size = point->GetLength();
					for (unsigned char j = 0; j < pt_size; j++) {
						char* num = new char[32];
						if (j > 0) {
							output += ",";
						}
						snprintf(num, 32, "%f", point->GetDouble(j, nullptr));
						output += num;
						delete[] num;
					}
					output += "]";
				}
				output += "]";
				output += "}";
				status &= _file->write(output);
			}
			_first = false;
			return status;
		}

		bool export_item(cPolygon<cNTuple>* polygon) {
			auto status = false;
			if (polygon != nullptr) {
				String output = "";
				if (!_first) {
					output += ",";
				}
				output += "{";
				output += "\"type\": \"Polygon\",";
				output += "\"coordinates\": [";
				auto pt_size = polygon->GetVerticesCount();
				auto* vertices = polygon->GetVerticesCollection();
				for (unsigned int i = 0; i < pt_size; i++) {
					if (i > 0) {
						output += ",";
					}
					output += "[";
					auto* point = vertices[i];
					auto pt_size = point->GetLength();
					for (unsigned char j = 0; j < pt_size; j++) {
						char* num = new char[32];
						if (j > 0) {
							output += ",";
						}
						snprintf(num, 32, "%f", point->GetDouble(j, nullptr));
						output += num;
						delete[] num;
					}
					output += "]";
				}
				output += "]";
				output += "}";
				status &= _file->write(output);
			}
			_first = false;
			return status;
		}

		// GeoJSON doesnt support sphere types, it is reduced to point
		bool export_item(cSphere<cNTuple>* sphere) {
			return export_item(sphere->GetVertex(0));
		}

	};
}