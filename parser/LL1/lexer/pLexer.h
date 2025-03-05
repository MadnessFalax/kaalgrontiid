#pragma once
#include "../../../container/pString.h"
#include "../../regex/pRegex.h"
#include "pTokenInstance.h"
#include "../../buffer/pFileHandler.h"

namespace nspLexer {
	using String = nspString::pString;
	using Regex = nspRegex::pRegex;
	template<class t>
	using Array = nspArray::pArray<t>;
	using FileHandler = nspFile::pFileHandler;

	template<class enum_t>
	class pLexer {
		Array<pTokenPrototype<enum_t, pPrototypeKind::REGEX>*> _tokens = Array<pTokenPrototype<enum_t, pPrototypeKind::REGEX>*>();
		FileHandler* _file = nullptr;
		String _path = "";

		char _peek_char() {
			char c = _file->get_char();
			_file->unget_char();
			return c;
		}

	public:
		pLexer() {}

		~pLexer() {
			size_t size = _tokens.size();
			for (size_t i = 0; i < size; i++) {
				delete _tokens[i];
				_tokens[i] = nullptr;
			}
			delete _file;
			_file = nullptr;
		}

		void open(String& path) {
			if (_file) {
				delete _file;
				_file = nullptr;
			}
			_file = new FileHandler(path);
		}

		void add_token_definition(enum_t type, String pattern) {
			_tokens.push_back(new pTokenPrototype<enum_t, pPrototypeKind::REGEX>(type, pattern));
		}
		
		pTokenInstance<enum_t, pPrototypeKind::REGEX> get_token() {
			if (_file) {
				String tmp = String();
				auto f_size = _file->size();
				auto t_count = _tokens.size();
				pTokenPrototype<enum_t, pPrototypeKind::REGEX>* selected_token = nullptr;

				for (auto i = _file->position(); i < f_size; i++) {
					tmp += _file->get_char();
					if (!selected_token) {
						for (auto j = 0; j < t_count; j++) {
							if (_tokens[j]->match(tmp)) {
								selected_token = _tokens[j];
								break;
							}
						}
					}
					if (selected_token) {
						if (!selected_token->match(tmp)) {
							tmp--;
							_file->unget_char();
							return pTokenInstance<enum_t, pPrototypeKind::REGEX>(selected_token, tmp, _file->position());
						}
					}
				}
			}
			return pTokenInstance<enum_t, pPrototypeKind::REGEX>(nullptr, "err_token", 0);
		}
	};
}