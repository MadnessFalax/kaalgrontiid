#pragma once
#undef TRUE
#undef FALSE
#undef NULL
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
		NULL,
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
		Coordinates,
		CoordinatesTail,
		Value,
		GenericArrayValues,
		GenericArrayValuesTail
	};
}