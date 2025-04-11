#pragma once
#include "cstdio"

#include "pExporter.h"
#include "../container/pString.h"
#include "../container/pArray.h"

namespace nsKML {
	using String = nspString::pString;
	using FileWriter = nspFile::pFileWriter;

	class kmlExporter : public nspExporter::pExporter {
		FileWriter* _file = nullptr;

		bool _write_header() {
			String output = "";
			output += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";	
			return _file->write(output);
		}

		bool _begin_document() {
			String output = "";
			output += "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n";
			output += "<Document>\n";
			return _file->write(output);
		}


		bool _end_document() {
			String output = "";
			output += "</Document>\n";
			output += "</kml>\n";
			return _file->write(output);
		}

	public:
		kmlExporter(String path) {
			_file = new FileWriter(path);
		}

		~kmlExporter() {
			delete _file;
			_file = nullptr;
		};

		// dump initial data
		bool begin() override {
			_write_header();
			_begin_document();
			return true;
		}

		// dump final data
		bool end() override {
			_end_document();
			return true;
		}

		bool export_item(cNTuple* point) {
			auto status = false;
			String output = "";
			output += "<Placemark>\n";
			output += "<Point>\n";
			output += "<coordinates>";

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

			output += "</coordinates>\n";
			output += "</Point>\n";
			output += "</Placemark>\n";

			status &= _file->write(output);
			return status;
		}

		bool export_item(cPoint<cNTuple>* point) {
			auto status = false;
			String output = "";
			output += "<Placemark>\n";
			output += "<Point>\n";
			output += "<coordinates>";

			auto pt = point->GetVertex(0);

			auto pt_size = pt->GetLength();
			for (unsigned char i = 0; i < pt_size; i++) {
				char* num = new char[32];
				if (i > 0) {
					output += ",";
				}
				snprintf(num, 32, "%f", pt->GetDouble(i, nullptr));
				output += num;
				delete[] num;
			}

			output += "</coordinates>\n";
			output += "</Point>\n";
			output += "</Placemark>\n";

			status &= _file->write(output);
			return status;
		}

		bool export_item(cLineString<cNTuple>* line) {
			auto status = false;
			String output = "";
			output += "<Placemark>\n";
			output += "<LineString>\n";
			output += "<coordinates>\n";

			auto pt_size = line->GetVerticesCount();
			auto* vertices = line->GetVerticesCollection();

			for (unsigned int i = 0; i < pt_size; i++) {
				if (i > 0) {
					output += "\n";
				}
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
			}

			output += "</coordinates>\n";
			output += "</LineString>\n";
			output += "</Placemark>\n";

			status &= _file->write(output);
			return status;
		}

		bool export_item(cPolygon<cNTuple>* polygon) {
			auto status = false;

			String output = "";
			output += "<Placemark>\n";
			output += "<Polygon>\n";
			output += "<outerBoundaryIs>\n";
			output += "<LinearRing>\n";
			output += "<coordinates>\n";

			auto pt_size = polygon->GetVerticesCount();
			auto* vertices = polygon->GetVerticesCollection();

			for (unsigned int i = 0; i < pt_size; i++) {
				if (i > 0) {
					output += "\n";
				}
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
			}

			output += "</coordinates>\n";
			output += "</LinearRing>\n";
			output += "</outerBoundaryIs>\n";
			output += "</Polygon>\n";
			output += "</Placemark>\n";

			status &= _file->write(output);

			return status;
		}

		// KML doesnt support sphere types, it is reduced to point
		bool export_item(cSphere<cNTuple>* sphere) {
			return export_item(sphere->GetVertex(0));
		}

	};
}