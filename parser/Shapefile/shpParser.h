#pragma once
#include "../../abstract/pBaseParser.h"
#include "../buffer/pFileHandler.h"
#include "shpFileHeader.h"
#include "shpRecordIndex.h"
#include "shpRecordHeader.h"
#include "shpRecordContent.h"

#include "shape/shpPoint.h"
#include "shape/shpPointM.h"
#include "shape/shpPointZ.h"
#include "shape/shpMultiPoint.h"
#include "shape/shpMultiPointM.h"
#include "shape/shpMultiPointZ.h"
#include "shape/shpPolygon.h"
#include "shape/shpPolygonM.h"
#include "shape/shpPolygonZ.h"
#include "shape/shpPolyLine.h"
#include "shape/shpPolyLineM.h"
#include "shape/shpPolyLineZ.h"
#include "shape/shpMultiPatch.h"

namespace nsShapeFile {

	using FileHandler = nspFile::pFileHandler;

	class shpParser : protected pBaseParser {
		
	private:
		cSpaceDescriptor* _space_descriptor = nullptr;
		FileHandler* _shx = nullptr;
		FileHandler* _shp = nullptr;
		DataShape _shape_type = DataShape{};
		cDataType* _item = nullptr;
		shpFileHeader _shp_header = shpFileHeader();
		shpFileHeader _shx_header = shpFileHeader();
		shpRecordHeader _record_header = shpRecordHeader();
		shpRecordContent* _record_content = nullptr;
		shpRecordIndex _record_index = shpRecordIndex();

	public:
		shpParser() {}

		~shpParser() {
			delete _shx;
			_shx = nullptr;
			delete _shp;
			_shp = nullptr;
			delete _space_descriptor;
			_space_descriptor = nullptr;
			delete _item;
			_item = nullptr;
			delete _record_content;
			_record_content = nullptr;
		}

		void open(String shp, String shx) {
			delete _shx;
			_shx = new FileHandler(shx, true);
			delete _shp;
			_shp = new FileHandler(shp, true);
			delete _space_descriptor;
			_space_descriptor = nullptr;
			delete _item;
			_item = nullptr;
			delete _record_content;
			_record_content = nullptr;
			
			_shp_header.load(_shp);
			_shx_header.load(_shx);

			switch (_shp_header.en_shape_type) {
			case shpShapeType::POINT:
				_space_descriptor = new cSpaceDescriptor(DIMENSION_2, new cNTuple(), new cDouble());
				_shape_type = DataShape::DS_POINT;
				_record_content = new shpPoint();
				break;
			case shpShapeType::POLYLINE:
				_space_descriptor = new cSpaceDescriptor(DIMENSION_2, new cNTuple(), new cDouble());
				_shape_type = DataShape::DS_LINESTRING;
				_record_content = new shpPolyLine();
				break;
			case shpShapeType::POLYGON:
				_space_descriptor = new cSpaceDescriptor(DIMENSION_2, new cNTuple(), new cDouble());
				_shape_type = DataShape::DS_POLYGON;
				_record_content = new shpPolygon();
				break;
			case shpShapeType::MULTIPOINT:
				_space_descriptor = new cSpaceDescriptor(DIMENSION_2, new cNTuple(), new cDouble());
				_shape_type = DataShape::DS_POINT;
				_record_content = new shpMultiPoint();
				break;
			case shpShapeType::POINTZ:
				_space_descriptor = new cSpaceDescriptor(DIMENSION_3, new cNTuple(), new cDouble());
				_shape_type = DataShape::DS_POINT;
				_record_content = new shpPointZ();
				break;
			case shpShapeType::POLYLINEZ:
				_space_descriptor = new cSpaceDescriptor(DIMENSION_3, new cNTuple(), new cDouble());
				_shape_type = DataShape::DS_LINESTRING;
				_record_content = new shpPolyLineZ();
				break;
			case shpShapeType::POLYGONZ:
				_space_descriptor = new cSpaceDescriptor(DIMENSION_3, new cNTuple(), new cDouble());
				_shape_type = DataShape::DS_POLYGON;
				_record_content = new shpPolygonZ();
				break;
			case shpShapeType::MULTIPOINTZ:
				_space_descriptor = new cSpaceDescriptor(DIMENSION_3, new cNTuple(), new cDouble());
				_shape_type = DataShape::DS_POINT;
				_record_content = new shpMultiPointZ();
				break;
			case shpShapeType::POINTM:
				_space_descriptor = new cSpaceDescriptor(DIMENSION_2, new cNTuple(), new cDouble());
				_shape_type = DataShape::DS_POINT;
				_record_content = new shpPointM();
				break;
			case shpShapeType::POLYLINEM:
				_space_descriptor = new cSpaceDescriptor(DIMENSION_2, new cNTuple(), new cDouble());
				_shape_type = DataShape::DS_LINESTRING;
				_record_content = new shpPolyLineM();
				break;
			case shpShapeType::POLYGONM:
				_space_descriptor = new cSpaceDescriptor(DIMENSION_2, new cNTuple(), new cDouble());
				_shape_type = DataShape::DS_POLYGON;
				_record_content = new shpPolygonM();
				break;
			case shpShapeType::MULTIPOINTM:
				_space_descriptor = new cSpaceDescriptor(DIMENSION_2, new cNTuple(), new cDouble());
				_shape_type = DataShape::DS_POINT;
				_record_content = new shpMultiPointM();
				break;
			case shpShapeType::MULTIPATCH:
				_space_descriptor = new cSpaceDescriptor(DIMENSION_3, new cNTuple(), new cDouble());
				_shape_type = DataShape::DS_POLYGON;
				_record_content = new shpMultiPatch();
				break;
			}
		}

		DataShape get_shape_type() override {
			return _shape_type;
		}

		cSpaceDescriptor* get_space_descriptor() override {
			return _space_descriptor;
		}

		cDataType* get_item() override {
			if (_record_content) {
				auto* possible_item = _record_content->get_item();
				if (possible_item) {
					return possible_item;
				}
			}
			_record_index.load(_shx);
			if (_record_index.offset != 0) {
				_record_header.load(_shp);
				_record_content->load(_shp, _record_header);

				auto* item = _record_content->get_item();
				return item;
			}
			return nullptr;
		}

		String get_parser_format() override {
			return "SHAPEFILE";
		}
	};

}