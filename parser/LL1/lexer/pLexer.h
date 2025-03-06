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

		String _peek_char(size_t count) {
			String buf = String(count);
			for (size_t i = 0; i < count; i++) {
				buf += _file->get_char();
			}

			auto buf_count = buf.length();
			for (size_t i = 0; i < buf_count; i++) {
				_file->unget_char();
			}

			return buf;
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
		
		pTokenInstance<enum_t, pPrototypeKind::REGEX> get_token(size_t lookahead_limit = 1) {
			if (_file) {
				String tmp = String();
				auto f_size = _file->size();
				auto t_count = _tokens.size();
				pTokenPrototype<enum_t, pPrototypeKind::REGEX>* selected_token = nullptr;
				for (; _file->position() < f_size;) {
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
						bool matched = selected_token->match(tmp);
						if (!matched) {
							// match failed, resolve lookaheads
							size_t cur_lookahead = 1;
							String lookahead_str = String();
							for (; cur_lookahead <= lookahead_limit; cur_lookahead++) {
								lookahead_str = _peek_char(cur_lookahead);
								String in_place_str = tmp + lookahead_str;
								if (selected_token->match(in_place_str)) {
									tmp = in_place_str;
									_file->set_position(_file->position() + cur_lookahead);
									matched = true;
									break;
								}
							}
							if (!matched) {
								tmp--;
								_file->unget_char();
								return pTokenInstance<enum_t, pPrototypeKind::REGEX>(selected_token, tmp, _file->position());
							}
						}
					}
				}
			}
			return pTokenInstance<enum_t, pPrototypeKind::REGEX>(nullptr, "err_token", 0);			// either error or eof
		}
	};
}