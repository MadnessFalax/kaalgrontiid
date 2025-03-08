#pragma once
#include "../../../container/pString.h"
#include "../../../container/pMap.h"
#include "../../regex/pRegex.h"
#include "pTokenInstance.h"
#include "../../buffer/pFileHandler.h"

namespace nspLexer {
	using String = nspString::pString;
	using Regex = nspRegex::pRegex;
	template<class t>
	using Array = nspArray::pArray<t>;
	using FileHandler = nspFile::pFileHandler;
	template<class t, class u, class v = unsigned short> 
	using Map = nspMap::pMap<t, u, v>;

	template<class enum_t>
	class pLexer {
		using Token = pTokenPrototype<enum_t, PrototypeKind::REGEX>;
		using Instance = pTokenInstance<enum_t, PrototypeKind::REGEX>;

		Array<Token*> _tokens = Array<Token*>();
		Map<enum_t, Token*, unsigned char> _token_map = Map<enum_t, Token*, unsigned char>(29);
		FileHandler* _file = nullptr;
		String _path = "";
		bool _discard_whitespace = true;
		Token* _whitespace_token = nullptr;
		bool _utf8 = true;

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
		pLexer(bool discard_whitespace = true, bool utf8 = true) : _discard_whitespace(discard_whitespace), _utf8(utf8) {}

		~pLexer() {
			size_t size = _tokens.size();
			for (nspPair::pPair<enum_t, Token*>& pair : _tokens) {
				delete pair.second();
				pair.second() = nullptr;
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

		void add_token_definition(enum_t type, String pattern, String name, bool is_whitespace = false) {
			Token* t = new Token(type, pattern, name, _utf8);
			_tokens.push_back(t);
			_token_map[t->get_id()] = t;
			if (is_whitespace) {
				_whitespace_token = t;
			}
			t = nullptr;
		}
		
		Instance get_token(size_t lookahead_limit = 1) {
			if (_file) {
				String tmp = String();
				auto f_size = _file->size();
				auto t_count = _tokens.size();
				Token* selected_token = nullptr;
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
								if (_discard_whitespace && selected_token == _whitespace_token) {
									return get_token(lookahead_limit);
								}
								else {
									return Instance(selected_token, tmp, _file->position());
								}
							}
						}
					}
				}
			}
			return Instance(nullptr, "err_token", 0);			// either error or eof
		}
	};
}