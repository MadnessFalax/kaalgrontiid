#pragma once
#undef TRUE
#undef FALSE
#undef ERROR

namespace nsGeoJSON {
	enum gjToken {
		ERROR,
		END,
		WS,
		LBRACE,
		RBRACE,
		LBRACKET,
		RBRACKET,
		COMMA,
		COLON,
		STRING,
		TRUE,
		FALSE,
		GJNULL,
		NUMBER
	};

	enum gjRule {
		EntryRule,
		GeoJSON,
		GJProperties,
		GJPropertiesTail,
		GJProperty,
		GeometryType,
		TypeVal,
		GenericProperties,
		GenericPropertiesTail,
		GenericProperty,
		FeatureValues,
		FeatureValuesTail,
		FProperties,
		FPropertiesTail,
		FProperty,
		GProperties,
		GPropertiesTail,
		GProperty,
		CoordinatesRoot,
		CoordinatesRootTail,
		Coordinates,
		CoordinatesTail,
		Value,
		GenericArrayValues,
		GenericArrayValuesTail,
		GeometryValues,
		GeometryValuesTail,
		OptionalGeometryValue
	};

	enum gjHandler {
		BufferNumber,
		BufferPoint,
		CommitShape,
		DepthOut,
	};
}