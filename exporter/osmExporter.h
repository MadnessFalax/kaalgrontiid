#pragma once
#include "cstdio"

#include "pExporter.h"
#include "../container/pString.h"
#include "../container/pArray.h"
#include "../container/pMap.h"

namespace nsOSM {
	using String = nspString::pString;
	using FileWriter = nspFile::pFileWriter;
	template<class T>
	using Array = nspArray::pArray<T>;
	using Mapping = nspMap::pMap<size_t, Array<size_t>*>;

	class osmExporter : public nspExporter::pExporter {
		FileWriter* _file = nullptr;
		size_t _cur_id = 1;
		Array<Array<size_t>*> _mapping_mem = Array<Array<size_t>*>();
		Mapping _way_mappings = Mapping();

		bool _write_header() {
			String output = "";
			output += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
			return _file->write(output);
		}

		bool _begin_document() {
			String output = "";
			output += "<osm version=\"0.6\" generator=\"RadegastDB\">\n";
			return _file->write(output);
		}


		bool _end_document() {
			String output = "";
			char* num_buf = new char[32];
			for (auto& pair : _way_mappings) {
				output += "<way id=\"";
				snprintf(num_buf, 32, "%llu", pair.first());
				output += num_buf;
				output += "\">\n";
				for (unsigned int i = 0; i < pair.second()->size(); i++) {
					output += "<nd ref=\"";
					snprintf(num_buf, 32, "%llu", (*pair.second())[i]);
					output += num_buf;
					output += "\"/>\n";
				}
				output += "</way>\n";
			}
			delete[] num_buf;

			output += "</osm>\n";
			return _file->write(output);
		}

	public:
		osmExporter(String path) {
			_file = new FileWriter(path);
		}

		~osmExporter() {
			delete _file;
			_file = nullptr;

			auto mem_size = _mapping_mem.size();
			for (unsigned int i = 0; i < mem_size; i++) {
				delete _mapping_mem[i];
				_mapping_mem[i] = nullptr;
			}
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

			char* num_buf = new char[32];

			auto dim = point->GetLength();

			output += "<node id=\"";
			snprintf(num_buf, 32, "%llu", _cur_id++);
			output += num_buf;
			output += "\" lat=\"";
			snprintf(num_buf, 32, "%.7f", point->GetDouble(1, nullptr));
			output += num_buf;
			output += "\" lon=\"";
			snprintf(num_buf, 32, "%.7f", point->GetDouble(0, nullptr));
			output += num_buf;
			output += "\">\n";

			if (dim > 2) {
				output += "<tag k=\"ele\" v=\"";
				snprintf(num_buf, 32, "%.7f", point->GetDouble(2, nullptr));
				output += num_buf;
				output += "\"/>\n";
			}
			output += "</node>\n";


			delete[] num_buf;

			status &= _file->write(output);
			return status;
		}

		bool export_item(cPoint<cNTuple>* point) {
			auto status = false;
			String output = "";

			char* num_buf = new char[32];

			auto* pt = point->GetVertex(0);

			auto dim = pt->GetLength();

			output += "<node id=\"";
			snprintf(num_buf, 32, "%llu", _cur_id++);
			output += num_buf;
			output += "\" lat=\"";
			snprintf(num_buf, 32, "%.7f", pt->GetDouble(1, nullptr));
			output += num_buf;
			output += "\" lon=\"";
			snprintf(num_buf, 32, "%.7f", pt->GetDouble(0, nullptr));
			output += num_buf;
			output += "\">\n";

			if (dim > 2) {
				output += "<tag k=\"ele\" v=\"";
				snprintf(num_buf, 32, "%.7f", pt->GetDouble(2, nullptr));
				output += num_buf;
				output += "\"/>\n";
			}
			output += "</node>\n";


			delete[] num_buf;

			status &= _file->write(output);
			return status;
		}

		bool export_item(cLineString<cNTuple>* line) {
			auto status = false;

			auto pt_size = line->GetVerticesCount();
			auto* vertices = line->GetVerticesCollection();

			auto line_id = _cur_id++;
			auto nds = new Array<size_t>();

			for (unsigned int i = 0; i < pt_size; i++) {
				auto* point = vertices[i];
				nds->push_back(_cur_id);
				export_item(point);
			}

			_way_mappings[line_id] = nds;
			_mapping_mem.push_back(nds);

			return status;
		}

		bool export_item(cPolygon<cNTuple>* polygon) {
			auto status = false;

			auto pt_size = polygon->GetVerticesCount();
			auto* vertices = polygon->GetVerticesCollection();

			auto line_id = _cur_id++;
			auto nds = new Array<size_t>();

			for (unsigned int i = 0; i < pt_size; i++) {
				auto* point = vertices[i];
				nds->push_back(_cur_id);
				export_item(point);
			}

			_way_mappings[line_id] = nds;
			_mapping_mem.push_back(nds);

			return status;
		}

		// OSM doesnt support sphere types, it is reduced to point
		bool export_item(cSphere<cNTuple>* sphere) {
			return export_item(sphere->GetVertex(0));
		}

	};
}