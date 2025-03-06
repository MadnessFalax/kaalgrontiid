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
}