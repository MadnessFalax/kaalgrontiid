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
		NdAttrPossible
	};

	enum osmHandler {
		C0CK
	};

}