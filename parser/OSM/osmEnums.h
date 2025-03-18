#pragma once

#undef ERROR

namespace nsOSM {
	enum osmToken {
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

	enum osmRule {
		EntryRule,
		OSM,
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
		AttrPossible,
		AttrValue,
		AttrValueQuoted,
		AttrValueDoubleQuoted,
		ExtractableAttrValue,
		ExtractableAttrValueQuoted,
		ExtractableAttrValueDoubleQuoted,
		GenericSingleOrPairTag,
		OptionalPairTagAnyContent,
		NestedOrEndingTag,
		NodeAttr,
		NodeAttrPossible,
		WayAttr,
		WayAttrPossible,
		WaySingleOrPairTag,
		OptionalPairTagWayContent,
		WayNestedOrEndingTag,
		AnyWayNestedTagContent,
		NormalWayNestedTag,
		NdAttr,
		NdAttrPossible,
		FinalSequence
	};

	enum osmHandler {
		ChangeSign,
		SetLongitude,
		SetLatitude,
		SetId,
		SetRef,
		PointsToArray,
		CommitPoint,
		CommitShape,
		DisposePoints
	};

}