#pragma once
#include <cstdio>
#include "../../container/pString.h"

#ifdef _DEBUG
#define fread(a, b, c, d) fread_s(a, c + 1, b, c, d)
#define clearerr(a) clearerr_s(a)
#define open(file, filename, mode, err) err = fopen_s(&file, filename, mode)
#endif

#ifndef _DEBUG
#define open(file, filename, mode, err) file = fopen(filename, mode)
#endif



namespace nspFile {

	using String = nspString::pString;
	
	class pFileHandler {
		bool _is_open = false;
		String* _filename = nullptr;
		FILE* _file = nullptr;
		bool _is_binary = false;
		int _err_type = 0;

		void _handle_error() {
			printf("An error has occured during file operation: %i", _err_type);
			clearerr(_file);
		}

		void _open() {
			if (!_is_open) {
				open(_file, _filename->c_str(), _is_binary ? "rb" : "r", _err_type);
				if (_file == nullptr) {
					_handle_error();
				}
				else {
					_is_open = true;
				}
			}
		}

		void _close() {
			if (_is_open) {
				if (_file) {
					fclose(_file);
					_file = nullptr;
				}
				_is_open = false;
			}
		}

	public:
		pFileHandler() = delete;

		pFileHandler(char* filename, bool binary = false) : _filename(new String(filename)), _is_binary(binary) {
			_open();
		}
		
		pFileHandler(const char* filename, bool binary = false) : _filename(new String(filename)), _is_binary(binary) {
			_open();
		}

		pFileHandler(pFileHandler& other) = delete;

		pFileHandler(pFileHandler&& other) noexcept
			: _is_open(other._is_open),
			_filename(other._filename),
			_file(other._file),
			_is_binary(other._is_binary),
			_err_type(other._err_type)
		{
			other._filename = nullptr;
			other._file = nullptr;
		}

		~pFileHandler() {
			_close();
			delete _filename;
		}

		pFileHandler& operator=(pFileHandler& other) = delete;
		
		pFileHandler& operator=(pFileHandler&& other) noexcept {
			if (this != &other) {
				_is_open = other._is_open;
				_is_binary = other._is_binary;
				_file = other._file;
				_filename = other._filename;
				_err_type = other._err_type;
				other._file = nullptr;
				other._filename = nullptr;
			}

			return *this;
		}

		String get_string(size_t byte_size) {
			_open();
			char* buf = new char[byte_size + 1];
			memset(buf, '\0', byte_size + 1);
			size_t count = fread(buf, sizeof(char), byte_size, _file);
			if (count < byte_size) {
				if (!feof(_file)) {
					_err_type = ferror(_file);
					_handle_error();
				}
			}

			auto ret_val = String(buf);

			delete[] buf;

			return ret_val;
		}

		char get_char() {
			_open();
			char read_val = fgetc(_file);
			if (read_val == EOF) {
				read_val = '\0';
				if (!feof(_file)) {
					_err_type = ferror(_file);
					_handle_error();
				}
			}

			return read_val;
		}
	};
}