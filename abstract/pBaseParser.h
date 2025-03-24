#pragma once
#include "../container/pString.h"

#include "cPolygon.h"
#include "cLineString.h"
#include "cSphere.h"

class pBaseParser {
protected:
	using String = nspString::pString;

public:
	virtual ~pBaseParser() {};
	virtual DataShape get_shape_type() = 0;
	virtual cSpaceDescriptor* get_space_descriptor() = 0;
	virtual cDataType* get_item() = 0;
	virtual String get_parser_format() {
		return "BASE";
	}
};