#pragma once

#undef ERROR

namespace nsKML {
	enum kmlToken {
		ERROR,
		END,
		LANGLEBRACKET,
		RANGLEBRACKET,
		LBRACKET,
		RBRACKET,
		DASH,
		QMARK,
		EXCLAMATION,
		EQUALS,
		WS,
		QUOTE,
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
		CommentOrCDATA,
		CDATAContent,
		CDATAFirstRBracket,
		CDATASecondRBracket,
		PostInstruction,
		ConsumeWS,
		CommentTag,
		CommentContent,
		OptionalTextContent,
		NormalTag,
		OptionalAttr,
		OptionalAttrPossible,
		AttrValue,
		AttrValueQuoted,
		AttrValueDoubleQuoted,
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
		MakeNegative,
		BufferNumber,
		BufferPoint,
		CommitShape
	};

}