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
		COMMA,
		ATTRIBUTE,
		NUMBER,
		STRING
	};

	enum kmlRule {
		EntryRule,
		KML,
		AnyTagContent,
		PostInstruction,
		ConsumeWS,
		CommentTag,
		CommentContent,
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
		CoordinatesNestedOrEndingTag,
		Coord,
		CoordTupleTail,
		NextTuple
	};

	enum kmlHandler {

	};

}