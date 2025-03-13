#pragma once

#undef ERROR

namespace nsKML {
	enum kmlToken {
		ERROR,
		END,
		LANGLEBRACKET,
		RANGLEBRACKET,
		DASH,
		QMARK,
		EXCLAMATION,
		EQUALS,
		WS,
		DOUBLEQUOTE,
		SLASH,
		NUMBER,
		STRING
	};

	enum kmlRule {
		KML,
		AnyTagContent,
		InstructionTag,
		CommentTag,
		OptionalTextContent,
		NormalTag,
		OptionalAttr,
		AttrSepOptional,
		NextAttrPossible,
		GenericSingleOrPairTag,
		OptionalPairTagAnyContent,
		NestedOrEndingTag,
		GeometrySingleOrPairTag,
		OptionalPairTagGeometryContent,
		GeometryNestedOrEndingTag,
		AnyGeometryNestedTagContent,
		NormalGeometryNestedTag,
		CoordinatesSingleOrPairTag,
		OptionalPairTagCoordinatesContent,
		CoordinatesEndingTag,
		CoordTuple,
		CoordTupleTail,
		NextTuple
	};

	enum kmlHandler {

	};

}